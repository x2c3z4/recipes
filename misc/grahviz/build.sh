#!/bin/bash
set -o nounset          # Treat unset variables as an error

#gcc -Wl,-Map=test.map -g -o test test.c
#dot -Tjpg test.dot -o test.jpg

gcc -g -finstrument-functions test.c instrument.c -o test
./test
pvtrace test
dot -Tjpg graph.dot -o graph.jpg

