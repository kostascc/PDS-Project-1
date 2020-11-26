#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <cilk/cilk.h>
#include "mat.h"
#include "mmio.h"



int * mmarket_import(char* filename){

    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;   
    int i;
    //double *val;


    /**************
    ** Open File **
    **************/


   time_t t;
   srand((unsigned) time(&t));
   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    printf("[Begin Reading File...]\n");

    if ((f = fopen(filename, "r")) == NULL) {
        printf("Couldn't Open the specified file: \"%s\"\n", filename);
        exit(1);
    }


    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }


    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) && 
            mm_is_sparse(matcode) )
    {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0)
        exit(1);
        

    if(M!=N){
        printf("This Matrix is not Square!\n");
        exit(EXIT_FAILURE);
    }


    /* reseve memory for matrices */

    // COO Row index
    int* I = (int *) malloc(nz * sizeof(int));

    // COO Column index
    int* J = (int *) malloc(nz * sizeof(int));

    // COO Value
    // val = (double *) malloc(nz * sizeof(double));

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    for (i=0; i<nz; i++)
    {
        fscanf(f, "%d %d\n", &I[i], &J[i]);
        I[i]--;  /* adjust from 1-based to 0-based */
        J[i]--;
    }

    if (f !=stdin) fclose(f);
    


    /********************
    ** Sort COO Matrix **
    ********************/

    printf("[Begin Sorting...]\n");


    mergeSort(I, J, nz);


    // int i_tmp, j_tmp, v_tmp;
    // bool i_grt, j_grt, j_eq, i_eq;

    // for (i = 0; i < nz-1; i++) {
    //     for (int j = 0; j < nz-i-1; j++){
        
    //         i_grt = I[j]>I[j+1];
    //         j_grt = J[j]>J[j+1];
    //         j_eq = J[j]==J[j+1];
    //         i_eq = I[j]==I[j+1];
            
    //         if( ( j_grt && i_eq ) || i_grt ){
    //             i_tmp = I[j];
    //             j_tmp = J[j];
    //             v_tmp = val[j];

    //             I[j] = I[j+1];
    //             J[j] = J[j+1];
    //             val[j] = val[j+1];

    //             I[j+1] = i_tmp;
    //             J[j+1] = j_tmp;
    //             val[j+1] = v_tmp;
    //         }
    //     }
    // }
        

    // // Clean variables

    // i_tmp = NULL;
    // j_tmp = NULL;
    // v_tmp = NULL;
    // i_grt = NULL;
    // j_grt = NULL;
    // j_eq = NULL;


    if(nz<100)
        for(int i=0; i<nz; i++){
            printf("(%d,%d): %d\n", I[i], J[i], 1);
        }
             


    /************************
    **  Convert COO to CSR **
    ************************/

    printf("[Begin Creating CSR...]\n");

    // Upper / Row Position
    int *u = malloc(nz * sizeof(int));


    // Initialize U  array
    for (int i=0; i<nz; i++){
        u[i] = 0;
    }

    for (int i = 0; i < nz; i++){
        u[I[i] + 1] += 1;
    }

    for (int i = 0; i < M; i++){
        u[i + 1] += u[i];
    }

    

    /*********************
    ** Create final mat **
    *********************/

    printf("[Begin Creating Matrix...]\n");

    // Size of mat: 3*nz +3.
    // (+1 for M in [0],
    // +1 for nz in [1]).
    long malloc_size = ( M + nz + 3 ) * sizeof(int);

    // Final Matrix.
    // ([0] = M, [1] = NZ)
    int *mat = (int *) malloc(malloc_size);
    
    if(mat == NULL){
        printf("Memory Allocation Failed Creating the Matrix (%ld bytes).\n", malloc_size); 
        exit(EXIT_FAILURE); 
    }else{
        printf("Matrix of ");

        if(malloc_size>=1000000){
            printf("%.2f MBytes ", ((float)malloc_size/1000000));
        }
        else if(malloc_size>=1000){
            printf("%.2f KBytes ", (float)malloc_size/1000);
        }
        else{
            printf("%d Bytes ", malloc_size);
        }       

        printf("created.\n");
    }


    mat[0] = M;
    mat[1] = nz;


    for(int i=0; i<M+2; i++){
        // U
        mat[i + 2] = u[i];
    }
    
    for(int i=0; i<nz; i++){

        // D
        mat[  M + i + 2 + 1 ] = J[i];

        // V (Forced to '1')
        // mat[M + nz + i + 2 + 1 ] = 1;
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
    printf("Importing took %f s\n", delta_us);


    // Return final CSR matrix
	return mat;
}

