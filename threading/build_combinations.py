from itertools import product

ADDER_NUMBERS = [int(1e7), int(1e8), int(1e9)]
ADDER_THREADS = [1,2,4,6,8,16,32,64,128,256]
N_ITER = 10

PRODUCER_CONSUMER = {(1, 1),(1, 2),(1, 4),(1, 8),(1, 16),(2, 1),(4, 1),(8, 1),(16, 1)}
MEMORY_SIZES = [1,2,4,16,32]

# Write build_csv_adder.sh
with open('build_csv_adder.sh', 'w') as script:
    script.write("echo 'no_thread;thread;n_threads;n_numbers' > threaded-adder-time.csv\n")
    script.write("echo '[-] Generating CSV Files'\n")
    for n_threads, n_numbers in product(ADDER_THREADS, ADDER_NUMBERS):
        for _ in range(N_ITER):
            script.write(f"echo $(./threaded_adder {n_numbers} {n_threads}) >> threaded-adder-time.csv\n")
    script.write("echo '[X] Generated'\n")

# Write build_csv_pc.sh
with open('build_csv_pc.sh', 'w') as script:
    script.write("echo 'memory_size;n_producers;n_consumers;time_elapsed' > pc-time.csv\n")
    script.write("echo '[-] Generating CSV Files'\n")
    for memory_size, prod_consu in product(MEMORY_SIZES, PRODUCER_CONSUMER):
        n_prod, n_consu = prod_consu
        for _ in range(N_ITER):
            script.write(f"echo \"$(./producer-consumer {memory_size} {n_prod} {n_consu} | grep final " + "| awk '{print $NF}')\" >> pc-time.csv\n")
    script.write("echo '[X] Generated'\n")