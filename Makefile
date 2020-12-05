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
	$(CILKCC) $(CFLAGS) -c -fpic mmarket.c -fcilkplus
	$(CILKCC) $(CFLAGS) -shared -o libmmarket.so mmarket.o

mat:
	$(CC) $(CFLAGS) -c -fpic mat.c -fcilkplus
	$(CC) $(CFLAGS) -shared -o libmat.so mat.o 

v3:
	$(CC) $(CFLAGS) -c -fpic v3.c
	$(CC) $(CFLAGS) -shared -o libv3.so v3.o

v3_clk:
	$(CILKCC) $(CFLAGS) -c -fpic v3_clk.c -fcilkplus  -lpthread
	$(CILKCC) $(CFLAGS) -shared -o libv3_clk.so v3_clk.o

v3_omp:
	$(CC) $(CFLAGS) -c -fpic v3_omp.c -fopenmp
	$(CC) $(CFLAGS) -shared -o libv3_omp.so v3_omp.o

v4:
	$(CC) $(CFLAGS) -c -fpic v4.c
	$(CC) $(CFLAGS) -shared -o libv4.so v4.o

v4_clk:
	$(CILKCC) $(CFLAGS) -c -fpic v4_clk.c -fcilkplus -lpthread
	$(CILKCC) $(CFLAGS) -shared -o libv4_clk.so v4_clk.o

v4_omp:
	$(CC) $(CFLAGS) -c -fpic v4_omp.c -fopenmp
	$(CC) $(CFLAGS) -shared -o libv4_omp.so v4_omp.o

v4_ptd:
	$(CC) $(CFLAGS) -c -fpic v4_ptd.c -lpthread
	$(CC) $(CFLAGS) -shared -o libv4_ptd.so v4_ptd.o

triangles: 
	$(CC) $(CFLAGS) -o triangles.o triangles.c mat.o \
	mmarket.o mmio.o msort.o v4.o v4_clk.o v4_omp.o v4_ptd.o \
	v3.o v3_clk.o v3_omp.o -fcilkplus -fopenmp

testfunc: 
	$(CC) $(CFLAGS) -o testfunc.o testfunc.c


all: msort mmio mat mmarket v3 v3_clk v3_omp v4 v4_clk v4_omp v4_ptd triangles

.PHONY: all test clean

test:
	./triangles.o $(ARGS)

clean:
	rm -f *.so
	rm -f *.o
	rm -f *.lib
	rm -f *.txt
