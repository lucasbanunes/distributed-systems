from multiprocessing import Process
import time
from datetime import datetime
import argparse
from mutual_exclusion import Lock, COORD_IP, COORD_PORT

def process_work(r, k):
    lock = Lock(COORD_IP, COORD_PORT)
    try:
        for i in range(r):

                #print(f'{lock.id};{i}; Aguardando')
                lock.acquire()
                with open('resultado.txt', 'a') as result_file:
                    result_file.write(f'{datetime.now()};{lock.id}\n')
                time.sleep(k)
                lock.release()
                #print(f'{lock.id};{i}; Liberado')
    except SystemExit:
        print("O servidor mandou finalizar todos os processos")
    finally:
        #print(f"{lock.id} Descadastrado")
        lock.unregister()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('n', type=int, help='Número de processos')
    parser.add_argument('r', type=int, help='Número de acessos')
    parser.add_argument('k', type=int, help='Segundos de espera')
    args = parser.parse_args().__dict__

    print('Inicializando')

    pool = [Process(target=process_work, args=(args['r'], args['k'])) for _ in range(args['n'])]
    [p.start() for p in pool]
    [p.join() for p in pool]
    print('Finalizado')