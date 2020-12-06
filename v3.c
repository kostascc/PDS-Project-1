#include "v3.h"
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

void find_triangles_v3(int* mat, bool __show_c, bool __show_info)
{

   // Start Timer
   time_t t;
   srand((unsigned) time(&t));
   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   

   // Get Size M of the Matrix
   int M = mat_get_M(mat);


   // Triangles Found
   int found = 0;

   
   /**
    * Arrays d_i, d_j will be used as the
    * lower arrays (columns of CSR).
    * C is the final triangle count vector.
    **/ 
   int *d_i = (int *)malloc(M*sizeof(int));
   if(d_i==NULL)  exit(EXIT_FAILURE);

   int *d_j = (int *)malloc(M*sizeof(int));
   if(d_j==NULL)  exit(EXIT_FAILURE);

   int *c   = (int *)calloc(M, sizeof(int));
   if(c==NULL)    exit(EXIT_FAILURE);


   

   /**
    * The algorithm is as follows:
    * 
    * For each row j, find all the connected
    * columns i. 
    * Now every i column can be considered as
    * a row i. For each such row, find the connected
    * columns e.
    * Any of the e columns that also exist in i,
    * form a triangle.
    * 
    * Ha!
    * 
    * This requires any self loops to be removed
    * in advance!
    **/


   for(int j=0; j<M; j++)
   {

      int size_d_j = M;

      // Columns of j
      mat_cols(mat, j, &d_j, &size_d_j);

      if(size_d_j<1) continue;

      // For each Column of j
      // Find edges to the other columns of j
      for(int i_indx=0; i_indx<size_d_j; i_indx++)
      {


         // Column of j, becomes a i row
         int i = d_j[i_indx];


         // Find Columns of row i
         int size_d_i = M;
         mat_cols(mat, i, &d_i, &size_d_i);


         // For each column found,
         // Check if it already exists in i
         for(int e_indx=0; e_indx<size_d_i; e_indx++)
         {
            
         
            int e = d_i[e_indx];
               
            for(int k_indx=0; k_indx<size_d_j; k_indx++)
            {
               
               int k = d_j[k_indx];


               if(e==k)       // e exists in i
               {
                  found+=1;
                  
                  c[i]++;
                  c[k]++;
                  c[j]++;
               }

            }

         }

      }

   }

   

   // Stop Timer
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);


   // Show Info
   if(__show_info)
      printf(" > V3 took %f s, Found %d triangles.\n", delta_us, found); 


   // Show c vector
   if(__show_c)
   {
      printf("\n");
      for(int i=0; i<M; i++){
         printf("%d ", c[i]);
      }

      printf("\n\n");
   }



   // Clean up
   free(c);
   free(d_i);
   free(d_j);


   return;  // I can't believe it worked..
}


