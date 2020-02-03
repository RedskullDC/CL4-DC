cle : cle.o
	gcc -pthread -m32 cle.o -Wl,-Bstatic -lavcall -Wl,-Bdynamic -lncurses -lm -lssl -lcrypto -ldl -o cle

	ls -al cle
cle.o : cle.c src/* ../Ccl4src/*
	gcc -std=gnu99 -pthread -m32 -g -c -I../Ccl4src -I./src cle.c
clean :
	rm cle *.o
