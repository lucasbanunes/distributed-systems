#!/usr/bin/env python3
# Controlador

import socket
import random
import argparse
from services import Client

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 8080       # Port to listen on (non-privileged ports are > 1023)

parser = argparse.ArgumentParser()
parser.add_argument('-n_numbers', type=int, default=10)
args = parser.parse_args()

try:
    c = Client(HOST, PORT)
    c.start()

    n = 1
    c.send(n)
    res = c.get_response()
    print(res)
    for _ in range(args.n_numbers-1):
        n += random.randint(1, 100)
        c.send(n)
        res = c.get_response()
        print(res)
    c.turn_off()
finally:
    c.close()