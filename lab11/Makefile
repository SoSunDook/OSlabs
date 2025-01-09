.PHONY: all rwlock clean

all: rwlock

rwlock: main.c
	gcc -lpthread main.c -o rwlock

clean:
	rm -rf *.o rwlock
