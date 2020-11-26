CC=gcc
MPICC=mpicc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3

default: all


msort:
	$(CILKCC) $(CFLAGS) -c -fpic msort.c -fcilkplus
	$(CILKCC) $(CFLAGS) -shared -o libmsort.so msort.o

mmio:
	$(CC) $(CFLAGS) -c -fpic mmio.c
	$(CC) $(CFLAGS) -shared -o libmmio.so mmio.o

mmarket:
	$(CILKCC) $(CFLAGS) -c -fpic mmarket.c mat.o mmio.o msort.o -fcilkplus
	$(CILKCC) $(CFLAGS) -shared -o libmmarket.so mmarket.o mat.o mmio.o msort.o

mat:
	$(CC) $(CFLAGS) -c -fpic mat.c msort.o -fcilkplus
	$(CC) $(CFLAGS) -shared -o libmat.so mat.o msort.o

triangles: 
	$(CILKCC) $(CFLAGS) -o triangles.o triangles.c mat.o mmarket.o mmio.o msort.o -fcilkplus

all: msort mmio mat mmarket triangles

.PHONY: all test clean

test:
	./triangles.o $(ARGS)

clean:
	rm -f *.so
	rm -f *.o
	rm -f *.out