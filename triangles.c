#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "mat.h"
#include "mmarket.h"
#include <cilk/cilk.h>






void find_triangles_v1(int* mat){

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


   int M = mat_get_M(mat);

   printf("M: %d\n", M);

   int *c = (int *)malloc((M+1) * sizeof(int));
   
   for(int k = 0; k < M-2; k++){
      for(int j = 0; j < M-1; j++){
         for(int i = 0; i < M; i++){
            if(mat_xy(mat,i,j)==1 && 
               mat_xy(mat,i,k)==1 && 
               mat_xy(mat,j,k)==1 )
            {
               c[i]+=1;
               c[j]+=1;
               c[k]+=1;
            }
         }
      }
   }


   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V1 took %f s\n", delta_us); 

   free(c);

   return;
}



void find_triangles_v1_cilk(int* mat){

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


   int M = mat_get_M(mat);

   int *c = (int *)malloc(M * sizeof(int));
   
   cilk_for(int k = 0; k < M; k++)
   {
      cilk_for(int j = 0; j < M-1; j++)
      {
         cilk_for(int i = 0; i < M-2; i++)
         {
            if( mat_xy(mat,i,j)==1 && mat_xy(mat,i,k)==1 && mat_xy(mat,j,k)==1 ){
               c[i]+=1;
               c[j]+=1;
               c[k]+=1;
            }
         }
      }
   }

   cilk_sync;


   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V1/Cilk took %f s\n", delta_us); 


   return;
}





void find_triangles_v2(int* mat){

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


   int M = mat_get_M(mat);

   int *c = (int *)malloc(M * sizeof(int));
   
   for(int i = 0; i < M-2; i++)
   {
      //printf("i=%d\n", i);
      for(int j = i+1; j < M-1; j++)
      {
         for(int k = j+1; k < M; k++)
         {
            if( mat_xy(mat,i,j)==1 && mat_xy(mat,i,k)==1 && mat_xy(mat,j,k)==1 ){
               c[i]+=1;
               c[j]+=1;
               c[k]+=1;
            }
         }
      }
   }


   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V2 took %f s\n", delta_us); 


   return;
}




void find_triangles_v2_cilk(int* mat){

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


   int M = mat_get_M(mat);

   int *c = (int *)malloc(M * sizeof(int));
   
   cilk_for(int i = 0; i < M-2; i++)
   {
      printf("i=%d\n", i);
      cilk_for(int j = i+1; j < M-1; j++)
      {
         cilk_for(int k = j+1; k < M; k++)
         {
            if( mat_xy(mat,i,j)==1 && mat_xy(mat,i,k)==1 && mat_xy(mat,j,k)==1 ){
               c[i]+=1;
               c[j]+=1;
               c[k]+=1;
            }
         }
      }
   }

   cilk_sync;


   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V2/Cilk took %f s\n", delta_us); 


   return;
}





void find_triangles_v3(int* mat){

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);


   int M = mat_get_M(mat);

   int *c = (int *)malloc(M * sizeof(int));
   
   for(int i = 0; i < M-2; i++)
   {
      //printf("i=%d\n", i);
      
      for(int j = i+1; j < M-1; j++)
      {
         for(int k = j+1; k < M; k++)
         {
            if( mat_xy(mat,i,j)==1 && mat_xy(mat,i,k)==1 && mat_xy(mat,j,k)==1 ){
               c[i]+=1;
               c[j]+=1;
               c[k]+=1;
            }
         }
      }
   }


   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V2 took %f s\n", delta_us); 


   return;
}





int main(int argc, char *argv[]){


   __cilkrts_set_param("nworkers", "3");


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
      if(strcmp(argv[i],"v1")==0){
         find_triangles_v1(mat);
      }

      if(strcmp(argv[i],"v2")==0){
         find_triangles_v2(mat);
      }

      if(strcmp(argv[i],"v2clk")==0){
         find_triangles_v2_cilk(mat);
      }

      if(strcmp(argv[i],"v3clk")==0){
         find_triangles_v3_cilk(mat);
      }
   }


   /***************
   ** Clean Up ! **
   ***************/

   free(mat);
   mat = NULL;


   // The End!

   return 0;
}



