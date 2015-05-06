.PHONY: all
all: METRO

METRO: estruturas.o
	gcc -g -std=gnu99 main.c estruturas.o -o metro -pthread

estruturas.o: ./estruturas.c
	gcc -g -std=gnu99 -Wall estruturas.c estruturas.h -c

clean:
	rm -f *.o
	rm -f metro
