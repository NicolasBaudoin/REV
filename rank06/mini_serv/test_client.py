#!/usr/bin/env python3
"""Scenario de test mini_serv : python3 test_client.py <binaire> <port>
Affiche une transcription deterministe (a comparer entre ta version et la reference)."""
import socket, subprocess, sys, time

binary, port = sys.argv[1], int(sys.argv[2])
srv = subprocess.Popen([binary, str(port)], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
time.sleep(0.3)

def connect():
    s = socket.create_connection(("127.0.0.1", port))
    s.settimeout(0.3)
    time.sleep(0.15)
    return s

def rd(s):
    out = b""
    try:
        while True:
            d = s.recv(65536)
            if not d:
                break
            out += d
    except socket.timeout:
        pass
    return out.decode(errors="replace")

try:
    a = connect(); b = connect(); c = connect()
    print("1. A recoit :", repr(rd(a)))
    print("2. B recoit :", repr(rd(b)))
    print("3. C recoit :", repr(rd(c)))
    a.sendall(b"hello\nworld\npart"); time.sleep(0.15)
    print("4. B recoit :", repr(rd(b)))
    print("5. A ne recoit rien :", repr(rd(a)))
    a.sendall(b"ial\n"); time.sleep(0.15)
    print("6. C recoit :", repr(rd(c)))
    b.close(); time.sleep(0.15)
    print("7. A recoit :", repr(rd(a)))
    d = connect()
    print("8. C recoit :", repr(rd(c)))
    big = (b"x" * 99 + b"\n") * 2000           # client "paresseux" : A ne lit pas tout de suite
    c.sendall(big); time.sleep(0.5)
    got = rd(a)
    print("9. A recoit le gros message :", len(got), "octets,", got.count("client 2: "), "prefixes")
    print("10. D recoit :", len(rd(d)), "octets")
finally:
    srv.terminate()
    srv.wait()
r = subprocess.run([binary], stderr=subprocess.PIPE)
print("11. sans argument :", repr(r.stderr.decode()), "exit", r.returncode)
