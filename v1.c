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