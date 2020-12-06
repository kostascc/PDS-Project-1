#include "v3_omp.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mat.h"
#include <time.h>


/******************
 * OpenMP
 * ****************/

void find_triangles_v3_omp(int* mat, bool __show_c, bool __show_info, int __threads)
{

    // Start Timer
    time_t t;
    srand((unsigned) time(&t));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    // Get Size M of the Matrix
    int M = mat_get_M(mat);


    // Set Threads
    // Used mostly for testing
    int __threads_ = __threads ;
    


    /**
    * Arrays d_i, d_j will be used as the
    * lower array (columns of CSR).
    * c is the final triangle count vector.
    * found is the sum of the triangles in each thread.
    **/ 
    int* c[__threads_];
    int* d_i[__threads_];
    int* d_j[__threads_];
    int found[__threads_]; 



    // Initialize arrays for each thread
    for(int tt=0; tt<__threads_; tt++)
    {
        d_i[tt]  = malloc(M * sizeof(int));
        if(d_i[tt]==NULL)   exit(EXIT_FAILURE);

        d_j[tt]  = malloc(M * sizeof(int));
        if(d_j[tt]==NULL)   exit(EXIT_FAILURE);

        c[tt]    = calloc(M, sizeof(int));
        if(c[tt]==NULL)     exit(EXIT_FAILURE);

        found[tt]= 0;
    }


   


    /**
     * The OpenMP Section 
     **/

    #pragma omp parallel
    {


        // For each row j
        for(int j=0; j<M; j+=__threads_)
        {

            // one initialization for each thread
            #pragma omp for schedule(static,1) nowait
            for(int t=0; t<__threads_; t++)
            {


                /**
                 * Now instead of j,
                 * we use it as t+j.
                 * Make sure t+j is within bounds.
                 **/
                if(t+j<M)
                {

                
                    int size_d_j;

                    // Get columns of j+t
                    mat_cols(mat, j+t, &(d_j[t]), &size_d_j);

                    if(size_d_j<1)  // t+j has no columns
                    {
                        continue;
                    }


                    // For each column i
                    for(int i_indx=0; i_indx<size_d_j; i_indx++)
                    {

                        int i = d_j[t][i_indx];


                        int size_d_i;

                        // Get Columns of row i
                        mat_cols(mat, i, &(d_i[t]), &size_d_i);


                        // for each column e of row i
                        for(int e_indx=0; e_indx<size_d_i; e_indx++)
                        {
                            
                            int e = d_i[t][e_indx];


                            // For each Column k of row j
                            for(int k_indx=0; k_indx<size_d_j; k_indx++)
                            {
                            
                                int k = d_j[t][k_indx];


                                
                                if(e==k)    // e exists in i
                                {
                                    found[t]+=1;

                                    c[t][i]+=1;
                                    c[t][k]+=1;
                                    c[t][j+t]+=1;

                                }
                                
                            }

                        }

                    }

                }   

            }

        }

    }

    

    // Collective triangle count
    int found_ = 0;


    // Collective C vector
    int* cc = (int *)calloc(M,sizeof(int));


    // For each thread
    for(int tt=0; tt<__threads_; tt++)
    {

        // Sum thread count to Overall triangle count
        found_ += found[tt];

        // Sum Up the c vector for every row
        for(int i=0; i<M; i++)
        {
            cc[i] += c[tt][i];
        }

        // Free memory
        free(d_i[tt]);
        free(d_j[tt]);
        free(c[tt]);
    }


    
    // Stop Timer
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);


    // Show Info
    if(__show_info)
        printf(" > V3/OpenMP took %f s, Found %d triangles.\n", delta_us, found_); 

   
    // Show C vector
    if(__show_c)
    {
        printf("\n");
        for(int i=0; i<M; i++)
        {
            printf("%d ", cc[i]);
        }

        printf("\n\n");

   }



    // Clean Up
    free(cc);

    return;

}




