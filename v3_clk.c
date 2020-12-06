#include "v3_clk.h"
#include "mat.h"
#include <cilk/cilk.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>



/******************
 * Cilk
 * ****************/



/**
 * Auxiliary Function
 **/
void __find_triangles_v3_cilk_f1(int* mat, int** d_i, int** d_j, int j, int** c, int* found, pthread_mutex_t* mux);




/**
 * Main Function
 **/
void find_triangles_v3_cilk(int* mat, bool __show_c, bool __show_info, int __threads)
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
    * Arrays d_i, d_j will be used as the
    * lower array (columns of CSR).
    * c is the final triangle count vector.
    * found is the sum of the triangles on each thread.
    **/ 
   int found[__threads_];
   int* c[__threads_];
   int* d_i[__threads_];
   int* d_j[__threads_]; 



   // Initialize arrays for each thread
   for(int tt=0; tt<__threads_; tt++)
   {
      d_i[tt]  = malloc(M * sizeof(int));
      if(d_i[tt]==NULL) exit(EXIT_FAILURE);

      d_j[tt]  = malloc(M * sizeof(int));
      if(d_j[tt]==NULL) exit(EXIT_FAILURE);

      c[tt]    = calloc(M, sizeof(int));
      if(c[tt]==NULL)   exit(EXIT_FAILURE);

      found[tt]= 0;
   }


   // Not used anywhere :(
   pthread_mutex_t mux;
   pthread_mutex_init(&mux, NULL);



   // For each row j
   for(int j=0; j<M; j++)
   {

      // Spawn Thread
      cilk_spawn __find_triangles_v3_cilk_f1(mat, &d_i, &d_j, j, &c, &found, &mux);

   }


   // Wait for threads to finish
   cilk_sync;


   int found_ = 0;

   // Collective C vector
   int* cc = (int *)calloc(M,sizeof(int));


   // For each thread
   for(int tt=0; tt<__threads_; tt++)
   {

      // Sum thread triangle count to 
      // Overall triangle count
      found_ += found[tt];

      // Sup up the c vector for every row
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
      printf(" > V3/Cilk took %f s, Found %d triangles.\n", delta_us, found_); 


   // Show vector c
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





void __find_triangles_v3_cilk_f1(int* mat, int** d_i, int** d_j, int j, int** c, int* found, pthread_mutex_t* mux)
{
   
   // Thread Id
   int t = __cilkrts_get_worker_number();

   int size_d_j;

   // Get Columns of row j
   mat_cols(mat, j, &(d_j[t]), &size_d_j);

   if(size_d_j<1)    // j has no columns
   {
      return;
   }


   // For each column i
   for(int i_indx=0; i_indx<size_d_j; i_indx++)
   {

      int i = d_j[t][i_indx];


      int size_d_i;

      // Get columns of row i
      mat_cols(mat, i, &(d_i[t]), &size_d_i);


      // For each column e
      for(int e_indx=0; e_indx<size_d_i; e_indx++)
      {
         
         int e = d_i[t][e_indx];


         // For each column of j
         for(int k_indx=0; k_indx<size_d_j; k_indx++)
         {
            
            int k = d_j[t][k_indx];


            // Found
            if(e==k) 
            {
               found[t]+=1;

               c[t][i]+=1;
               c[t][k]+=1;
               c[t][j]+=1;

            }
               
         }

      }

   }

}


