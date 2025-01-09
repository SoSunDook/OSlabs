.PHONY: all pipe fifo run clean

all: run

pipe: pipe.c
	gcc pipe.c -o pipe

fifo: fifo.c
	gcc fifo.c -o fifo

run: pipe fifo
	./pipe && ./fifo

clean:
	rm -f pipe fifo
