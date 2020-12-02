#include "mat.h"


/******************
 * Serial
 * ****************/


void v4_simple(int* mat, bool __show_c, bool __show_info)
{

    
    int M = mat_get_M(mat);

    int *d = malloc(M*sizeof(int));
    if(d==NULL) exit(EXIT_FAILURE);

    int *dd = malloc(M*sizeof(int));
    if(dd==NULL) exit(EXIT_FAILURE);

    int *c = (int *)calloc(M, M*sizeof(int));
    if(c==NULL) exit(EXIT_FAILURE);

    int d_, dd_, c_ = 0;

    // Start Timer
    time_t t;
    srand((unsigned) time(&t));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    // For each row
    for(int i=0; i<M; i++)
    {

        mat_cols(mat, i, &d, &d_);

        for(int jj=0; jj<d_; jj++)
        {
            
            int j = d[jj];

            for(int i_=0; i_<d_; i_++){

                if( mat_xy_b(mat, d[i_], j) ) 
                {
                    c_++;

                    c[i]++;
                    c[j]++;
                    c[ d[i_] ]++;
                }
                    
            }
            
        }

    }


   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

    if(__show_info)
        printf(" > V4 took %f s, Found %d triangles.\n", delta_us, c_); 


    if(__show_c)
    {
        printf("\n");
        for(int i=0; i<M; i++){
            printf("%d ", c[i]);
        }

        printf("\n\n");
    }

   free(c);
   free(d);
   free(dd);

   return;
}


