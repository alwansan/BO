// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;
import java.util.function.LongSupplier;

/**
 * Detached objective snapshot bodies. This class does not assign a room packet
 * type, change the room protocol, send TLS traffic, or read/write game objects.
 *
 * A body is an 80-byte header followed by at most 26 canonical 16-byte records.
 * The header contains the existing 36-byte big-endian story ticket, a positive
 * big-endian sequence, unsigned 16-bit total count and part index, and SHA-256 of
 * all record bytes. A record contains an opaque eight-byte ID, then little-endian
 * unsigned counter and state values bounded by 65535 and 3 respectively.
 * SHA-256 detects accidental or conflicting assembly; it is not authentication.
 */
public final class RoomObjectiveChunks {
    public static final int HEADER_BYTES = 80;
    public static final int RECORD_BYTES = 16;
    public static final int RECORDS_PER_PART = 26;
    public static final int MAX_RECORDS = 4096;
    public static final int MAX_BODY_BYTES = RoomWire.MAX_FRAME - 8;

    private RoomObjectiveChunks() {}

    // Shared with RoomTls's pre-READY binding. Never derive this table from a
    // received body. Validation and copying do not create an owner-thread
    // assembler; the TLS reader creates that exactly once for the binding.
    static byte[][] copyExpectedIds(byte[][] independentlyExpectedIds) throws IOException {
        if (independentlyExpectedIds == null || independentlyExpectedIds.length > MAX_RECORDS) {
            throw new IOException("Invalid independent objective table");
        }
        byte[][] result = new byte[independentlyExpectedIds.length][];
        Set<Long> unique = new HashSet<Long>();
        for (int i = 0; i < result.length; i++) {
            byte[] id = independentlyExpectedIds[i];
            if (id == null || id.length != 8) {
                throw new IOException("Objective ID must contain eight bytes");
            }
            byte[] copy = id.clone();
            if (!unique.add(Long.valueOf(idBits(copy)))) {
                throw new IOException("Independent objective IDs must be unique");
            }
            result[i] = copy;
        }
        return result;
    }

    public enum Result { REJECTED, PENDING, COMPLETE }

    /** An immutable, canonical body sequence; every body accessor returns a copy. */
    public static final class EncodedSnapshot {
        private final byte[][] bodies;

        private EncodedSnapshot(byte[][] ownedBodies) {
            bodies = ownedBodies;
        }

        public int partCount() {
            return bodies.length;
        }

        public byte[] body(int index) {
            return bodies[index].clone();
        }
    }

    /** A fully validated detached value, not evidence that an engine applied it. */
    public static final class Snapshot {
        public final RoomStoryWire.Ticket ticket;
        public final long sequence;
        private final byte[] records;

        private Snapshot(RoomStoryWire.Ticket value, long serial, byte[] ownedRecords) {
            ticket = value;
            sequence = serial;
            records = ownedRecords;
        }

        public byte[] records() {
            return records.clone();
        }
    }

    /**
     * Encodes values only. Duplicate IDs can be represented here; the receiver
     * must obtain its unique expected ID table independently of these bodies.
     */
    public static EncodedSnapshot encode(RoomStoryWire.Ticket ticket, long sequence,
                                         byte[] records) throws IOException {
        if (ticket == null || sequence <= 0 || records == null ||
                records.length % RECORD_BYTES != 0 ||
                records.length / RECORD_BYTES > MAX_RECORDS) {
            throw new IOException("Invalid objective snapshot bounds");
        }
        byte[] copy = records.clone();
        validateScalars(copy, 0, copy.length / RECORD_BYTES);
        int total = copy.length / RECORD_BYTES;
        int parts = partCount(total);
        byte[] digest = sha256(copy);
        byte[][] bodies = new byte[parts][];
        for (int part = 0; part < parts; part++) {
            int count = recordsInPart(total, part);
            ByteArrayOutputStream bytes = new ByteArrayOutputStream(
                    HEADER_BYTES + count * RECORD_BYTES);
            DataOutputStream out = new DataOutputStream(bytes);
            writeTicket(out, ticket);
            out.writeLong(sequence);
            out.writeShort(total);
            out.writeShort(part);
            out.write(digest);
            out.write(copy, part * RECORDS_PER_PART * RECORD_BYTES,
                    count * RECORD_BYTES);
            bodies[part] = bytes.toByteArray();
            if (bodies[part].length > MAX_BODY_BYTES) {
                throw new IOException("Objective body exceeds room frame budget");
            }
        }
        return new EncodedSnapshot(bodies);
    }

    /**
     * One owner-thread-confined assembly for one exact story ticket. It retains
     * at most one pending record buffer (65536 bytes) and one completed snapshot.
     * Calls from another thread reject/return null; abort there has no effect.
     * Callers must hand bodies to the owner thread and must not mutate an input
     * array concurrently with a call.
     *
     * A valid part zero starts a strictly newer sequence. There is no reordering or
     * retry of a discarded sequence. Assembly expires fifteen seconds after its
     * first accepted part; duplicates never renew that deadline. The last
     * completed snapshot may remain visible while a newer snapshot is pending.
     * Long.MAX_VALUE is the last sequence for this ticket; sequence never wraps.
     * A new instance loses this instance's high-water mark. Recreating one for
     * the same ticket is not a replay-safe reset and needs external lifecycle
     * policy. Neither this watermark nor the hash authenticates the sender.
     */
    public static final class Assembler {
        private final Thread owner = Thread.currentThread();
        private final LongSupplier clock;
        private static final long ASSEMBLY_TTL_NS=15000000000L;
        private final RoomStoryWire.Ticket ticket;
        private final byte[][] expectedIds;
        private boolean revoked;
        private long highWater;
        private Pending pending;
        private Snapshot completed;

        public Assembler(RoomStoryWire.Ticket value, byte[][] independentlyExpectedIds)
                throws IOException {
            this(value, independentlyExpectedIds, System::nanoTime);
        }

        // Package-private deterministic clock for synthetic deadline tests.
        Assembler(RoomStoryWire.Ticket value, byte[][] independentlyExpectedIds,
                  LongSupplier monotonicClock) throws IOException {
            if (value == null || monotonicClock == null)
                throw new IOException("Missing objective story ticket or clock");
            clock = monotonicClock;
            ticket = value; // Ticket is immutable and owns its session bytes.
            expectedIds = copyExpectedIds(independentlyExpectedIds);
        }

        public Result accept(RoomStoryWire.Ticket currentTicket, byte[] body) {
            if (Thread.currentThread() != owner || !active(currentTicket)) {
                return Result.REJECTED;
            }
            final long at = clock.getAsLong();
            if (pending != null) {
                long age = at - pending.startedAtNs;
                if (age < 0 || age > ASSEMBLY_TTL_NS) pending = null;
                // Keep highWater: an expired sequence must never restart.
            }
            if (body == null || body.length < 44) {
                return Result.REJECTED; // No complete scope + sequence to correlate.
            }
            // Bound the copy even for an oversized caller-supplied array. Its
            // prefix can still identify malformed data for the pending sequence.
            byte[] copy = Arrays.copyOf(body, Math.min(body.length, MAX_BODY_BYTES + 1));
            final long sequence;
            final DataInputStream in = new DataInputStream(new ByteArrayInputStream(copy));
            try {
                RoomStoryWire.Ticket packetTicket = readTicket(in);
                sequence = in.readLong();
                if (!ticket.same(packetTicket) || sequence <= 0) {
                    return Result.REJECTED;
                }
            } catch (IOException malformedScope) {
                return Result.REJECTED;
            }

            final int total;
            final int part;
            final byte[] digest = new byte[32];
            try {
                if (body.length > MAX_BODY_BYTES || copy.length < HEADER_BYTES) {
                    throw new IOException("Invalid objective body length");
                }
                total = in.readUnsignedShort();
                part = in.readUnsignedShort();
                in.readFully(digest);
                if (total > MAX_RECORDS || total != expectedIds.length ||
                        part >= partCount(total) ||
                        copy.length != HEADER_BYTES + recordsInPart(total, part) * RECORD_BYTES) {
                    throw new IOException("Noncanonical objective part");
                }
                int count = recordsInPart(total, part);
                validateScalars(copy, HEADER_BYTES, count);
                validateIds(copy, HEADER_BYTES, part * RECORDS_PER_PART, count);
            } catch (IOException malformedBody) {
                discardIfPending(sequence);
                return Result.REJECTED;
            }

            if ((completed != null && sequence <= completed.sequence) || sequence < highWater) {
                return Result.REJECTED;
            }
            if (sequence > highWater) {
                if (part != 0) {
                    return Result.REJECTED; // Orphan does not replace state or move the watermark.
                }
                // Allocate before replacing the current pending state.
                Pending next = new Pending(sequence, total, digest, at);
                pending = next;
                highWater = sequence;
            } else if (pending == null) {
                return Result.REJECTED; // Discarded/completed sequence cannot restart.
            }

            if (pending.total != total || !MessageDigest.isEqual(pending.digest, digest)) {
                pending = null;
                return Result.REJECTED;
            }
            int offset = part * RECORDS_PER_PART * RECORD_BYTES;
            int size = copy.length - HEADER_BYTES;
            if (part < pending.nextPart) {
                // Matching canonical metadata plus payload means an exact duplicate.
                if (!rangeEquals(copy, HEADER_BYTES, pending.records, offset, size)) {
                    pending = null;
                }
                return Result.REJECTED;
            }
            if (part != pending.nextPart) {
                pending = null;
                return Result.REJECTED;
            }
            System.arraycopy(copy, HEADER_BYTES, pending.records, offset, size);
            pending.nextPart++;
            if (pending.nextPart != partCount(total)) {
                return Result.PENDING;
            }
            try {
                validateScalars(pending.records, 0, total);
                validateIds(pending.records, 0, 0, total);
                if (!MessageDigest.isEqual(pending.digest, sha256(pending.records))) {
                    pending = null;
                    return Result.REJECTED;
                }
            } catch (IOException invalidCompletedValue) {
                pending = null;
                return Result.REJECTED;
            }
            completed = new Snapshot(ticket, sequence, pending.records);
            pending = null; // Transfer sole ownership of the record buffer to Snapshot.
            return Result.COMPLETE;
        }

        public Snapshot latest(RoomStoryWire.Ticket currentTicket) {
            if (Thread.currentThread() != owner || !active(currentTicket)) {
                return null;
            }
            return completed;
        }

        public void abort() {
            if (Thread.currentThread() == owner) {
                revoke();
            }
        }

        private boolean active(RoomStoryWire.Ticket currentTicket) {
            if (revoked) {
                return false;
            }
            if (!ticket.same(currentTicket)) {
                revoke();
                return false;
            }
            return true;
        }

        private void revoke() {
            revoked = true;
            pending = null;
            completed = null;
        }

        private void discardIfPending(long sequence) {
            if (pending != null && pending.sequence == sequence) {
                pending = null;
            }
        }

        private void validateIds(byte[] bytes, int offset, int first, int count)
                throws IOException {
            for (int i = 0; i < count; i++) {
                if (!rangeEquals(bytes, offset + i * RECORD_BYTES, expectedIds[first + i], 0, 8)) {
                    throw new IOException("Objective ID differs from independent table");
                }
            }
        }
    }

    private static final class Pending {
        final long sequence,startedAtNs;
        final int total;
        final byte[] digest;
        final byte[] records;
        int nextPart;

        Pending(long serial, int count, byte[] hash, long at) {
            startedAtNs = at;
            sequence = serial;
            total = count;
            digest = hash;
            records = new byte[count * RECORD_BYTES];
        }
    }

    private static int partCount(int total) {
        return Math.max(1, (total + RECORDS_PER_PART - 1) / RECORDS_PER_PART);
    }

    private static int recordsInPart(int total, int part) {
        return Math.min(RECORDS_PER_PART, total - part * RECORDS_PER_PART);
    }

    private static void writeTicket(DataOutputStream out, RoomStoryWire.Ticket ticket)
            throws IOException {
        out.write(ticket.session());
        out.writeLong(ticket.token);
        out.writeInt((int) ticket.rosterRevision);
        out.writeByte(ticket.mission);
        out.writeShort(ticket.stage);
        out.writeInt(ticket.sceneEpoch);
        out.writeByte(ticket.participants);
    }

    private static RoomStoryWire.Ticket readTicket(DataInputStream in) throws IOException {
        byte[] session = new byte[16];
        in.readFully(session);
        long token = in.readLong();
        long revision = in.readInt() & 0xffffffffL;
        int mission = in.readUnsignedByte();
        int stage = in.readUnsignedShort();
        int epoch = in.readInt();
        int participants = in.readUnsignedByte();
        return new RoomStoryWire.Ticket(session, token, revision, mission, stage, epoch, participants);
    }

    private static void validateScalars(byte[] bytes, int offset, int count) throws IOException {
        for (int i = 0; i < count; i++) {
            int base = offset + i * RECORD_BYTES;
            if (littleUnsigned(bytes, base + 8) > 65535L ||
                    littleUnsigned(bytes, base + 12) > 3L) {
                throw new IOException("Invalid objective counter or state");
            }
        }
    }

    private static long littleUnsigned(byte[] bytes, int offset) {
        return (bytes[offset] & 255L) | ((bytes[offset + 1] & 255L) << 8) |
                ((bytes[offset + 2] & 255L) << 16) | ((bytes[offset + 3] & 255L) << 24);
    }

    private static long idBits(byte[] id) {
        long value = 0;
        for (byte element : id) {
            value = (value << 8) | (element & 255L);
        }
        return value;
    }

    private static boolean rangeEquals(byte[] a, int aOffset, byte[] b, int bOffset, int count) {
        for (int i = 0; i < count; i++) {
            if (a[aOffset + i] != b[bOffset + i]) {
                return false;
            }
        }
        return true;
    }

    private static byte[] sha256(byte[] bytes) throws IOException {
        try {
            return MessageDigest.getInstance("SHA-256").digest(bytes);
        } catch (NoSuchAlgorithmException unavailable) {
            throw new IOException("SHA-256 unavailable", unavailable);
        }
    }
}
