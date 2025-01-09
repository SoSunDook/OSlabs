.PHONY: all customls run clean

all: run clean

customls: main.c
	gcc main.c -o customls

run: customls
	./customls

clean:
	rm -f customls
