
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
      for(int j = 0; j < M; j++)
      {
         for(int k = 0; k < M; k++)
         {
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

   printf(" > V2 took %f s, Found %d triangles.\n", delta_us, found); 

   free(c);

   return;
}






// void find_triangles_v2_cilk(int* mat){

//    time_t t;
//    srand((unsigned) time(&t));

//    struct timespec start, end;
//    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


//    int M = mat_get_M(mat);

//    int found = 0;

//    int *c = (int *)malloc(M * sizeof(int));
   
//    cilk_for(int i = 0; i < M; i++)
//    {
//       cilk_for(int j = 0; j < i; j++)
//       {
//          if(j==i) continue;
//          cilk_for(int k = 0; k < j; k++)
//          {
//             if(k==i||k==j) continue;
//             if( mat_xy_b(mat,i,j) && mat_xy_b(mat,i,k) && mat_xy_b(mat,j,k) ){
//                c[i]+=1;
//                c[j]+=1;
//                c[k]+=1;
//                found+=1;
//             }
//          }
//       }
//    }



   
//    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//    float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

//    printf("V2/Cilk took %f s, Found %d triangles.\n", delta_us, found); 

//    free(c);

//    return;
// }
