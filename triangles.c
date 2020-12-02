#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <cilk/cilk.h>
#include "mat.h"
#include "mmarket.h"
#include <string.h>
#include <omp.h>
#include "v2.c"
#include "v3_clk.c"
#include "v3_omp.c"
#include "v3.c"
#include "v4.c"
#include "v4_clk.c"
#include "v4_omp.c"
#include "v4_ptd.c"



int main(int argc, char *argv[]){


   int __threads = 8;
   bool __show_c = true;
   bool __show_info = true;


   for(int i=0; i<argc; i++){

      int _tmp;
      

      // Scan Threads
      if(sscanf(argv[i], "-t%d", &_tmp))
      {
         if(_tmp>0 && _tmp<1024)
            __threads = _tmp;
      }

      // C table
      if(sscanf(argv[i], "-c%d", &_tmp))
      {
         if(_tmp==0)
         {
            __show_c = false;
            __show_info = true;
         }
         if(_tmp==1)
         {
            __show_c = true;
            __show_info = true;
         }
         if(_tmp==2)
         {
            __show_c = true;
            __show_info = false;
         }
      }
      
      _tmp = NULL;

   }



   /**********************************
   ** Welcome and change parameters **
   **********************************/

   if(__show_info)
   {
     printf("*************************************************\n");
      printf("*  Triangulinator - Made by K. Chatzis           \n");
      printf("*    <kachatzis@ece.auth.gr>                     \n");
      printf("*  ~ Configuration ~                             \n");
      printf("* Threads: %d                                    \n", __threads);
      printf("*************************************************\n");
   }
   

   char _tmp_str[4];
   sprintf(_tmp_str, "%d", __threads);
   __cilkrts_set_param("nworkers", _tmp_str);
   omp_set_num_threads(__threads);


   // Check Imput Arguments
   if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
		exit(1);
	}


   /******************
   ** Create Matrix **
   ******************/

   int* mat = mmarket_import(argv[1], __show_info); // Import MM


   /*****************
   ** Run versions **
   *****************/

   for(int i=0; i<argc; i++){

      if(strcmp(argv[i],"-v2")==0){
         find_triangles_v2(mat);
      }

      if(strcmp(argv[i],"-v3")==0){
         find_triangles_v3(mat, __show_c, __show_info);
      }

      if(strcmp(argv[i],"-v3clk")==0){
         find_triangles_v3_cilk(mat, __show_c, __show_info, __threads);
      }

      if(strcmp(argv[i],"-v3omp")==0){
         find_triangles_v3_omp(mat, __show_c, __show_info, __threads);
      }

      if(strcmp(argv[i],"-v4")==0){
         v4_simple(mat, __show_c, __show_info);
      }


      if(strcmp(argv[i],"-v4clk")==0){
         v4_cilk(mat, __show_c, __show_info, __threads);
      }

      if(strcmp(argv[i],"-v4omp")==0){
         v4_openmp(mat, __show_c, __show_info, __threads);
      }
   }



// int M = mat_get_M(mat);
// int* d = malloc(M*sizeof(int));
//       int d_;



//    for(int i=0; i<M; i++){
//       mat_cols(mat, i, &d, &d_);
//       for(int ii=0; ii<d_; ii++){
//          printf("%d,", i);
//       }
//    }

// printf("\n\n");

//    for(int i=0; i<M; i++){
//       mat_cols(mat, i, &d, &d_);
//       for(int ii=0; ii<d_; ii++){
//          printf("%d,", d[ii]);
//       }
//    }





   /***************
   ** Clean Up ! **
   ***************/

   free(mat);


   // The End!

   return 0;
}



