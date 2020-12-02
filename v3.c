#include "mat.h"


/******************
 * Serial
 * ****************/

void find_triangles_v3(int* mat, bool __show_c, bool __show_info)
{

   

   int found = 0;

   int M = mat_get_M(mat);

   int *d_i = (int *)malloc(M*sizeof(int));
   if(d_i==NULL) exit(EXIT_FAILURE);

   int *d_j = (int *)malloc(M*sizeof(int));
   if(d_j==NULL) exit(EXIT_FAILURE);

   int *c = (int *)calloc(M, sizeof(int));
   if(c==NULL) exit(EXIT_FAILURE);

   int i_g_j = 0;


   // Start Timer
   time_t t;
   srand((unsigned) time(&t));
   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


   // i j k
   for(int j=0; j<M; j++)
   {

      int size_d_j = M;

      // Columns of j
      mat_cols(mat, j, &d_j, &size_d_j);

      if(size_d_j<1) continue;

      // For each Column
      // Find edges to the other columns of j
      for(int i_indx=0; i_indx<size_d_j; i_indx++)
      {

         int i = d_j[i_indx];

         // if(i==j) continue;

         int size_d_i = M;
         mat_cols(mat, i, &d_i, &size_d_i);

         for(int e_indx=0; e_indx<size_d_i; e_indx++)
         {
            
            int e = d_i[e_indx];

            // if(e==i) continue;
               
            for(int k_indx=0; k_indx<size_d_j; k_indx++)
            {
               
               int k = d_j[k_indx];

               // if(i==e||e==j) continue;

               if(e==k){
                  found+=1;
                  c[i]++;
                  c[k]++;
                  c[j]++;
               }

            }

         }

      }

   }

   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   if(__show_info)
      printf(" > V3 took %f s, Found %d triangles.\n", delta_us, found); 

   if(__show_c)
   {
      printf("\n");
      for(int i=0; i<M; i++){
         printf("%d ", c[i]);
      }

      printf("\n\n");
   }

   free(c);
   free(d_i);
   free(d_j);


   return;
}


