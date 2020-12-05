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


void __v4_cilk_f1(int* mat, int** d, int i, int** c, int* found, pthread_mutex_t* mux)
{

    int t = __cilkrts_get_worker_number();

    int d_;
    mat_cols(mat, i, &(d[t]), &d_);

    if(d_<=0)
    {
        return;
    }

    for(int jj=0; jj<d_; jj++)
    {
        
        int j = d[t][jj];

        for(int i_=0; i_<d_; i_++){

            if( mat_xy_b(mat, d[t][i_], j) ) 
            {
                
                // pthread_mutex_lock(&mux);

                found[ t ] ++;

                c[ t ][ i ] ++;
                c[ t ][ j ] ++;
                c[ t ][ d[ t ][ i_ ] ] ++;

                // pthread_mutex_unlock(&mux);

            }                
                
        }
        
    }

    return;
}



void v4_cilk(int* mat, bool __show_c, bool __show_info, int __threads)
{
    
    // Start Timer
    time_t t;
    srand((unsigned) time(&t));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    int M = mat_get_M(mat);

    int __threads_ = __cilkrts_get_nworkers();

    int* d[__threads_];
    int found[__threads_];
    int* c[__threads_];
    
    int c_ = 0;

    // Initialize Mallocs for each thread
    for(int tt=0; tt<__threads_; tt++)
    {
        d[tt]    = malloc(M * sizeof(int));
        c[tt]    = calloc(M, sizeof(int));
        found[tt]= 0;
    }


    pthread_mutex_t mux;
    pthread_mutex_init(&mux, NULL);


    // For each row
    for(int i=0; i<M; i++)
    {

        cilk_spawn __v4_cilk_f1(mat, &d, i, &c, &found, &mux);
        
    }

    // pthread_mutex_unlock(&mux);
    cilk_sync;


    int* cc = calloc(M, sizeof(int));

    for(int tt=0; tt<__threads_;tt++){

        for(int i=0; i<M; i++){
            cc[i] += c[tt][i];
        }

        c_ += found[tt];
        
        free(d[tt]);
    }

//     free(c);
//    free(d);
//    free(dd);
   

   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

    if(__show_info)
        printf(" > V4/Cilk took %f s, Found %d triangles.\n", delta_us, c_); 


    if(__show_c)
    {
        printf("\n");
        for(int i=0; i<M; i++){
            printf("%d ", cc[i]);
        }

        printf("\n\n");
    }

return;

}

