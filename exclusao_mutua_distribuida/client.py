import time
from datetime import datetime
import argparse
import socket

MSG_SEP = '|'
MSG_SIZE = 6
INIT_MSG = MSG_SIZE*'0'
REQUEST_ID = '1'
GRANT_ID = '2'
RELEASE_ID = '3'
UNAUTHORIZED_ID = '4'
COORD_IP = "127.0.0.1"
COORD_PORT = 8080

RESPONSE_IDS = [
    'registred',
    'request',
    'grant',
    'release',
    'denied'
]

RESPONSE_IDX = 0
ID_IDX = 1

class Lock(object):

    def __init__(self, addr, port):
        self.addr = addr
        self.port = port
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.s.connect((self.addr, self.port))
        self.s.send(INIT_MSG.encode())
        res = self._listen()
        if res['response'] == 'denied':
            raise RuntimeError('Conexão não autorizada')
        elif res['response'] == 'resgistred':
            self.id = res['id']
    
    def _listen(self):
        msg_str = self.s.recv(MSG_SIZE).decode().split()
        res = {
            'response': RESPONSE_IDS[msg_str[RESPONSE_IDX]],
            'id': RESPONSE_IDS[msg_str[ID_IDX]] 
        }
        return res

    def _request(self):
        msg = REQUEST_ID + MSG_SEP + self.id + MSG_SEP
        msg += (MSG_SIZE - len(msg)) * '0'
        self.s.send(msg.encode())

    def _wait_grant(self):
        res = self._listen()
        if res['response'] == 'grant':
            return 0
        else:
            raise RuntimeError(f'Recebida messagem invalida {res}')
    
    def acquire(self):
        self._request()
        self._wait_grant()

    def release(self):
        msg = RELEASE_ID + MSG_SEP + self.id + MSG_SEP
        msg += (MSG_SIZE - len(msg)) * '0'
        self.s.send(msg.encode())


parser = argparse.ArgumentParser()
parser.add_argument('r', type=int, help='Número de acessos')
parser.add_argument('k', type=int, help='Segundos de espera')
args = parser.parse_args()


lock = Lock(COORD_IP, COORD_PORT)

for i in range(args['r']):

    lock.acquire()
    with open('resultado.txt', 'a') as result_file:
        result_file.write(f'{datetime.now()};{lock.id}\n')
    time.sleep(args['k'])
    lock.release()