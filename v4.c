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

        for(int j=0; j<M; j++)
        {
            
            // int j = d[jj];


            if( mat_xy_b(mat, i, j) ){


                for(int i_=0; i_<M; i_++){

                        if( mat_xy_b(mat, i, i_) && 
                            mat_xy_b(mat, i_, j) ) 
                            c_+=1;

                     
                }


            }
            
        }


    }




   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf(" > V4 took %f s, Found %d triangles.\n", delta_us, c_); 

   return;
}

