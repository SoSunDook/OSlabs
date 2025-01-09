.PHONY: all customchmod run clean

all: run

customchmod: main.c
	gcc main.c -o customchmod

run: customchmod
	./customchmod g+rw file.txt

clean:
	rm -f  customchmod
