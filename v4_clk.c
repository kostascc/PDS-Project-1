#include "v4_clk.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mat.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <cilk/cilk.h>
#include <pthread.h>


/******************
 * Cilk
 * ****************/



/**
 * Auxiliary Function
 **/
void __v4_cilk_f1(int* mat, int** d, int i, int** c, int* found, pthread_mutex_t* mux);




/**
 * Main Function
 **/
void v4_cilk(int* mat, bool __show_c, bool __show_info, int __threads)
{
    
    // Start Timer
    time_t t;
    srand((unsigned) time(&t));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    // Get Size M of Matrix
    int M = mat_get_M(mat);


    // Set Number of threads
    int __threads_ = __cilkrts_get_nworkers();


    /**
    * Array d will be used as the
    * lower array (columns of CSR).
    * c is the final triangle count vector.
    * found is the sum of the triangles.
    **/ 
    int* d[__threads_];
    int found[__threads_];
    int* c[__threads_];
    

    // Collective Triangle sum
    int c_ = 0;

    // Initialize arrays for each thread
    for(int tt=0; tt<__threads_; tt++)
    {
        d[tt]    = malloc(M * sizeof(int));
        c[tt]    = calloc(M, sizeof(int));
        found[tt]= 0;
    }


    // Not used
    // I leave it here for now
    pthread_mutex_t mux;
    pthread_mutex_init(&mux, NULL);


    // For each row i
    for(int i=0; i<M; i++)
    {

        // Spawn a thread
        cilk_spawn __v4_cilk_f1(mat, &d, i, &c, &found, &mux);
        
    }


    // Wait for threads to finish
    cilk_sync;


    // Collective C vector
    int* cc = calloc(M, sizeof(int));


    // For each thread
    for(int tt=0; tt<__threads_;tt++)
    {

        // Sum Up the c vector for every row
        for(int i=0; i<M; i++)
        {
            cc[i] += c[tt][i];
        }

        // Sum thread count to Overall triangle count
        c_ += found[tt];
        
        // Free memory
        free(d[tt]);
        free(c[tt]);

    }

   
    // End Timer
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);


    // Show Info
    if(__show_info)
        printf(" > V4/Cilk took %f s, Found %d triangles.\n", delta_us, c_); 


    // Show C vector
    if(__show_c)
    {
        printf("\n");
        for(int i=0; i<M; i++){
            printf("%d ", cc[i]);
        }

        printf("\n\n");
    }


    // Clean Up
    free(cc);

    return;

}






void __v4_cilk_f1(int* mat, int** d, int i, int** c, int* found, pthread_mutex_t* mux)
{

    // Thread Id
    int t = __cilkrts_get_worker_number();

    int d_;

    // Get Columns of i
    mat_cols(mat, i, &(d[t]), &d_);

    if(d_<=0)   // i has no columns
    {
        return;
    }


    // For each column j
    for(int jj=0; jj<d_; jj++)
    {
        
        int j = d[t][jj];


        // For Each column of i
        for(int i_=0; i_<d_; i_++){

            // if there is an edge
            if( mat_xy_b(mat, d[t][i_], j) ) 
            {
                
                // A triangle has been found!
                found[ t ] ++;

                /**
                 * This is matrix multiplication.
                 * The rows and columns are added, 
                 * but the product is always 1, 
                 * so the sum is just incremented by 1.
                 **/
                
                c[ t ][ i ] ++;
                c[ t ][ j ] ++;
                c[ t ][ d[ t ][ i_ ] ] ++;

            }                
                
        }
        
    }


    return;
}

