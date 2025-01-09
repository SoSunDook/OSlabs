.PHONY: all customcat clean

all: customcat customgrep

customcat: customcat.c
	gcc customcat.c -o customcat

customgrep: customgrep.c
	gcc customgrep.c -o customgrep

clean:
	rm -rf *.o customcat customgrep
