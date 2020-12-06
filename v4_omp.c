#include "v4_omp.h"
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
#include "mat.h"
#include <omp.h>


/******************
 * OpenMP
 * ****************/




void v4_openmp(int* mat, bool __show_c, bool __show_info, int __threads)
{

    // Start Timer
    time_t t;
    srand((unsigned) time(&t));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    // Get size M of matrix
    int M = mat_get_M(mat);


    // Set threads
    // This is mostly used for testing
    int __threads_ =  __threads ;


    /**
    * Array d will be used as the
    * lower array (columns of CSR).
    * c is the final triangle count vector.
    * found is the sum of the triangles in a thread.
    **/ 
    int* d[__threads_];
    int found[__threads_];
    int* c[__threads_];
    


    // Collective Triangle sum
    int c_ = 0;

    // Initialize Mallocs for each thread
    for(int tt=0; tt<__threads_; tt++)
    {
        d[tt]    = malloc(M * sizeof(int));
        c[tt]    = calloc(M, sizeof(int));
        found[tt]= 0;
    }



    /**
     * The OpenMP Section 
     **/
    
    #pragma omp parallel
    {

        // For Each Row
        for(int i=0; i<M; i+=__threads_)
        {

            // one initialization for each thread
            #pragma omp for schedule(static,1) nowait
            for(int t=0; t<__threads_; t++)
            {


                /**
                 * Now instead of i,
                 * we use it as t+i.
                 * Make sure t+i is within bounds.
                 **/
                if(t+i<M)
                {


                    int d_;
                    
                    // Get Columns of t+i
                    mat_cols(mat, t+i, &(d[t]), &d_);

                    if(d_<=0)   // t+i has no columns
                    {
                        continue;
                    }

                    // For each column j
                    for(int jj=0; jj<d_; jj++)
                    {
                        
                        int j = d[t][jj];


                        // For each column of t+i
                        for(int i_=0; i_<d_; i_++){


                            // If there is an edge
                            if(mat_xy_b(mat, d[t][i_], j) ) 
                            {

                                // A triangle has been found!

                                found[t] += 1;

                                /**
                                 * This is matrix multiplication.
                                 * The rows and columns are added, 
                                 * but the product is always 1, 
                                 * so the sum is just incremented by 1.
                                 **/

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



    /**
     * Now all threads are finished.
     * Each c vector has to be summed up
     * on cc.
     **/
    int* cc = calloc(M, sizeof(int));   // Collective C

    // For each thread
    for(int tt=0; tt<__threads_;tt++)
    {

        /**
         * For each row, add the value on
         * the Collective C vector.
         */
        for(int i=0; i<M; i++){
            cc[i] += c[tt][i];
        }

        c_ += found[tt];    
        
        // Free Unused arrays
        free(d[tt]);
        free(c[tt]);

    }


    // Stop Timer
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   
    // Show Info
    if(__show_info)
        printf(" > V4/OpenMP took %f s, Found %d triangles.\n", delta_us, c_); 


    // Show c vector
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
