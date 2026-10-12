#!/usr/bin/env python3
"""Scenario de test mini_db : python3 test_client.py <binaire> <port> <dossier_tmp>
Affiche une transcription deterministe (a comparer entre ta version et la reference)."""
import os, signal, socket, subprocess, sys, time

binary, port, tmp = sys.argv[1], int(sys.argv[2]), sys.argv[3]
save = os.path.join(tmp, ".save")
if os.path.exists(save):
    os.remove(save)

def start():
    p = subprocess.Popen([binary, str(port), save], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    print("ready ->", repr(p.stdout.readline().decode()))
    return p

def ask(s, cmds):
    s.sendall(cmds)
    time.sleep(0.15)
    return s.recv(65536).decode()

p = start()
s = socket.create_connection(("127.0.0.1", port)); s2 = socket.create_connection(("127.0.0.1", port))
print(repr(ask(s, b"POST A B\nPOST B C\nGET A\nGET C\n")))
print(repr(ask(s, b"DELETE A\nDELETE C\nUNKNOWN_COMMAND\nGET\nPOST A\n")))
s.sendall(b"POST lo"); time.sleep(0.1)             # commande coupee en deux
print(repr(ask(s2, b"GET B\n")))                   # un autre client entre-temps
print(repr(ask(s, b"ng " + b"v" * 900 + b"\nGET long\n")[:20]))
s.close(); s2.close()
p.send_signal(signal.SIGINT); p.wait()
print("SIGINT -> exit", p.returncode)
p = start()
s = socket.create_connection(("127.0.0.1", port))
print("apres redemarrage :", repr(ask(s, b"GET B\nGET A\n")))
s.close()
p.send_signal(signal.SIGINT); p.wait()
