#!/bin/sh

# create release version of cle.
# -O == optimise

gcc -pthread -m32 -O -c -I../Ccl4src -I./src clc.c
gcc -pthread -m32 -o clc clc.o -lncurses -lm

# remove all debug and symbol info
strip -s clc

# report file size
ls -al clc
