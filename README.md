## Parallel & Distributed Systems - Project 1

The main function can be found in triangles.c, while v3.c, 3vclk.c, v3omp.c, v4.c, v4clk.c, v4omp.c, v4ptd.c contain implementations of the two versions in serial and parallel forms.

The project can be Compiled and executed by using:
````
./run "<Path to mtx file> -t<threads> -c0 -v4omp -v3clk <any implementations, see below> "
````

The available arguments are:
````
Usage: ./triangles.o [mtx-filename] [arguments]

 -tX     Set Threads

 -cX     Set display type.
         X=0: Only Status info (Default).
         X=1: Status info and C vector.
         X=2: Only C vector.

 -vX     To run the serialized code.
         X=2,3,4: For V2, V3, V4 respectively.

 -vXclk  To run OpenCilk code.
         X=3,4: For V3, V4 respectively.

 -vXomp  To run OpenMP code.
         X=3,4: For V3, V4 respectively.

 -v4ptd  To run the V4 POSSIX Threads code.

 -coorow Print 1-Based COO rows horizontally.
 -coocol Print 1-Based COO columns horiizonntally.
 -coo    Print 1-Based COO rows and columns vertically.
````


## Testing

Use `-c2` as an argument, for the C vector to be printed on its own. For comparing the two outputs, use the `compare` script.

Example: 
````
./run "<Path to mtx file> -t<threads> -c2 -v4omp > out_v4_omp.out_ "
./run "<Path to mtx file> -t<threads> -c2 -v3omp > out_v3_omp.out_ "
./compare out_v4_omp.out_ out_v3_omp.out_
````
