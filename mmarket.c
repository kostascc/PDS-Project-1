#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <cilk/cilk.h>
#include "mat.h"
#include "mmio.h"
#include "mmarket.h"



int * mmarket_import(char* filename, bool __show_info){

    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;   
    int i;


    /**************
    ** Open File **
    **************/



    if(__show_info)
        printf("[Begin Reading File...]\n");


    if ((f = fopen(filename, "r")) == NULL) {
        printf("Couldn't Open the specified file: \"%s\"\n", filename);
        exit(EXIT_FAILURE);
    }


    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(EXIT_FAILURE);
    }


    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) && 
            mm_is_sparse(matcode) )
    {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(EXIT_FAILURE);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0)
        exit(EXIT_FAILURE);
        

    if(M!=N){
        printf("This Matrix is not Square!\n");
        exit(EXIT_FAILURE);
    }


    /* reseve memory for matrices */

    // COO Row index
    int* I = (int *) malloc(nz * sizeof(int));
    if(I==NULL) exit(EXIT_FAILURE);

    // COO Column index
    int* J = (int *) malloc(nz * sizeof(int));
    if(J==NULL) exit(EXIT_FAILURE);

    // COO Value
    // val = (double *) malloc(nz * sizeof(double));

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    int offset = 0;
    int xx, yy;

    for (i=0; i<nz; i++)
    {
        fscanf(f, "%d %d\n", &xx, &yy);

        if(xx == yy)
        {
            offset += 1;
            continue;
        }

        I[i - offset] = xx - 1;
        J[i - offset] = yy - 1;

    }


    if (f !=stdin) fclose(f);

    // Remove null items offset
    nz -= offset;


    return mmarket_aux(__show_info, I, J, M, nz);
}



int * mmarket_aux(bool __show_info, int* I, int* J, int M, int nz)
{

    
    time_t t;
    srand((unsigned) time(&t));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    /********************
    ** Sort COO Matrix **
    ********************/

    if(__show_info)
        printf("[Begin Sorting...]\n");

    switcharoo_to_lower_triangle(I, J, nz);

    mergeSort(I, J, nz);


    /************************
    **  Convert COO to CSR **
    ************************/

    if(__show_info)
        printf("[Begin Creating CSR...]\n");

    // Upper / Row Position
    int *u = malloc(nz * sizeof(int));


    // Initialize U  array
    for (int i=0; i<nz; i++)
    {
        u[i] = 0;
    }

    for (int i = 0; i < nz; i++)
    {
        u[I[i] + 1] += 1;
    }

    for (int i = 0; i < M; i++)
    {
        u[i + 1] += u[i];
    }

    

    /*********************
    ** Create final mat **
    *********************/
    if(__show_info)
        printf("[Begin Creating Matrix...]\n");

    // Size of mat: 3*nz +3.
    // (+1 for M in [0],
    // +1 for nz in [1]).
    long malloc_size = ( M + nz + 3 ) * sizeof(int);

    // Final Matrix.
    // ([0] = M, [1] = NZ)
    int *mat = (int *) malloc(malloc_size);
    
    if(mat == NULL)
    {
        printf("Memory Allocation Failed Creating the Matrix (%ld bytes).\n", malloc_size); 
        exit(EXIT_FAILURE); 
    }
    else if(__show_info)
    {
        
        printf("Matrix of ");

        if(malloc_size>=1000000)
        {
            printf("%.2f MBytes ", ((float)malloc_size/1000000));
        }
        else if(malloc_size>=1000)
        {
            printf("%.2f KBytes ", (float)malloc_size/1000);
        }
        else
        {
            printf("%d Bytes ", malloc_size);
        }       

        printf("created ( M: %d, Non-Zeroes: %d).\n", M, nz);
    }


    mat[0] = M;
    mat[1] = nz;


    for(int i=0; i<M+2; i++)
    {
        // U
        mat[i + 2] = u[i];
    }
    
    for(int i=0; i<nz; i++)
    {
        // D
        mat[  M + i + 2 + 1 ] = J[i];
    }




    /***************
    ** Clean Up ! **
    ***************/

    free(I);
    I = NULL;

    free(J);
    J = NULL;

    // free(val);
    // val = NULL;

    free(u);
    u = NULL;

    fflush(stdin);
    fflush(stdout);



    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);
    
    if(__show_info)
        printf(" > Import took %f s\n", delta_us);


    // Return final CSR matrix
	return mat;

}

