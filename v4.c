#include "v4.h"
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



/******************
 * Serial
 * ****************/


void v4_simple(int* mat, bool __show_c, bool __show_info)
{



    // Start Timer
    time_t t;
    srand((unsigned) time(&t));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    
    // Get Size M of the Matrix
    int M = mat_get_M(mat);


    /**
    * Arrays d, dd will be used as the
    * lower arrays (columns of CSR).
    * c is the final triangle count vector.
    **/ 
    int *d  = (int *)malloc(M*sizeof(int));
    if(d==NULL)     exit(EXIT_FAILURE);

    int *dd = (int *)malloc(M*sizeof(int));
    if(dd==NULL)    exit(EXIT_FAILURE);

    int *c  = (int *)calloc(M, sizeof(int));
    if(c==NULL)     exit(EXIT_FAILURE);


    // Array size of d or dd.
    int d_, dd_ = 0;


    // Triangles Found
    int c_ = 0;



    // For each row
    for(int i=0; i<M; i++)
    {

        // Get Columns of i
        mat_cols(mat, i, &d, &d_);

        if(d_<=0)   // i has no columns
        {
            // try next row
            continue;
        }
            

        // For every column j
        for(int jj=0; jj<d_; jj++)
        {
            
            int j = d[jj];


            // For every column of i
            for(int i_=0; i_<d_; i_++)
            {

                // If there is an edge
                if( mat_xy_b(mat, d[i_], j) ) 
                {

                    // A triangle has been found!

                    c_++;


                    /**
                     * This is matrix multiplication.
                     * The rows and columns are added, 
                     * but the product is always 1, 
                     * so the sum is just incremented by 1.
                     **/

                    c[i]++;
                    c[j]++;
                    c[ d[i_] ]++;

                }
                    
            }
            
        }

    }



    // Clean Up
    free(d);
    free(dd);
   

    // Stop Timer
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);


    // Show info
    if(__show_info)
        printf(" > V4 took %f s, Found %d triangles.\n", delta_us, c_); 


    // Show vector C
    if(__show_c)
    {
        printf("\n");
        for(int i=0; i<M; i++){
            printf("%d ", c[i]);
        }

        printf("\n\n");
    }


    // Clean Up
   free(c);

   return;
}


