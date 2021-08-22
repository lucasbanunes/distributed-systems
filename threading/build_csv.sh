#!/bin/bash

#Initializing csv files
echo 'no_thread;thread;n_threads;n_numbers' > threaded-adder-time.csv
#Generating CSV Files
echo '[-] Generating CSV Files'

echo $(./threaded_adder 151 2) >> threaded-adder-time.csv
echo $(./threaded_adder 179 2) >> threaded-adder-time.csv
echo $(./threaded_adder 144 2) >> threaded-adder-time.csv
echo $(./threaded_adder 105 2) >> threaded-adder-time.csv
echo $(./threaded_adder 173 2) >> threaded-adder-time.csv
echo $(./threaded_adder 126 2) >> threaded-adder-time.csv
echo $(./threaded_adder 130 2) >> threaded-adder-time.csv
echo $(./threaded_adder 174 2) >> threaded-adder-time.csv
echo $(./threaded_adder 156 2) >> threaded-adder-time.csv
echo $(./threaded_adder 122 2) >> threaded-adder-time.csv
echo $(./threaded_adder 115 2) >> threaded-adder-time.csv
echo $(./threaded_adder 148 2) >> threaded-adder-time.csv
echo $(./threaded_adder 198 2) >> threaded-adder-time.csv
echo $(./threaded_adder 133 2) >> threaded-adder-time.csv

echo '[X] Generated' 
#Generating Graph images
#echo $(python3 graph_generator.py)