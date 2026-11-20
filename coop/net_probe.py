"""Isolated UDP lab. No BLACK integration, security or production netcode."""
import argparse
import json
import socket
import threading
import time

PORT = 24872

def send(sock, addr, msg):
    sock.sendto(json.dumps(msg, separators=(",", ":")).encode(), addr)

def host(sock, stop):
    players = {}
    sock.settimeout(0.1)
    while not stop.is_set():
        try:
            data, addr = sock.recvfrom(2048)
        except socket.timeout:
            continue
        try:
            msg = json.loads(data)
            if msg.get("v") != 1:
                continue
            kind = msg["type"]
            if kind == "join":
                if addr not in players:
                    if len(players) >= 4:
                        continue
                    players[addr] = {"id": len(players)+1, "x": 0, "seq": 0}
                send(sock, addr, {"type": "joined", "id": players[addr]["id"]})
            elif kind == "input" and addr in players:
                p = players[addr]
                seq, dx = msg["seq"], msg["dx"]
                if type(seq) is not int or type(dx) is not int:
                    continue
                if seq > p["seq"] and dx in (-1, 0, 1):
                    p["x"] += dx
                    p["seq"] = seq
                state = {"type": "snapshot", "players": list(players.values())}
                for peer in players:
                    send(sock, peer, state)
        except (ValueError, KeyError, TypeError, AttributeError):
            continue

def receive(sock, kind, predicate=lambda m: True):
    limit = time.monotonic() + 3
    while time.monotonic() < limit:
        sock.settimeout(max(0.01, limit-time.monotonic()))
        msg = json.loads(sock.recv(2048))
        if msg.get("type") == kind and predicate(msg):
            return msg
    raise TimeoutError(kind)

def join(sock):
    sock.send(json.dumps({"v": 1, "type": "join"}).encode())
    return receive(sock, "joined")["id"]

def movement(sock, seq, dx):
    sock.send(json.dumps({"v": 1, "type": "input", "seq": seq, "dx": dx}).encode())

def selftest():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server, \
         socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as a, \
         socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as b:
        server.bind(("127.0.0.1", 0))
        stop = threading.Event()
        thread = threading.Thread(target=host, args=(server, stop))
        thread.start()
        try:
            a.connect(server.getsockname())
            b.connect(server.getsockname())
            assert (join(a), join(b)) == (1, 2)
            movement(a, 1, 1)
            receive(a, "snapshot")
            receive(b, "snapshot")
            movement(b, 1, -1)
            expected = [(1, 1, 1), (2, -1, 1)]
            def matches(m):
                return [(p["id"], p["x"], p["seq"]) for p in m["players"]] == expected
            assert matches(receive(a, "snapshot", matches))
            assert matches(receive(b, "snapshot", matches))
            movement(b, 1, -1)  # duplicate sequence must not move twice
            assert matches(receive(a, "snapshot"))
            assert matches(receive(b, "snapshot"))
            print("PASS: two clients, distinct IDs, shared authoritative state, duplicate input ignored")
        finally:
            stop.set()
            thread.join(timeout=2)

def main():
    p = argparse.ArgumentParser()
    p.add_argument("mode", choices=["host", "client", "selftest"])
    p.add_argument("--ip", default="127.0.0.1")
    p.add_argument("--port", type=int, default=PORT)
    a = p.parse_args()
    if a.mode == "selftest":
        return selftest()
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        if a.mode == "host":
            s.bind((a.ip, a.port))
            print("LAB ONLY: listening on", s.getsockname(), flush=True)
            host(s, threading.Event())
        else:
            s.connect((a.ip, a.port))
            print("joined", join(s))
            for seq in range(1, 6):
                movement(s, seq, 1)
                print(receive(s, "snapshot"))
                time.sleep(1)

if __name__ == "__main__":
    main()
