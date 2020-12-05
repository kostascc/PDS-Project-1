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


void __find_triangles_v3_cilk_f1(int* mat, int** d_i, int** d_j, int j, int** c, int* found, pthread_mutex_t* mux)
{
   
   int t = __cilkrts_get_worker_number();

   int size_d_j;
   mat_cols(mat, j, &(d_j[t]), &size_d_j);

   if(size_d_j<1) 
   {
      return;
   }

   for(int i_indx=0; i_indx<size_d_j; i_indx++)
   {

      int i = d_j[t][i_indx];

      if(i==j) continue;

      int size_d_i;
      mat_cols(mat, i, &(d_i[t]), &size_d_i);

      for(int e_indx=0; e_indx<size_d_i; e_indx++)
      {
         
         int e = d_i[t][e_indx];

         if(e==i) 
         {
            continue;
         }

         for(int k_indx=0; k_indx<size_d_j; k_indx++)
         {
            
            int k = d_j[t][k_indx];

            if(i==e||e==j) continue;

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


void find_triangles_v3_cilk(int* mat, bool __show_c, bool __show_info, int __threads)
{


   // Start Timer
   time_t t;
   srand((unsigned) time(&t));
   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


   int M = mat_get_M(mat);

   int __threads_ = __cilkrts_get_nworkers();

   int found[__threads_];

   int* c[__threads_];
   int* d_i[__threads_];
   int* d_j[__threads_]; 


   // Initialize Mallocs for each thread
   for(int tt=0; tt<__threads_; tt++)
   {
      d_i[tt]  = malloc(M * sizeof(int));
      if(d_i[tt]==NULL) exit(EXIT_FAILURE);

      d_j[tt]  = malloc(M * sizeof(int));
      if(d_j[tt]==NULL) exit(EXIT_FAILURE);

      c[tt]    = calloc(M, sizeof(int));
      if(c[tt]==NULL) exit(EXIT_FAILURE);

      found[tt]= 0;
   }


   pthread_mutex_t mux;
   pthread_mutex_init(&mux, NULL);


   // For each Second node j
   for(int j=0; j<M; j++)
   {

      // Spawn Thread
      cilk_spawn __find_triangles_v3_cilk_f1(mat, &d_i, &d_j, j, &c, &found, &mux);

   }

   cilk_sync;

   

   int found_ = 0;
   int* cc = (int *)calloc(M,sizeof(int));

   for(int tt=0; tt<__threads_; tt++)
   {
      found_ += found[tt];

      for(int i=0; i<M; i++)
         {
            cc[i] += c[tt][i];
         }

      free(d_i[tt]);
      free(d_j[tt]);
   }
   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   
   if(__show_info)
      printf(" > V3/Cilk took %f s, Found %d triangles.\n", delta_us, found_); 

   if(__show_c)
   {
      
      

      printf("\n");
      for(int i=0; i<M; i++)
      {
         printf("%d ", cc[i]);
      }

      printf("\n\n");

      free(cc);
   }

   for(int tt=0; tt<__threads_; tt++)
   {
      free(c[tt]);
   }

   return;
}