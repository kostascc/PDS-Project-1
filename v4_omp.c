#include "mat.h"
#include <omp.h>


/******************
 * OpenMP
 * ****************/



void __v4_openmp_f1(int* mat, int** d, int i, int* c, int* found, int __threads_, int t)
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



void v4_openmp(int* mat, bool __show_c, bool __show_info, int __threads)
{

    int M = mat_get_M(mat);

    int __threads_ = M > 1200 ? 1200 : M;

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


    
    // Start Timer
    time_t t;
    srand((unsigned) time(&t));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    // For each row
    for(int i=0; i<M; i+=__threads_)
    {

        #pragma omp parallel
        {
            #pragma omp for schedule(dynamic) nowait
            for(int t=0; t<__threads_; t++)
            {

                if(t+i<M)
                {

                    int d_;
                    mat_cols(mat, t+i, &(d[t]), &d_);

                    for(int jj=0; jj<d_; jj++)
                    {
                        
                        int j = d[t][jj];

                        for(int i_=0; i_<d_; i_++){

                            if(mat_xy_b(mat, d[t][i_], j) ) 
                            {
                                found[t] += 1;

                                c[t][i+t] +=1;
                                c[t][j] +=1;
                                c[t][ d[t][i_] ] +=1;
                            }   
                        }
                    }
                }
            }
        }               
    }


    int* cc = calloc(M, sizeof(int));

    for(int tt=0; tt<__threads_;tt++){

        for(int i=0; i<M; i++){
            cc[i] += c[tt][i];
        }

        c_ += found[tt];
        
        free(d[tt]);
        free(c[tt]);
    }



   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   
   if(__show_info)
        printf(" > V4/OpenMP took %f s, Found %d triangles.\n", delta_us, c_); 


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
