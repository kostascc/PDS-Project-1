#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "mat.h"
#include "mmarket.h"
#include <string.h>
#include <cilk/cilk.h>




void find_triangles_v1(int* mat){

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


   int M = mat_get_M(mat);

   int found = 0;

   int *c = (int *)malloc((M+1) * sizeof(int));
   
   for(int k = 0; k < M-2; k++){
      for(int j = 0; j < M-1; j++){
         if(j==k) continue;
         for(int i = 0; i < M; i++){
            if(i==j||i==k) continue;
            if(mat_xy_b(mat,i,j) && mat_xy_b(mat,i,k) && mat_xy_b(mat,j,k))
            {
               c[i]+=1;
               c[j]+=1;
               c[k]+=1;

               found += 1;
            }
         }
      }
   }


   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V1 took %f s, Found %d triangles.\n", delta_us, found); 

   free(c);

   return;
}



void find_triangles_v2(int* mat){

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   int found = 0;

   int M = mat_get_M(mat);

   int *c = (int *)malloc((M+1) * sizeof(int));
   
   for(int i = 0; i < M; i++)
   {
      for(int j = 0; j < i; j++)
      {
         if(i==j) continue;
         for(int k = 0; k < j; k++)
         {
            if(k==i||k==j) continue;
            if( mat_xy_b(mat,i,j) && mat_xy_b(mat,i,k) && mat_xy_b(mat,j,k) ){
               c[i]+=1;
               c[j]+=1;
               c[k]+=1;
               found+=1;
            }
         }
      }
   }


   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V2 took %f s, Found %d triangles.\n", delta_us, found); 

   free(c);

   return;
}




void find_triangles_v2_cilk(int* mat){

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


   int M = mat_get_M(mat);

   int found = 0;

   int *c = (int *)malloc(M * sizeof(int));
   
   cilk_for(int i = 0; i < M; i++)
   {
      cilk_for(int j = 0; j < i; j++)
      {
         if(j==i) continue;
         cilk_for(int k = 0; k < j; k++)
         {
            if(k==i||k==j) continue;
            if( mat_xy_b(mat,i,j) && mat_xy_b(mat,i,k) && mat_xy_b(mat,j,k) ){
               c[i]+=1;
               c[j]+=1;
               c[k]+=1;
               found+=1;
            }
         }
      }
   }



   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V2/Cilk took %f s, Found %d triangles.\n", delta_us, found); 

   free(c);

   return;
}





void find_triangles_v3(int* mat){

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   int found = 0;

   int M = mat_get_M(mat);

   int *c = (int *)malloc(M * sizeof(int));




   for(int j=0; j<M; j++){

      int size_d_j;

      // Columns of j
      int *d_j = mat_cols(mat, j, &size_d_j);



      // For each Column
      // Find edges to the other columns of j
      for(int i_indx=0; i_indx<size_d_j; i_indx++){

         
         int i = d_j[i_indx];


         if(i==j) continue;


         int size_d_i;
         int *d_i = mat_cols(mat, i, &size_d_i);


         for(int e_indx=0; e_indx<size_d_i; e_indx++){
            
            int e = d_i[e_indx];

            if(e==i) continue;
            
            for(int k_indx=0; k_indx<size_d_j; k_indx++){
               
               
               int k = d_j[k_indx];


               if(i==e||e==j) continue;

               if(e==k){
                  found+=1;
               }

            }

         }

      }

   }

   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V3 took %f s, Found %d triangles.\n", delta_us, found); 


   return;
}


void find_triangles_v3_cilk(int* mat){
   //TODO
   return;
}



int main(int argc, char *argv[]){


   int __threads = 8;


   for(int i=0; i<argc; i++){

      int _tmp;
      

      // Scan Threads
      if(sscanf(argv[i], "-t%d", &_tmp)){
         if(_tmp>0 && _tmp<1024)
            __threads = _tmp;
      }
      
      _tmp = NULL;

   }


   printf("*****************************************************\n");
   printf("*  ~ Configuration ~\n");
   printf("* Threads: %d\n", __threads);
   printf("*****************************************************\n");

   __cilkrts_set_param("nworkers", (char*)__threads);


   // Check Imput Arguments
   if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
		exit(1);
	}


   /******************
   ** Create Matrix **
   ******************/

   int* mat = mmarket_import(argv[1]); // Import MM


   /**************
   ** Run Tests **
   **************/

   for(int i=0; i<argc; i++){
      if(strcmp(argv[i],"-v1")==0){
         find_triangles_v1(mat);
      }

      if(strcmp(argv[i],"-v2")==0){
         find_triangles_v2(mat);
      }

      if(strcmp(argv[i],"-v2clk")==0){
         find_triangles_v2_cilk(mat);
      }

      if(strcmp(argv[i],"-v3")==0){
         find_triangles_v3(mat);
      }

      if(strcmp(argv[i],"-v3clk")==0){
         find_triangles_v3_cilk(mat);
      }
   }



   // // Test

   // for (int i=0; i<mat_get_M(mat); i++){

   //    for (int j=0; j<mat_get_M(mat); j++){
   //       if(mat_xy_b(mat,i,j)){
   //          printf("(%d,%d),", i, j);
   //       }
   //    }

   //    printf("\n");
      
   //    int size;
   //    int *d = mat_cols(mat, i, &size);

   //    printf("ROW[%d] size %d: ", i, size);

   //    for(int i=0; i<size; i++){
   //       printf("%d ", d[i]);
   //    }

   //    printf("\n");
   // }



   /***************
   ** Clean Up ! **
   ***************/

   free(mat);
   mat = NULL;


   // The End!

   return 0;
}



