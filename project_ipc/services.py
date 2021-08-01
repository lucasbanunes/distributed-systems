import socket

class Comunicator(object):

    def __init__(self, addr, port):
        self.addr = addr
        self.port = port
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    def build_msg(self, method: str, value: int, src: str) -> bytes:
        msg = f"{method} {value} {src}\n".upper()
        return msg
    
    def parse_msg(self, msg: bytes):
        msg = msg.decode()
        method, value, src = msg.split(' ')
        src = src[:-1]  #Removes line break
        value = int(value)
        return method, value, src
    
    def close(self):
        self.s.close()

class Server(Comunicator):

    def start(self):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.bind((self.addr, self.port))
        self.s.listen()
        print('Ouvindo')
        self.client, self.clientAddr = self.s.accept()
        while True:
            msg = self.client.recv(1024)
            method, value, src = self.parse_msg(msg)

            if method == 'NUMERO' and src == 'CLIENT':
                print(f'Checando número {value}')
                self.check_prime(value)
            elif method == 'FINALIZA' and src == 'CLIENT' and not value:
                print('Messagem de encerramento recebida')
                self.close()
                break
            else:
                raise RuntimeError('Messagem inválida recebida')
    
    def check_prime(self, num):
        value = self.is_prime(num)
        msg = self.build_msg(method='RESPOSTA', value=value, src='SERVER')
        self.client.send(msg.encode())
    
    def is_prime(self, num: int) -> int:
        if (num%2 == 0) or (num<=1):
            return 0
        
        for i in range(3, num, 2):
            if num%i == 0:
                return 0
        
        return 1

class Client(Comunicator):

    def start(self):
        self.s.connect((self.addr, self.port))

    def send(self, num: int) -> str:
        print(f'Enviando {num}.')
        msg = self.build_msg(method='NUMERO', value=num, src='CLIENT')
        self.s.send(msg.encode())
    
    def get_response(self):

        msg = self.s.recv(1024)
        method, value, src = self.parse_msg(msg)

        if (method == 'RESPOSTA') and (src == 'SERVER'):
            if value:
                res = 'O número fornecido é primo'
            else:
                res = 'O número fornecido não é primo'
        elif method == 'FINALIZA' and src == 'SERVER' and not value:
            print('Messagem de encerramento recebida')
            res = self.close()
        else:
            raise RuntimeError('Messagem inválida recebida')

        return res
    
    def turn_off(self):

        print('Desligando server')
        msg = self.build_msg('FINALIZA', 0, 'CLIENT')
        self.s.send(msg.encode())
