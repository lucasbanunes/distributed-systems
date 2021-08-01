#!/usr/bin/env python3
import socket
from services import Server
HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 8080       # Port to listen on (non-privileged ports are > 1023)

try:
    s = Server(HOST, PORT)
    s.start()
finally:
    s.close()