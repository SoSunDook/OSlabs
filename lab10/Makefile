.PHONY: all condvar clean

all: condvar

condvar: main.c
	gcc -lpthread main.c -o condvar

clean:
	rm -rf *.o condvar
