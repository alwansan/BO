// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLException;
import javax.net.ssl.SSLSocket;

/**
 * Actual pinned TLS sockets with a synthetic objective producer and independent
 * fixture ID tables. No BLACK source, game-thread callback, engine or Android.
 */
public final class RoomObjectiveTlsTest {
    private static final int MAX_RECORDS = 4096;
    private static final int MAX_PARTS = 158;
    private static final String FIRST_SENT = "OBJECTIVE_HOST_FIRST_PART_SENT";
    private static int scenarios;

    private RoomObjectiveTlsTest() {}

    private interface Condition { boolean ready() throws Exception; }

    private static void require(boolean value, String message) {
        if (!value) throw new AssertionError(message);
    }

    private static void until(String label, Condition condition) throws Exception {
        long deadline = System.nanoTime() + TimeUnit.SECONDS.toNanos(12);
        while (!condition.ready()) {
            if (System.nanoTime() >= deadline) throw new AssertionError(label + " timed out");
            Thread.sleep(10);
        }
    }

    private static void callbackSucceeded(AtomicReference<Throwable> failure) {
        Throwable value = failure.get();
        if (value != null) throw new AssertionError("TLS diagnostic callback failed", value);
    }

    private static void passed(String name) {
        scenarios++;
        System.out.println("OBJECTIVE_TLS_CASE_PASS " + name);
    }

    // These IDs are defined by the fixture specification, never parsed from a
    // received chunk, a completed snapshot, or the host's record byte array.
    private static byte[][] expectedIds(int count) {
        byte[][] ids = new byte[count][8];
        for (int i = 0; i < count; i++) {
            ids[i][0] = (byte) i;
            ids[i][1] = (byte) (i >>> 8);
            ids[i][7] = (byte) 0x80;
        }
        return ids;
    }

    private static byte[] records(int count, int seed) {
        ByteBuffer out = ByteBuffer.allocate(count * 16).order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < count; i++) {
            out.putLong(0x8000000000000000L | i);
            out.putInt((i + seed * 97) & 65535);
            out.putInt((i + seed) & 3);
        }
        return out.array();
    }

    private static void verify(RoomObjectiveChunks.Snapshot snapshot,
                               RoomStoryWire.Ticket ticket, long sequence,
                               int count, int seed) {
        require(snapshot != null && snapshot.ticket.same(ticket), "exact snapshot ticket");
        require(snapshot.sequence == sequence, "exact snapshot sequence");
        byte[] value = snapshot.records();
        require(value.length == count * 16, "complete record cardinality");
        ByteBuffer in = ByteBuffer.wrap(value).order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < count; i++) {
            require(in.getLong() == (0x8000000000000000L | i), "every objective ID");
            require(in.getInt() == ((i + seed * 97) & 65535), "every objective counter");
            require(in.getInt() == ((i + seed) & 3), "every objective state");
        }
        if (value.length > 0) {
            value[0] ^= 0x7f;
            require(Arrays.equals(snapshot.records(), records(count, seed)),
                    "completed bytes are defensively owned");
        }
    }

    private static boolean allAt(List<RoomTls.Client> guests, RoomStoryWire.Ticket ticket,
                                 long sequence) {
        for (RoomTls.Client guest : guests) {
            RoomObjectiveChunks.Snapshot snapshot = guest.latestObjectiveSnapshot(ticket);
            if (snapshot == null || snapshot.sequence != sequence) return false;
        }
        return true;
    }

    private static void prepared(List<RoomTls.Client> guests, RoomStoryWire.Ticket ticket)
            throws Exception {
        until("all prepared", () -> {
            for (RoomTls.Client guest : guests)
                if (guest.storyTicket() == null || !guest.storyTicket().same(ticket)) return false;
            return true;
        });
    }

    private static void bind(List<RoomTls.Client> guests, RoomStoryWire.Ticket ticket,
                             int count) throws Exception {
        for (RoomTls.Client guest : guests) {
            byte[][] ids = expectedIds(count);
            require(guest.bindObjectiveIds(ticket, ids), "independent IDs bound before READY");
            if (count > 0) ids[0][7] ^= 1;
            require(!guest.bindObjectiveIds(ticket, expectedIds(count)),
                    "same-ticket rebinding cannot reset the receive watermark");
        }
    }

    private static void commit(RoomTls.Server host, List<RoomTls.Client> guests,
                               RoomStoryWire.Ticket ticket) throws Exception {
        require(!host.hostSceneLoaded(ticket.mission, ticket.stage, ticket.sceneEpoch),
                "host waits for guest READY");
        for (RoomTls.Client guest : guests)
            require(guest.acknowledgeSceneLoaded(ticket.mission, ticket.stage, ticket.sceneEpoch),
                    "fixture scene acknowledgement enqueued");
        until("all committed", () -> {
            if (!host.storyCommitted()) return false;
            for (RoomTls.Client guest : guests) if (!guest.storyCommitted()) return false;
            return true;
        });
    }

    private static void fourPeerLifecycle(SSLContext hostContext, SSLContext guestContext,
                                          InetAddress loop) throws Exception {
        List<RoomTls.Client> guests = new ArrayList<>();
        try (RoomTls.Server host = RoomTls.Server.start(hostContext, loop, 0, "Host", "Objectives")) {
            try {
                for (int i = 0; i < 3; i++)
                    guests.add(RoomTls.Client.connect(guestContext, loop, host.port(), "Guest " + (i + 2)));
                until("four-person roster", () -> {
                    if (host.snapshot().players.size() != 4) return false;
                    for (RoomTls.Client guest : guests)
                        if (guest.snapshot() == null || guest.snapshot().players.size() != 4) return false;
                    return true;
                });
                RoomStoryWire.Ticket ticket = host.prepareStory(0, 0, 11);
                prepared(guests, ticket);
                require(!host.offerObjectiveSnapshot(ticket, 1, records(MAX_RECORDS, 0)),
                        "objectives cannot be offered before COMMIT");
                bind(guests, ticket, MAX_RECORDS);
                for (RoomTls.Client guest : guests)
                    require(guest.latestObjectiveSnapshot(ticket) == null, "no pre-COMMIT value");
                commit(host, guests, ticket);
                require(!guests.get(0).bindObjectiveIds(ticket, expectedIds(MAX_RECORDS)),
                        "committed binding cannot be replaced");
                require(!host.offerObjectiveSnapshot(ticket, 0, records(MAX_RECORDS, 0)),
                        "nonpositive transport sequence rejected");
                passed("commit_and_independent_binding_gates");

                List<ConcurrentLinkedQueue<Long>> completions = new ArrayList<>();
                AtomicReference<Throwable> callbackFailure = new AtomicReference<>();
                for (RoomTls.Client guest : guests) {
                    ConcurrentLinkedQueue<Long> seen = new ConcurrentLinkedQueue<>();
                    completions.add(seen);
                    guest.setSafeEventSink(code -> {
                        if ("OBJECTIVE_GUEST_COMPLETED".equals(code)) {
                            try {
                                RoomObjectiveChunks.Snapshot value = guest.latestObjectiveSnapshot(ticket);
                                // Notification does not extend the value's lifetime;
                                // an independently concurrent ABORT may already revoke it.
                                if (value != null) seen.add(Long.valueOf(value.sequence));
                            } catch (Throwable problem) { callbackFailure.compareAndSet(null, problem); }
                        }
                    });
                }

                // A WORLD value offered at 100 Hz must not starve the multipart
                // objective stream. Both use real per-peer TLS writer loops.
                List<RoomWorldSnapshotWire.Avatar> avatars = new ArrayList<>();
                for (int slot = 1; slot <= 4; slot++)
                    avatars.add(new RoomWorldSnapshotWire.Avatar(slot, slot * 100, 20, -slot * 80, 0, 0));
                AtomicInteger worldOffers = new AtomicInteger();
                AtomicInteger worldTick = new AtomicInteger();
                ScheduledExecutorService worldPump = Executors.newSingleThreadScheduledExecutor();
                try {
                    worldPump.scheduleAtFixedRate(() -> {
                        try {
                            if (host.offerWorldSnapshot(worldTick.incrementAndGet(), avatars))
                                worldOffers.incrementAndGet();
                        } catch (Throwable problem) { callbackFailure.compareAndSet(null, problem); }
                    }, 0, 10, TimeUnit.MILLISECONDS);
                    byte[] offered = records(MAX_RECORDS, 0);
                    require(host.offerObjectiveSnapshot(ticket, 1, offered), "initial full offer");
                    Arrays.fill(offered, (byte) 0); // producer no longer owns the queued bytes
                    until("4096 records despite continuously offered WORLD", () -> {
                        callbackSucceeded(callbackFailure);
                        return allAt(guests, ticket, 1);
                    });
                    require(worldOffers.get() > 50, "continuous WORLD load actually offered");
                    for (RoomTls.Client guest : guests) {
                        verify(guest.latestObjectiveSnapshot(ticket), ticket, 1, MAX_RECORDS, 0);
                        RoomWorldSnapshotWire.Snapshot world = guest.worldSnapshot();
                        require(world != null && world.avatars.size() == 4 && world.matches(ticket),
                                "WORLD also progresses during objective transfer");
                    }
                } finally {
                    worldPump.shutdownNow();
                    require(worldPump.awaitTermination(2, TimeUnit.SECONDS), "world fixture stopped");
                }
                callbackSucceeded(callbackFailure);
                passed("4096_records_three_guests_continuous_world_fairness");

                AtomicBoolean superseded = new AtomicBoolean();
                host.setSafeEventSink(code -> {
                    if (FIRST_SENT.equals(code) && superseded.compareAndSet(false, true)) {
                        try {
                            require(host.offerObjectiveSnapshot(ticket, 3, records(MAX_RECORDS, 3)),
                                    "newer snapshot supersedes a physically started transfer");
                        } catch (Throwable problem) { callbackFailure.compareAndSet(null, problem); }
                    }
                });
                require(host.offerObjectiveSnapshot(ticket, 2, records(MAX_RECORDS, 2)), "older pending offer");
                until("new snapshot after first old part was sent", () -> {
                    callbackSucceeded(callbackFailure);
                    return superseded.get() && allAt(guests, ticket, 3);
                });
                host.setSafeEventSink(null);
                require(!host.offerObjectiveSnapshot(ticket, 2, records(MAX_RECORDS, 2)), "old offer cannot replay");
                for (int i = 0; i < guests.size(); i++) {
                    verify(guests.get(i).latestObjectiveSnapshot(ticket), ticket, 3, MAX_RECORDS, 3);
                    require(!completions.get(i).contains(Long.valueOf(2)), "partial superseded value never published");
                    require(guests.get(i).isRunning(), "heartbeat survives multiple large snapshots");
                }
                callbackSucceeded(callbackFailure);
                passed("newer_snapshot_supersedes_started_transfer_atomically");

                AtomicBoolean aborted = new AtomicBoolean();
                host.setSafeEventSink(code -> {
                    if (FIRST_SENT.equals(code) && aborted.compareAndSet(false, true)) {
                        try { require(host.cancelStory(), "cancel while objective transfer is incomplete"); }
                        catch (Throwable problem) { callbackFailure.compareAndSet(null, problem); }
                    }
                });
                require(host.offerObjectiveSnapshot(ticket, 4, records(MAX_RECORDS, 4)), "abort fixture offer");
                until("ABORT invalidates all guests", () -> {
                    callbackSucceeded(callbackFailure);
                    if (!aborted.get() || host.storyTicket() != null) return false;
                    for (RoomTls.Client guest : guests) if (guest.storyTicket() != null) return false;
                    return true;
                });
                host.setSafeEventSink(null);
                for (int i = 0; i < guests.size(); i++) {
                    require(guests.get(i).latestObjectiveSnapshot(ticket) == null,
                            "ABORT revokes old completed and pending values");
                    require(!completions.get(i).contains(Long.valueOf(4)), "aborted partial value never published");
                    require(guests.get(i).isRunning(), "ABORT preserves room connection");
                    guests.get(i).setSafeEventSink(null);
                }
                require(!host.offerObjectiveSnapshot(ticket, 5, records(MAX_RECORDS, 5)), "cancelled ticket rejected");
                passed("abort_after_first_part_revokes_pending_and_complete");

                RoomStoryWire.Ticket next = host.prepareStory(1, 3, 12);
                require(!next.same(ticket), "new mission has independent ticket");
                prepared(guests, next);
                bind(guests, next, MAX_RECORDS);
                commit(host, guests, next);
                require(!host.offerObjectiveSnapshot(ticket, 99, records(MAX_RECORDS, 0)), "old full ticket refused");
                require(host.offerObjectiveSnapshot(next, 1, records(MAX_RECORDS, 1)), "new ticket resets sequence to one");
                until("new-ticket sequence one", () -> allAt(guests, next, 1));
                for (RoomTls.Client guest : guests) {
                    verify(guest.latestObjectiveSnapshot(next), next, 1, MAX_RECORDS, 1);
                    require(guest.latestObjectiveSnapshot(ticket) == null, "old-ticket getter remains empty");
                }
                passed("new_ticket_sequence_reset_without_old_scope_reuse");

                AtomicBoolean departed = new AtomicBoolean();
                host.setSafeEventSink(code -> {
                    if (FIRST_SENT.equals(code) && departed.compareAndSet(false, true)) guests.get(2).close();
                });
                require(host.offerObjectiveSnapshot(next, 2, records(MAX_RECORDS, 2)), "roster invalidation fixture");
                until("departure invalidates started objective stream", () ->
                        departed.get() && host.snapshot().players.size() == 3 && host.storyTicket() == null &&
                        guests.get(0).storyTicket() == null && guests.get(1).storyTicket() == null);
                host.setSafeEventSink(null);
                for (RoomTls.Client guest : guests)
                    require(guest.latestObjectiveSnapshot(next) == null, "roster change revokes all objective state");
                require(!host.offerObjectiveSnapshot(next, 3, records(MAX_RECORDS, 3)), "departed roster cannot publish");
                passed("roster_departure_during_transfer_revokes_all_values");

                List<RoomTls.Client> remaining = guests.subList(0, 2);
                RoomStoryWire.Ticket emptyTicket = host.prepareStory(2, 0, 13);
                prepared(remaining, emptyTicket);
                bind(remaining, emptyTicket, 0);
                commit(host, remaining, emptyTicket);
                require(host.offerObjectiveSnapshot(emptyTicket, 1, new byte[0]), "empty objective table offer");
                until("empty objective table complete", () -> allAt(remaining, emptyTicket, 1));
                for (RoomTls.Client guest : remaining)
                    verify(guest.latestObjectiveSnapshot(emptyTicket), emptyTicket, 1, 0, 0);
                remaining.get(0).close();
                require(remaining.get(0).latestObjectiveSnapshot(emptyTicket) == null,
                        "close immediately revokes a completed value");
                passed("empty_table_and_immediate_close_revocation");
            } finally {
                for (RoomTls.Client guest : guests) guest.close();
            }
        }
    }

    private static void independentIds(SSLContext hostContext, SSLContext guestContext,
                                       InetAddress loop) throws Exception {
        try (RoomTls.Server host = RoomTls.Server.start(hostContext, loop, 0, "Host", "Independent IDs");
             RoomTls.Client guest = RoomTls.Client.connect(guestContext, loop, host.port(), "Guest")) {
            RoomStoryWire.Ticket ticket = host.prepareStory(0, 0, 21);
            List<RoomTls.Client> guests = Arrays.asList(guest);
            prepared(guests, ticket);
            byte[][] wrongIds = expectedIds(1);
            wrongIds[0][7] = (byte) 0x81;
            require(guest.bindObjectiveIds(ticket, wrongIds), "independent mismatching ID bound");
            AtomicInteger rejected = new AtomicInteger();
            guest.setSafeEventSink(code -> {
                if ("OBJECTIVE_GUEST_REJECTED".equals(code)) rejected.incrementAndGet();
            });
            commit(host, guests, ticket);
            require(host.offerObjectiveSnapshot(ticket, 1, records(1, 0)), "mismatch fixture sent");
            until("independent ID mismatch observed on actual TLS", () -> rejected.get() == 1);
            require(guest.latestObjectiveSnapshot(ticket) == null, "wire ID cannot replace independent ID");
            require(guest.isRunning(), "bounded data rejection preserves authenticated room");
            require(!guest.bindObjectiveIds(ticket, expectedIds(1)), "mismatch cannot rebind same-ticket history");
            passed("independent_id_mismatch_has_no_publication");
        }
    }

    private static void missingBinding(SSLContext hostContext, SSLContext guestContext,
                                       InetAddress loop) throws Exception {
        try (RoomTls.Server host = RoomTls.Server.start(hostContext, loop, 0, "Host", "No binding");
             RoomTls.Client guest = RoomTls.Client.connect(guestContext, loop, host.port(), "Guest")) {
            RoomStoryWire.Ticket ticket = host.prepareStory(0, 0, 22);
            List<RoomTls.Client> guests = Arrays.asList(guest);
            prepared(guests, ticket);
            // Legacy story traffic is still allowed with no objective consumer;
            // once READY has been sent a new table cannot be bound to this ticket.
            commit(host, guests, ticket);
            require(!guest.bindObjectiveIds(ticket, expectedIds(1)), "late binding after READY rejected");
            require(host.offerObjectiveSnapshot(ticket, 1, records(1, 0)), "unbound receive fixture");
            until("unbound objective receive closes", () -> !guest.isRunning());
            require(guest.latestObjectiveSnapshot(ticket) == null, "no implicit IDs from host bytes");
            until("unbound peer departure revokes story", () -> host.storyTicket() == null);
            passed("unbound_receive_fails_without_deriving_ids_from_wire");
        }
    }

    /** Ordinary protocol client with the same real certificate pin as Client. */
    private static final class RawGuest implements AutoCloseable {
        final SSLSocket socket;
        final DataInputStream in;
        final DataOutputStream out;

        RawGuest(SSLContext context, InetAddress loop, int port) throws Exception {
            socket = (SSLSocket) context.getSocketFactory().createSocket(loop, port);
            socket.setSoTimeout(5000);
            socket.setTcpNoDelay(true);
            socket.startHandshake();
            in = new DataInputStream(socket.getInputStream());
            out = new DataOutputStream(socket.getOutputStream());
            send(RoomWire.join(new RoomWire.Join("Protocol fixture", new byte[16])));
            require(read().type == RoomWire.ACCEPT, "raw fixture accepted with pinned TLS");
        }

        RoomWire.Packet read() throws IOException { return RoomWire.decode(RoomWire.read(in)); }

        void send(byte[] frame) throws IOException {
            synchronized (out) { RoomWire.write(out, frame); }
        }

        RoomWire.Packet untilType(int type) throws Exception {
            long deadline = System.nanoTime() + TimeUnit.SECONDS.toNanos(8);
            while (System.nanoTime() < deadline) {
                RoomWire.Packet packet = read();
                if (packet.type == type) return packet;
                require(packet.type == RoomWire.ROSTER, "only roster precedes expected control frame");
            }
            throw new AssertionError("raw TLS control deadline");
        }

        @Override public void close() throws IOException { socket.close(); }
    }

    private static byte[] objectiveBody(RoomWire.Packet packet) throws IOException {
        DataInputStream in = RoomWire.body(packet, RoomWire.OBJECTIVE_CHUNK);
        byte[] bytes = new byte[in.available()];
        in.readFully(bytes);
        return bytes;
    }

    private static void controlAndDirection(SSLContext hostContext, SSLContext guestContext,
                                            InetAddress loop) throws Exception {
        try (RoomTls.Server host = RoomTls.Server.start(hostContext, loop, 0, "Host", "Control priority");
             RawGuest guest = new RawGuest(guestContext, loop, host.port())) {
            RoomStoryWire.Ticket ticket = host.prepareStory(0, 0, 31);
            require(RoomStoryWire.parse(guest.untilType(RoomWire.STORY_PREPARE)).same(ticket), "raw PREPARE ticket");
            require(!host.hostSceneLoaded(0, 0, 31), "host waits for raw READY");
            guest.send(RoomStoryWire.encode(RoomWire.STORY_READY, ticket));
            require(RoomStoryWire.parse(guest.untilType(RoomWire.STORY_COMMIT)).same(ticket), "raw COMMIT ticket");
            require(host.offerObjectiveSnapshot(ticket, 1, records(MAX_RECORDS, 7)), "raw full snapshot offer");
            RoomObjectiveChunks.Assembler assembler = new RoomObjectiveChunks.Assembler(ticket, expectedIds(MAX_RECORDS));
            int parts = 0;
            int pongAfterParts = -1;
            long lastPong = 0;
            byte[] first = null;
            AtomicLong sentPulse = new AtomicLong();
            AtomicReference<Throwable> heartbeatFailure = new AtomicReference<>();
            ScheduledExecutorService heartbeat = Executors.newSingleThreadScheduledExecutor();
            long deadline = System.nanoTime() + TimeUnit.SECONDS.toNanos(12);
            try {
                while (parts < MAX_PARTS && System.nanoTime() < deadline) {
                    callbackSucceeded(heartbeatFailure);
                    RoomWire.Packet packet = guest.read();
                    if (packet.type == RoomWire.ROSTER) continue;
                    if (packet.type == RoomWire.PONG) {
                        long received = RoomWire.parsePulse(packet);
                        require(received > lastPong && received <= sentPulse.get(), "matching increasing PONG response");
                        if (pongAfterParts == -1) {
                            require(received == 1, "first PONG answers the first heartbeat");
                            pongAfterParts = parts;
                        }
                        lastPong = received;
                        continue;
                    }
                    require(packet.type == RoomWire.OBJECTIVE_CHUNK, "expected bounded objective frame");
                    byte[] body = objectiveBody(packet);
                    require(body.length + 8 <= RoomWire.MAX_FRAME, "actual TLS application frame is bounded");
                    RoomObjectiveChunks.Result result = assembler.accept(ticket, body);
                    parts++;
                    if (parts == 1) {
                        first = body;
                        require(result == RoomObjectiveChunks.Result.PENDING, "first physical part is incomplete");
                        // Use the ordinary one-second room heartbeat for the
                        // whole bounded transfer, including a slower CI host.
                        // The first response must still precede the final part.
                        sentPulse.set(1);
                        guest.send(RoomWire.pulse(RoomWire.PING, 1));
                        heartbeat.scheduleAtFixedRate(() -> {
                            try { guest.send(RoomWire.pulse(RoomWire.PING, sentPulse.incrementAndGet())); }
                            catch (Throwable problem) { heartbeatFailure.compareAndSet(null, problem); }
                        }, 1, 1, TimeUnit.SECONDS);
                    }
                    require(result == (parts == MAX_PARTS ? RoomObjectiveChunks.Result.COMPLETE : RoomObjectiveChunks.Result.PENDING),
                            "complete-only publication across actual frames");
                }
                callbackSucceeded(heartbeatFailure);
                require(parts == MAX_PARTS && pongAfterParts >= 1 && pongAfterParts < MAX_PARTS,
                        "PONG processed while objective stream still in progress");
                verify(assembler.latest(ticket), ticket, 1, MAX_RECORDS, 7);
                passed("heartbeat_pong_precedes_last_of_158_parts");

                // Direction is an ordinary protocol contract: a joined guest is
                // never an authoritative objective producer, even for its own ticket.
                // Heartbeats continue here: expiry must not make a host which
                // incorrectly ignores the frame look like a successful rejection.
                require(first != null, "captured ordinary objective frame");
                guest.send(RoomWire.frame(RoomWire.OBJECTIVE_CHUNK, first));
                boolean closed = false;
                long closeDeadline = System.nanoTime() + TimeUnit.SECONDS.toNanos(8);
                while (!closed && System.nanoTime() < closeDeadline) {
                    try { guest.read(); }
                    catch (SocketTimeoutException timeout) { throw new AssertionError("wrong-direction frame was not closed", timeout); }
                    catch (EOFException | SocketException | SSLException expectedClose) { closed = true; }
                }
                require(closed, "guest objective frame rejected by host");
                until("wrong-direction sender removed", () -> host.snapshot().players.size() == 1 && host.storyTicket() == null);
                passed("guest_cannot_publish_host_objectives");
            } finally {
                heartbeat.shutdownNow();
                require(heartbeat.awaitTermination(2, TimeUnit.SECONDS), "raw fixture heartbeat stopped");
            }
        }
    }

    public static void main(String[] args) throws Exception {
        require(args.length == 1, "PKCS12 fixture path required");
        Path identity = Paths.get(args[0]);
        SSLContext hostContext = TlsRoomTest.host(identity);
        SSLContext guestContext = RoomTls.pinnedContext(TlsRoomTest.fingerprint(identity));
        InetAddress loop = InetAddress.getByName("127.0.0.1");
        fourPeerLifecycle(hostContext, guestContext, loop);
        independentIds(hostContext, guestContext, loop);
        missingBinding(hostContext, guestContext, loop);
        controlAndDirection(hostContext, guestContext, loop);
        require(scenarios == 11, "all planned lifecycle cases executed");
        until("room threads closed", () -> Thread.getAllStackTraces().keySet().stream()
                .noneMatch(t -> t.isAlive() && t.getName().startsWith("bo-room-")));
        System.out.println("OBJECTIVE_REAL_TLS_PASS scenarios=" + scenarios +
                " guests=3 records=4096 parts=158 PINNED_LOOPBACK_TLS SYNTHETIC_VALUES_NO_ANDROID_ENGINE");
    }
}
