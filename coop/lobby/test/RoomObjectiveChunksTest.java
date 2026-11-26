// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;

/** Synthetic value tests only: no TLS sockets, Android, or original engine. */
public final class RoomObjectiveChunksTest {
    private static RoomStoryWire.Ticket ticket;
    private static byte[] hostRecords;
    private static final byte[][] guestRecords = new byte[3][];
    private static int scenarios;

    private RoomObjectiveChunksTest() {}

    private interface Checked {
        void run() throws Exception;
    }

    public static void main(String[] args) throws Exception {
        if (args.length != 2) {
            throw new IllegalArgumentException("Usage: RoomObjectiveChunksTest host-records.bin output-dir");
        }
        ticket = fixtureTicket();
        hostRecords = Files.readAllBytes(Paths.get(args[0]));
        run("golden_and_canonical_boundaries", RoomObjectiveChunksTest::goldenAndBoundaries);
        run("encoding_validation", RoomObjectiveChunksTest::encodingValidation);
        run("independent_identity", RoomObjectiveChunksTest::independentIdentity);
        run("three_guest_fixture", RoomObjectiveChunksTest::threeGuestFixture);
        run("pending_and_supersession", RoomObjectiveChunksTest::pendingAndSupersession);
        run("ordering_and_orphans", RoomObjectiveChunksTest::orderingAndOrphans);
        run("exact_and_conflicting_duplicates", RoomObjectiveChunksTest::duplicates);
        run("digest_and_same_sequence_mix", RoomObjectiveChunksTest::digestAndMix);
        run("malformed_same_sequence", RoomObjectiveChunksTest::malformedSameSequence);
        run("all_scope_fields", RoomObjectiveChunksTest::allScopeFields);
        run("abort_and_owner_thread", RoomObjectiveChunksTest::abortAndOwnerThread);
        run("copies_and_sequence_limit", RoomObjectiveChunksTest::copiesAndSequenceLimit);
        check(scenarios == 12, "Every named scenario must run before emitting outputs");
        Path output = Paths.get(args[1]);
        Files.createDirectories(output);
        for (int guest = 0; guest < 3; guest++) {
            check(guestRecords[guest] != null, "Completed guest fixture missing");
            Files.write(output.resolve("guest-" + (guest + 2) + ".bin"), guestRecords[guest]);
        }
        System.out.println("OBJECTIVE_CHUNKS_JAVA_PASS scenarios=" + scenarios +
                " parts=158 guests=3 SYNTHETIC_BODY_ONLY_NO_TLS_ANDROID_ENGINE");
    }

    private static void goldenAndBoundaries() throws Exception {
        // Fixed independent Python struct/hashlib oracle supplied with the design.
        byte[] golden = hex("808182838485868788898a8b8c8d8e8f0102030405060708" +
                "ffffffff07ffff7fffffff0f000000000000000100010000" +
                "a44f869b720be79dafbfd94382036b7bd2406e1c162d632f74cb4a67a081b37e" +
                "00000000000000800000000000000000");
        equal(golden, encode(1, 1, 0).body(0), "Fixed 96-byte golden body");
        check(golden.length == 96, "Golden length");
        int[] counts = {0, 1, 26, 27, 4096};
        int[] expectedParts = {1, 1, 1, 2, 158};
        for (int n = 0; n < counts.length; n++) {
            int count = counts[n];
            RoomObjectiveChunks.EncodedSnapshot encoded = encode(1, count, 0);
            check(encoded.partCount() == expectedParts[n], "Independent canonical part count");
            RoomObjectiveChunks.Assembler assembler = assembler(count);
            for (int part = 0; part < encoded.partCount(); part++) {
                int localCount = Math.min(26, count - part * 26);
                byte[] body = encoded.body(part);
                check(body.length == 80 + localCount * 16, "Canonical body length");
                check(body.length <= 504, "Room body budget");
                check(unsigned16(body, 44) == count && unsigned16(body, 46) == part,
                        "Unsigned total and part fields");
                boolean last = part + 1 == encoded.partCount();
                expect(last ? RoomObjectiveChunks.Result.COMPLETE : RoomObjectiveChunks.Result.PENDING,
                        assembler.accept(ticket, body), "Boundary assembly");
                check(last || assembler.latest(ticket) == null, "No boundary partial publication");
            }
            equal(records(count, 0), assembler.latest(ticket).records(), "Boundary record bytes");
        }
        check(encode(1, 4096, 0).body(157).length == 80 + 14 * 16, "Final part has fourteen records");
        check(encode(1, 26, 0).body(0).length == 496, "Full body 496, outer frame would be 504");
    }

    private static void encodingValidation() throws Exception {
        bad(() -> RoomObjectiveChunks.encode(null, 1, records(1, 0)), "Null ticket");
        bad(() -> RoomObjectiveChunks.encode(ticket, 1, null), "Null records");
        for (long serial : new long[] {0, -1, Long.MIN_VALUE}) {
            bad(() -> encode(serial, 1, 0), "Nonpositive sequence");
        }
        for (int length : new int[] {1, 15, 17}) {
            bad(() -> RoomObjectiveChunks.encode(ticket, 1, new byte[length]), "Partial record");
        }
        bad(() -> encode(1, 4097, 0), "Record count 4097");
        for (long value : new long[] {65536, 0xffffffffL}) {
            byte[] invalid = records(1, 0);
            little32(invalid, 8, value);
            bad(() -> RoomObjectiveChunks.encode(ticket, 1, invalid), "Counter overflow");
        }
        for (long value : new long[] {4, 0xffffffffL}) {
            byte[] invalid = records(1, 0);
            little32(invalid, 12, value);
            bad(() -> RoomObjectiveChunks.encode(ticket, 1, invalid), "State overflow");
        }
        byte[] limits = records(1, 0);
        little32(limits, 8, 65535);
        little32(limits, 12, 3);
        for (int participants : new int[] {3, 5, 9, 13, 15}) {
            RoomStoryWire.Ticket wide = new RoomStoryWire.Ticket(new byte[16], Long.MAX_VALUE,
                    1, 0, 0, 1, participants);
            RoomObjectiveChunks.Assembler assembler = new RoomObjectiveChunks.Assembler(wide, ids(1));
            expect(RoomObjectiveChunks.Result.COMPLETE,
                    assembler.accept(wide, RoomObjectiveChunks.encode(wide, 1, limits).body(0)),
                    "Real Ticket bounds include zero session and sparse masks");
            equal(limits, assembler.latest(wide).records(), "Maximum unsigned scalar values");
        }
    }

    private static void independentIdentity() throws Exception {
        bad(() -> new RoomObjectiveChunks.Assembler(null, ids(1)), "Null bound ticket");
        bad(() -> new RoomObjectiveChunks.Assembler(ticket, null), "Missing independent table");
        bad(() -> new RoomObjectiveChunks.Assembler(ticket, new byte[4097][8]), "Independent count overflow");
        bad(() -> new RoomObjectiveChunks.Assembler(ticket, new byte[][] {null}), "Null independent ID");
        for (int length : new int[] {0, 7, 9}) {
            bad(() -> new RoomObjectiveChunks.Assembler(ticket, new byte[][] {new byte[length]}),
                    "Independent ID length");
        }
        byte[][] duplicateIds = ids(2);
        duplicateIds[1] = duplicateIds[0].clone();
        bad(() -> new RoomObjectiveChunks.Assembler(ticket, duplicateIds), "Duplicate expected IDs");
        byte[] duplicates = records(2, 0);
        System.arraycopy(duplicates, 0, duplicates, 16, 8);
        RoomObjectiveChunks.EncodedSnapshot duplicateBody = RoomObjectiveChunks.encode(ticket, 1, duplicates);
        check(duplicateBody.partCount() == 1, "Encoder permits duplicate IDs as raw values");
        expect(RoomObjectiveChunks.Result.REJECTED,
                assembler(2).accept(ticket, duplicateBody.body(0)), "Independent table rejects duplicate values");
        expect(RoomObjectiveChunks.Result.REJECTED,
                assembler(26).accept(ticket, encode(1, 27, 0).body(0)), "Independent count mismatch");
        byte[][] swapped = ids(2);
        byte[] first = swapped[0];
        swapped[0] = swapped[1];
        swapped[1] = first;
        expect(RoomObjectiveChunks.Result.REJECTED,
                new RoomObjectiveChunks.Assembler(ticket, swapped).accept(ticket, encode(1, 2, 0).body(0)),
                "Expected IDs bind descriptor order");
        byte[][] mutableIds = ids(2);
        RoomObjectiveChunks.Assembler copied = new RoomObjectiveChunks.Assembler(ticket, mutableIds);
        mutableIds[0] = new byte[8];
        mutableIds[1][0] ^= 32;
        expect(RoomObjectiveChunks.Result.COMPLETE, copied.accept(ticket, encode(1, 2, 0).body(0)),
                "Independent ID table is deeply copied");
    }

    private static void threeGuestFixture() throws Exception {
        check(hostRecords.length == 4096 * 16, "C++ host fixture must contain exactly 4096 records");
        // Check the documented fixture directly; do not derive the independent
        // guest descriptor IDs from incoming records or from the body codec.
        for (int i = 0; i < 4096; i++) {
            int base = i * 16;
            check((hostRecords[base] & 255) == (i & 255) &&
                    (hostRecords[base + 1] & 255) == (i >>> 8) &&
                    (hostRecords[base + 7] & 255) == 128, "C++ fixture ID bytes");
            for (int j = 2; j < 7; j++) {
                check(hostRecords[base + j] == 0, "C++ fixture zero ID bytes");
            }
            check(unsignedLittle32(hostRecords, base + 8) == i &&
                    unsignedLittle32(hostRecords, base + 12) == i % 4, "C++ fixture scalar values");
        }
        RoomObjectiveChunks.EncodedSnapshot encoded = RoomObjectiveChunks.encode(ticket, 1, hostRecords);
        check(encoded.partCount() == 158, "4096 records split into 158 bodies");
        RoomObjectiveChunks.Assembler[] guests = {
            assembler(4096), assembler(4096), assembler(4096)
        };
        for (int part = 0; part < 158; part++) {
            for (RoomObjectiveChunks.Assembler guest : guests) {
                byte[] body = encoded.body(part);
                expect(part == 157 ? RoomObjectiveChunks.Result.COMPLETE : RoomObjectiveChunks.Result.PENDING,
                        guest.accept(ticket, body), "Synthetic host-to-guest body delivery");
                Arrays.fill(body, (byte) 0); // The receiver must not retain this caller-owned body.
                check(part == 157 || guest.latest(ticket) == null, "No guest publishes partial records");
            }
        }
        for (int guest = 0; guest < 3; guest++) {
            RoomObjectiveChunks.Snapshot complete = guests[guest].latest(ticket);
            check(complete != null && complete.sequence == 1 && complete.ticket.same(ticket),
                    "Guest completes exact ticket and sequence");
            guestRecords[guest] = complete.records();
            equal(hostRecords, guestRecords[guest], "Unmodified completed C++ fixture bytes");
        }
    }

    private static void pendingAndSupersession() throws Exception {
        RoomObjectiveChunks.Assembler assembler = assembler(27);
        RoomObjectiveChunks.EncodedSnapshot one = encode(1, 27, 0);
        complete(assembler, one);
        RoomObjectiveChunks.EncodedSnapshot two = encode(2, 27, 1);
        RoomObjectiveChunks.EncodedSnapshot three = encode(3, 27, 2);
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, two.body(0)), "New pending sequence");
        check(assembler.latest(ticket).sequence == 1, "Previous complete remains visible during pending");
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, three.body(0)), "Higher part zero replaces pending");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, two.body(1)), "Older pending cannot complete");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, one.body(0)), "Completed replay rejects");
        expect(RoomObjectiveChunks.Result.COMPLETE, assembler.accept(ticket, three.body(1)), "Newer sequence completes");
        equal(records(27, 2), assembler.latest(ticket).records(), "Only new completed value published");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, three.body(0)), "Completed sequence cannot restart");
    }

    private static void orderingAndOrphans() throws Exception {
        RoomObjectiveChunks.Assembler assembler = assembler(53);
        RoomObjectiveChunks.EncodedSnapshot one = encode(1, 53, 0);
        RoomObjectiveChunks.EncodedSnapshot two = encode(2, 53, 1);
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, two.body(1)), "Initial orphan rejected");
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, one.body(0)), "Initial orphan did not raise high-water");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, two.body(1)), "Higher orphan rejected");
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, one.body(1)), "Higher orphan did not drop current pending");
        expect(RoomObjectiveChunks.Result.COMPLETE, assembler.accept(ticket, one.body(2)), "Current sequence completes after orphan");
        complete(assembler, two); // Also proves orphan sequence remains available for a valid part zero.
        RoomObjectiveChunks.EncodedSnapshot three = encode(3, 53, 2);
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, three.body(0)), "Gap setup");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, three.body(2)), "Gap discards pending");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, three.body(1)), "Missing part cannot repair gap");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, three.body(0)), "Gap retains high-water");
        check(assembler.latest(ticket).sequence == 2, "Gap preserves previous complete");
        complete(assembler, encode(4, 53, 3));
        check(assembler.latest(ticket).sequence == 4, "Higher sequence recovers after gap");
    }

    private static void duplicates() throws Exception {
        RoomObjectiveChunks.Assembler assembler = assembler(53);
        RoomObjectiveChunks.EncodedSnapshot one = encode(1, 53, 0);
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, one.body(0)), "Duplicate setup");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, one.body(0)), "Exact duplicate zero rejects");
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, one.body(1)), "Exact duplicate did not advance or discard");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, one.body(1)), "Exact duplicate middle rejects");
        expect(RoomObjectiveChunks.Result.COMPLETE, assembler.accept(ticket, one.body(2)), "Exact duplicates preserve pending");
        RoomObjectiveChunks.EncodedSnapshot two = encode(2, 53, 0);
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, two.body(0)), "Conflicting duplicate setup");
        byte[] conflict = two.body(0);
        little32(conflict, 80 + 8, 12);
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, conflict), "Conflicting duplicate discards pending");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, two.body(1)), "Conflicted sequence cannot continue");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, two.body(0)), "Conflicted sequence cannot restart");
        check(assembler.latest(ticket).sequence == 1, "Conflicting duplicate preserves previous complete");
        complete(assembler, encode(3, 53, 1));
    }

    private static void digestAndMix() throws Exception {
        RoomObjectiveChunks.Assembler assembler = assembler(27);
        RoomObjectiveChunks.EncodedSnapshot a = encode(1, 27, 0);
        RoomObjectiveChunks.EncodedSnapshot b = encode(1, 27, 10);
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, a.body(0)), "Metadata mismatch setup");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, b.body(1)), "Same sequence different digest discards");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, a.body(1)), "Digest mismatch leaves no reusable pending");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, a.body(0)), "Digest mismatch retains watermark");
        a = encode(2, 27, 0);
        b = encode(2, 27, 10);
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, a.body(0)), "Nonoverlapping mix setup");
        byte[] mixed = b.body(1);
        System.arraycopy(a.body(0), 48, mixed, 48, 32);
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, mixed),
                "Whole digest rejects mixed values even with rewritten per-part digest");
        check(assembler.latest(ticket) == null, "Mixed snapshot never publishes");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, a.body(0)), "Failed full digest cannot restart same sequence");
        complete(assembler, encode(3, 27, 0));
        RoomObjectiveChunks.Assembler single = assembler(1);
        byte[] damagedDigest = encode(1, 1, 0).body(0);
        damagedDigest[48] ^= 1;
        expect(RoomObjectiveChunks.Result.REJECTED, single.accept(ticket, damagedDigest), "Single part digest failure");
        expect(RoomObjectiveChunks.Result.REJECTED, single.accept(ticket, encode(1, 1, 0).body(0)),
                "Failed single part retains watermark");
        complete(single, encode(2, 1, 0));
    }

    private static void malformedSameSequence() throws Exception {
        for (int kind = 0; kind < 12; kind++) {
            RoomObjectiveChunks.Assembler assembler = assembler(27);
            complete(assembler, encode(1, 27, 0));
            RoomObjectiveChunks.EncodedSnapshot two = encode(2, 27, 1);
            expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, two.body(0)), "Malformed setup");
            byte[] malformed = two.body(1);
            switch (kind) {
                case 0: big16(malformed, 44, 4097); break;
                case 1: big16(malformed, 44, 26); break;
                case 2: big16(malformed, 46, 2); break;
                case 3: malformed = Arrays.copyOf(malformed, 79); break;
                case 4: malformed = Arrays.copyOf(malformed, malformed.length + 1); break;
                case 5: malformed = Arrays.copyOf(malformed, 505); break;
                case 6: little32(malformed, 88, 65536); break;
                case 7: little32(malformed, 92, 4); break;
                case 8: malformed[80] ^= 1; break;
                case 9: malformed[48] ^= 1; break;
                case 10: little32(malformed, 88, 0xffffffffL); break;
                case 11: little32(malformed, 92, 0xffffffffL); break;
                default: throw new AssertionError("Unreachable mutation");
            }
            expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, malformed), "Malformed bound case " + kind);
            check(assembler.latest(ticket).sequence == 1, "Malformed pending preserves complete");
            expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, two.body(1)), "Malformed same sequence drops pending");
            expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, two.body(0)), "Malformed same sequence retains high-water");
            complete(assembler, encode(3, 27, 2));
        }
        RoomObjectiveChunks.Assembler assembler = assembler(27);
        RoomObjectiveChunks.EncodedSnapshot one = encode(1, 27, 0);
        RoomObjectiveChunks.EncodedSnapshot two = encode(2, 27, 1);
        expect(RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, one.body(0)), "Invalid higher setup");
        byte[] invalidHigher = two.body(0);
        little32(invalidHigher, 88, 65536);
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, invalidHigher), "Invalid higher part zero rejects");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, null), "Null body rejects");
        expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, new byte[43]), "Uncorrelatable short body rejects");
        for (int field = 0; field < 7; field++) {
            byte[] invalidScope = two.body(0);
            switch (field) {
                case 0: Arrays.fill(invalidScope, 16, 24, (byte) 0); break;
                case 1: Arrays.fill(invalidScope, 24, 28, (byte) 0); break;
                case 2: invalidScope[28] = 8; break;
                case 3: Arrays.fill(invalidScope, 31, 35, (byte) 0); break;
                case 4: invalidScope[35] = 2; break;
                case 5: Arrays.fill(invalidScope, 36, 44, (byte) 0); break;
                case 6: Arrays.fill(invalidScope, 36, 44, (byte) 255); break;
                default: throw new AssertionError("Unreachable invalid scope");
            }
            expect(RoomObjectiveChunks.Result.REJECTED, assembler.accept(ticket, invalidScope), "Invalid ticket/sequence rejects");
        }
        expect(RoomObjectiveChunks.Result.COMPLETE, assembler.accept(ticket, one.body(1)),
                "Invalid higher or uncorrelatable data did not replace valid pending");
        complete(assembler, two); // Invalid higher part zero did not raise high-water either.
    }

    private static void allScopeFields() throws Exception {
        for (int field = 0; field < 7; field++) {
            RoomStoryWire.Ticket foreign = differentTicket(field);
            check(!ticket.same(foreign), "Each alternate changes exactly one valid scope field");
            RoomObjectiveChunks.Assembler preserved = assembler(27);
            complete(preserved, encode(1, 27, 0));
            RoomObjectiveChunks.EncodedSnapshot two = encode(2, 27, 1);
            expect(RoomObjectiveChunks.Result.PENDING, preserved.accept(ticket, two.body(0)), "Foreign scope setup");
            byte[] foreignBody = RoomObjectiveChunks.encode(foreign, 2, records(27, 1)).body(1);
            expect(RoomObjectiveChunks.Result.REJECTED, preserved.accept(ticket, foreignBody), "Foreign packet rejected");
            check(preserved.latest(ticket).sequence == 1, "Foreign packet preserves complete");
            expect(RoomObjectiveChunks.Result.COMPLETE, preserved.accept(ticket, two.body(1)), "Foreign packet preserves pending");

            RoomObjectiveChunks.Assembler viaAccept = assembler(27);
            complete(viaAccept, encode(1, 27, 0));
            expect(RoomObjectiveChunks.Result.PENDING, viaAccept.accept(ticket, two.body(0)), "Current mismatch setup");
            expect(RoomObjectiveChunks.Result.REJECTED, viaAccept.accept(foreign, two.body(1)), "Current scope mismatch rejects");
            check(viaAccept.latest(ticket) == null, "Current mismatch clears complete");
            expect(RoomObjectiveChunks.Result.REJECTED, viaAccept.accept(ticket, encode(3, 27, 0).body(0)),
                    "Current mismatch permanently revokes assembler");

            RoomObjectiveChunks.Assembler viaLatest = assembler(1);
            complete(viaLatest, encode(1, 1, 0));
            check(viaLatest.latest(foreign) == null && viaLatest.latest(ticket) == null,
                    "Latest with changed current scope permanently revokes");
            expect(RoomObjectiveChunks.Result.REJECTED, viaLatest.accept(ticket, encode(2, 1, 0).body(0)),
                    "Latest scope mismatch cannot later resume");
        }
    }

    private static void abortAndOwnerThread() throws Exception {
        final RoomObjectiveChunks.Assembler owner = assembler(27);
        complete(owner, encode(1, 27, 0));
        final RoomObjectiveChunks.EncodedSnapshot two = encode(2, 27, 1);
        expect(RoomObjectiveChunks.Result.PENDING, owner.accept(ticket, two.body(0)), "Owner setup");
        final RoomObjectiveChunks.Result[] foreignResult = new RoomObjectiveChunks.Result[1];
        final RoomObjectiveChunks.Snapshot[] foreignSnapshot = new RoomObjectiveChunks.Snapshot[1];
        final Throwable[] failure = new Throwable[1];
        Thread foreign = new Thread(() -> {
            try {
                foreignResult[0] = owner.accept(null, two.body(1));
                foreignSnapshot[0] = owner.latest(null);
                owner.abort();
            } catch (Throwable problem) {
                failure[0] = problem;
            }
        }, "objective-nonowner-test");
        foreign.start();
        foreign.join();
        check(failure[0] == null, "Non-owner calls must reject without throwing");
        expect(RoomObjectiveChunks.Result.REJECTED, foreignResult[0], "Non-owner accept rejects");
        check(foreignSnapshot[0] == null && owner.latest(ticket).sequence == 1,
                "Non-owner cannot read or revoke owner state");
        expect(RoomObjectiveChunks.Result.COMPLETE, owner.accept(ticket, two.body(1)), "Owner pending remains intact");
        owner.abort();
        check(owner.latest(ticket) == null, "Owner abort clears complete");
        expect(RoomObjectiveChunks.Result.REJECTED, owner.accept(ticket, encode(3, 27, 0).body(0)), "Abort is permanent");
        owner.abort();

        RoomObjectiveChunks.Assembler pending = assembler(27);
        expect(RoomObjectiveChunks.Result.PENDING, pending.accept(ticket, encode(1, 27, 0).body(0)), "Pending abort setup");
        pending.abort();
        expect(RoomObjectiveChunks.Result.REJECTED, pending.accept(ticket, encode(1, 27, 0).body(1)), "Abort clears pending");
        RoomObjectiveChunks.Assembler nullAccept = assembler(1);
        complete(nullAccept, encode(1, 1, 0));
        expect(RoomObjectiveChunks.Result.REJECTED, nullAccept.accept(null, encode(2, 1, 0).body(0)), "Null current revokes via accept");
        check(nullAccept.latest(ticket) == null, "Null current removed completed snapshot");
        RoomObjectiveChunks.Assembler nullLatest = assembler(1);
        complete(nullLatest, encode(1, 1, 0));
        check(nullLatest.latest(null) == null && nullLatest.latest(ticket) == null, "Null current revokes via latest");
        expect(RoomObjectiveChunks.Result.REJECTED, nullLatest.accept(ticket, encode(2, 1, 0).body(0)), "Null scope revocation is permanent");
    }

    private static void copiesAndSequenceLimit() throws Exception {
        byte[] source = records(27, 0);
        RoomObjectiveChunks.EncodedSnapshot encoded = RoomObjectiveChunks.encode(ticket, 1, source);
        byte[] originalBody = encoded.body(0);
        Arrays.fill(source, (byte) 0);
        byte[] bodyCopy = encoded.body(0);
        Arrays.fill(bodyCopy, (byte) 0);
        equal(originalBody, encoded.body(0), "Encoded body and source ownership are independent");
        RoomObjectiveChunks.Assembler assembler = assembler(27);
        complete(assembler, encoded);
        RoomObjectiveChunks.Snapshot one = assembler.latest(ticket);
        byte[] recordCopy = one.records();
        Arrays.fill(recordCopy, (byte) 0);
        equal(records(27, 0), one.records(), "Completed records accessor returns a copy");
        byte[] sessionCopy = one.ticket.session();
        Arrays.fill(sessionCopy, (byte) 0);
        check(one.ticket.same(ticket), "Completed ticket keeps its own session bytes");
        complete(assembler, encode(2, 27, 1));
        equal(records(27, 0), one.records(), "Published older immutable value survives later completion");
        equal(records(27, 1), assembler.latest(ticket).records(), "New complete has separate storage");
        assembler.abort();
        equal(records(27, 0), one.records(), "An already returned immutable value survives revocation");

        RoomObjectiveChunks.Assembler limit = assembler(1);
        complete(limit, encode(Long.MAX_VALUE - 1, 1, 0));
        complete(limit, encode(Long.MAX_VALUE, 1, 1));
        check(limit.latest(ticket).sequence == Long.MAX_VALUE, "Maximum sequence completes without overflow");
        for (long serial : new long[] {1, Long.MAX_VALUE - 1, Long.MAX_VALUE}) {
            expect(RoomObjectiveChunks.Result.REJECTED, limit.accept(ticket, encode(serial, 1, 0).body(0)),
                    "No older value or same maximum sequence can restart");
        }
        byte[] wrapped = encode(Long.MAX_VALUE, 1, 1).body(0);
        wrapped[36] = (byte) 0x80;
        Arrays.fill(wrapped, 37, 44, (byte) 0);
        expect(RoomObjectiveChunks.Result.REJECTED, limit.accept(ticket, wrapped), "Negative wrap is invalid");
        check(limit.latest(ticket).sequence == Long.MAX_VALUE, "Rejected wrap preserves maximum complete");
    }

    private static RoomStoryWire.Ticket fixtureTicket() throws IOException {
        byte[] session = new byte[16];
        for (int i = 0; i < session.length; i++) {
            session[i] = (byte) (0x80 + i);
        }
        return new RoomStoryWire.Ticket(session, 0x0102030405060708L, 0xffffffffL,
                7, 65535, Integer.MAX_VALUE, 15);
    }

    private static RoomStoryWire.Ticket differentTicket(int field) throws IOException {
        byte[] session = ticket.session();
        if (field == 0) {
            session[0] ^= 1;
        }
        return new RoomStoryWire.Ticket(session,
                field == 1 ? ticket.token + 1 : ticket.token,
                field == 2 ? ticket.rosterRevision - 1 : ticket.rosterRevision,
                field == 3 ? 6 : ticket.mission,
                field == 4 ? 65534 : ticket.stage,
                field == 5 ? Integer.MAX_VALUE - 1 : ticket.sceneEpoch,
                field == 6 ? 13 : ticket.participants);
    }

    private static RoomObjectiveChunks.Assembler assembler(int count) throws IOException {
        return new RoomObjectiveChunks.Assembler(ticket, ids(count));
    }

    private static RoomObjectiveChunks.EncodedSnapshot encode(long serial, int count, int delta)
            throws IOException {
        return RoomObjectiveChunks.encode(ticket, serial, records(count, delta));
    }

    private static void complete(RoomObjectiveChunks.Assembler assembler,
                                 RoomObjectiveChunks.EncodedSnapshot encoded) {
        for (int part = 0; part < encoded.partCount(); part++) {
            expect(part + 1 == encoded.partCount() ? RoomObjectiveChunks.Result.COMPLETE :
                    RoomObjectiveChunks.Result.PENDING, assembler.accept(ticket, encoded.body(part)),
                    "Complete canonical sequence");
        }
    }

    /** Independent fixture descriptor table, never derived from incoming records. */
    private static byte[][] ids(int count) {
        byte[][] value = new byte[count][8];
        for (int i = 0; i < count; i++) {
            value[i][0] = (byte) (i & 255);
            value[i][1] = (byte) (i >>> 8);
            value[i][7] = (byte) 0x80;
        }
        return value;
    }

    private static byte[] records(int count, int delta) {
        byte[] value = new byte[count * 16];
        for (int i = 0; i < count; i++) {
            int base = i * 16;
            value[base] = (byte) (i & 255);
            value[base + 1] = (byte) (i >>> 8);
            value[base + 7] = (byte) 0x80;
            little32(value, base + 8, i + delta);
            little32(value, base + 12, i % 4);
        }
        return value;
    }

    private static void run(String name, Checked test) throws Exception {
        test.run();
        scenarios++;
        System.out.println("PASS " + name);
    }

    private static void bad(Checked call, String message) throws Exception {
        try {
            call.run();
        } catch (IOException expected) {
            return;
        }
        throw new AssertionError(message + ": expected IOException");
    }

    private static void expect(RoomObjectiveChunks.Result expected,
                               RoomObjectiveChunks.Result actual, String message) {
        check(expected == actual, message + ": expected " + expected + ", got " + actual);
    }

    private static void equal(byte[] expected, byte[] actual, String message) {
        check(Arrays.equals(expected, actual), message);
    }

    private static void check(boolean condition, String message) {
        if (!condition) {
            throw new AssertionError(message);
        }
    }

    private static int unsigned16(byte[] bytes, int offset) {
        return ((bytes[offset] & 255) << 8) | (bytes[offset + 1] & 255);
    }

    private static long unsignedLittle32(byte[] bytes, int offset) {
        return (bytes[offset] & 255L) | ((bytes[offset + 1] & 255L) << 8) |
                ((bytes[offset + 2] & 255L) << 16) | ((bytes[offset + 3] & 255L) << 24);
    }

    private static void big16(byte[] bytes, int offset, int value) {
        bytes[offset] = (byte) (value >>> 8);
        bytes[offset + 1] = (byte) value;
    }

    private static void little32(byte[] bytes, int offset, long value) {
        for (int byteIndex = 0; byteIndex < 4; byteIndex++) {
            bytes[offset + byteIndex] = (byte) (value >>> (8 * byteIndex));
        }
    }

    private static byte[] hex(String value) {
        check((value.length() & 1) == 0, "Hex oracle has an even length");
        byte[] bytes = new byte[value.length() / 2];
        for (int i = 0; i < bytes.length; i++) {
            bytes[i] = (byte) Integer.parseInt(value.substring(i * 2, i * 2 + 2), 16);
        }
        return bytes;
    }
}
