#include "mat.h"



void v4_simple(int* mat)
{

    time_t t;
    srand((unsigned) time(&t));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    int M = mat_get_M(mat);

    int* d = (int *) malloc(M * sizeof(int));
    int* dd = (int *) malloc(M * sizeof(int));

    int d_, dd_, c_ = 0;

    // For each row
    for(int i=0; i<M; i++)
    {

        mat_cols(mat, i, &d, &d_);

        for(int jj=0; jj<d_; jj++)
        {
            
            int j = d[jj];

            for(int i_=0; i_<d_; i_++){

                if( /*mat_xy_b(mat, i, d[i_]) &&*/ 
                    mat_xy_b(mat, d[i_], j) ) 
                    c_+=1;
                    
            }
            
        }

    }


   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf(" > V4 took %f s, Found %d triangles.\n", delta_us, c_); 

   return;
}



void __v4_cilk_f1(int* mat, int** d, int i, int* c, int* found, int __threads_, int t)
{

    int d_;
    mat_cols(mat, i, &(d[t]), &d_);

    for(int jj=0; jj<d_; jj++)
    {
        
        int j = d[t][jj];

        for(int i_=0; i_<d_; i_++){

            if( /*mat_xy_b(mat, i, d[i_]) &&*/ 
                mat_xy_b(mat, d[t][i_], j) ) 
                found[t] += 1;
                
        }
        
    }

    return;
}



void v4_cilk(int* mat, int __threads)
{
    
    int __threads_ = 1200;

    time_t t;
    srand((unsigned) time(&t));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    int M = mat_get_M(mat);

    int* d[__threads_];
    int found[__threads_];
    int* c[__threads_];
    
    int c_ = 0;

    // Initialize Mallocs for each thread
    for(int tt=0; tt<__threads_; tt++)
    {
        d[tt]    = malloc(M * sizeof(int));
        c[tt]    = malloc(M * sizeof(int));
        found[tt]= 0;
    }


    // For each row
    for(int i=0; i<M; i+=__threads_)
    {

        for(int t=0; t<__threads_; t++)
        {

            if(t+i<M)
            {

                // Spawn Thread
                cilk_spawn __v4_cilk_f1(mat, &d, i+t, &c, &found, __threads_, t);

            }

        }

        cilk_sync;
        
    }


    for(int i=0; i<__threads_;i++){
        c_ += found[i];
    }


   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf(" > V4/Cilk took %f s, Found %d triangles.\n", delta_us, c_); 

   return;
}

