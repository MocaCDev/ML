.PHONY: all
.PHONY: clean

all:
	gcc -g -O2 main.c -o main.o

clean:
	rm -rf *.o
