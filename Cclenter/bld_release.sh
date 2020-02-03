#!/bin/sh

# create release version of cle.
# -O == optimise

gcc -std=gnu99 -pthread -m32 -ggdb -c -I../Ccl4src -I./src cle.c
#gcc -std=gnu99 -pthread -m32 -O -c -I../Ccl4src -I./src cle.c
gcc -pthread -m32 cle.o -Wl,-Bstatic -lavcall -Wl,-Bdynamic -lncurses -lm -lssl -lcrypto -ldl -o cle

# remove all debug and symbol info
#strip -s cle

# report file size
ls -al cle
