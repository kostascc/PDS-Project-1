#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "mat.h"
#include "mmarket.h"
#include <string.h>
#include <cilk/cilk.h>
#include "v2.c"
#include "v3.c"
#include "v4.c"




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



   /**********************************
   ** Welcome and change parameters **
   **********************************/

   printf("*************************************************\n");
   printf("*  Triangulinator - Made by K. Chatzis           \n");
   printf("*    <kachatzis@ece.auth.gr>                     \n");
   printf("*  ~ Configuration ~                             \n");
   printf("* Threads: %d                                    \n", __threads);
   printf("*************************************************\n");


   char _tmp_str[4];
   sprintf(_tmp_str, "%d", __threads);
   __cilkrts_set_param("nworkers", _tmp_str);


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


   /*****************
   ** Run versions **
   *****************/

   for(int i=0; i<argc; i++){

      if(strcmp(argv[i],"-v2")==0){
         find_triangles_v2(mat);
      }

      if(strcmp(argv[i],"-v3")==0){
         find_triangles_v3(mat);
      }

      if(strcmp(argv[i],"-v3clk")==0){
         find_triangles_v3_cilk(mat, __threads);
      }
      // if(strcmp(argv[i],"-v3omp")==0){
      //    find_triangles_v3_omp(mat, __threads);
      // }

      if(strcmp(argv[i],"-v4")==0){
         v4_simple(mat);
      }


      if(strcmp(argv[i],"-v4clk")==0){
         v4_cilk(mat, __threads);
      }
   }



   // int* d = malloc(mat_get_M(mat)*sizeof(int));
   // int s;      

   // for(int i=0; i<mat_get_M(mat); i++){
   //    printf("i:%d [", i);

   //    mat_cols(mat, i, &d, &s);

   //    for(int j=0; j<s; j++){
   //       printf("%d ", d[j]);
   //    }

   //    for(int j=0; j<mat_get_M(mat); j++){
   //       if(mat_xy_b(mat,i,j)){
   //          printf("(%d,%d), ", i, j);
   //       }
   //    }

   //    printf("]\n");

   // }




   /***************
   ** Clean Up ! **
   ***************/

   free(mat);


   // The End!

   return 0;
}



