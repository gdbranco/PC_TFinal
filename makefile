all: METRO

METRO: estruturas.o
	gcc main.c estruturas.o -o metro -pthread

estruturas.o: ./estruturas.c
	gcc -std=gnu99 -Wall estruturas.c estruturas.h -c

clean:
	rm -f *.o
	rm -f metro
