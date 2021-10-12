import socket

# Message IDS
REGISTER_ID = '0'
REQUEST_ID = '1'
GRANT_ID = '2'
RELEASE_ID = '3'
DENIED_ID = '4'
QUIT_ID = '5'
UNREGISTER_ID = '6'
# Message Utils
MSG_SIZE = 7
MSG_SEP = '|'
INIT_MSG = '0|0|00\n'
# Sockets
COORD_IP = "127.0.0.1"
COORD_PORT = 8080

MSG_IDS = [
    'registred',
    'request',
    'grant',
    'release',
    'denied',
    'quit',
    'unregister'
]

MSG_IDX = 0
PROCESS_ID_IDX = 1

class Lock(object):

    def __init__(self, addr, port):
        self.addr = addr
        self.port = port
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.s.connect((self.addr, self.port))
        #print('Conectado e registrando')
        self._register()

    def _register(self):
        self.s.send(INIT_MSG.encode())
        #print('Mensagem de registro enviada')
        res = self._listen()
        if res['response'] == 'denied':
            raise RuntimeError('Conexão não autorizada')
        elif res['response'] == 'registred':
            self.id = res['id']
        else:
            raise RuntimeError(f'Mensagem não esperada {res}')

    def _listen(self):
        msg_str = self.s.recv(MSG_SIZE).decode().split(MSG_SEP)
        msg_idx=int(msg_str[MSG_IDX])
        res = {
            'response': MSG_IDS[msg_idx],
            'id': msg_str[PROCESS_ID_IDX]
        }
        return res

    def _request(self):
        msg = REQUEST_ID + MSG_SEP + self.id + MSG_SEP
        msg += (MSG_SIZE - len(msg)) * '0' + '\n'
        self.s.send(msg.encode())

    def _wait_grant(self):
        res = self._listen()
        if res['response'] == 'grant':
            return 0
        elif res['response'] == 'quit':
            raise SystemExit(f'Coordenador mandou finalizar {res}')
        else:
            raise RuntimeError(f'Recebida mensagem invalida {res}')
    
    def acquire(self):
        self._request()
        self._wait_grant()

    def release(self):
        msg = RELEASE_ID + MSG_SEP + self.id + MSG_SEP
        msg += (MSG_SIZE - len(msg)) * '0' + '\n'
        self.s.send(msg.encode())
    
    def unregister(self):
        msg = UNREGISTER_ID + MSG_SEP + self.id + MSG_SEP
        msg += (MSG_SIZE - len(msg)) * '0' + '\n'
        self.s.send(msg.encode())