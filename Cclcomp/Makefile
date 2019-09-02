clc : clc.o
	gcc -pthread -m32 clc.o -lncurses -lm -o clc
	ls -al clc
clc.o : clc.c src/* ../Ccl4src/*
	gcc -pthread -m32 -g -c -I../Ccl4src -I./src clc.c
clean :
	rm clc *.o
