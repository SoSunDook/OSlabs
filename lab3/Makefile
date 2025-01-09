.PHONY: all fork run clean

all: run

fork: main.c
	gcc main.c -o fork

run: fork
	./fork

clean:
	rm -f fork