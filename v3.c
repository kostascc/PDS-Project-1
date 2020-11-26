
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

        
        if(size_d_j<1) continue;

      // For each Column
      // Find edges to the other columns of j
      for(int i_indx=0; i_indx<size_d_j; i_indx++){

         
         int i = d_j[i_indx];

         if(i<0){
             printf("here: i:%d\n", d_j[i_indx]);
         }


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




    int __find_triangles_v3_cilk_f3(int* mat, int* j, int* i, int* d_j, int* size_d_j, int* e, int* d_i, int* size_d_i, int* c, int* found){

        
        for(int k_indx=0; k_indx<*size_d_j; k_indx++){
            
            
            int k = d_j[k_indx];


            if(*i==*e||*e==*j) continue;

            if(*e==k){
                *(found)+=1;
            }

        }

        return 1;
   }



   
   int __find_triangles_v3_cilk_f2(int* mat, int* j, int* i, int* d_j, int* size_d_j, int* c, int* found){

        // if(*i==*j) return 1;


        int size_d_i;
        int *d_i = mat_cols(mat, *i, &size_d_i);


        for(int e_indx=0; e_indx<size_d_i; e_indx++){
        
            int e = d_i[e_indx];


            if(e==*i) continue;
            
            //cilk_spawn __find_triangles_v3_cilk_f3(mat, j, i, d_j, size_d_j, &e, d_i, &size_d_i, c, found);

        }


        //cilk_sync;

        return 1;
   }

   
   
   
   
   int __find_triangles_v3_cilk_f1(int* mat, int* j, int* c,  int* found){

      int size_d_j;

      // Columns of j
      int *d_j = mat_cols(mat, *j, &size_d_j);



      // For each Column
      // Find edges to the other columns of j
      for(int i_indx=0; i_indx<size_d_j; i_indx++){

         
        int i = d_j[i_indx];

        if(i==*j) continue;

        cilk_spawn __find_triangles_v3_cilk_f2(mat, j, &i, d_j, &size_d_j, c, found);

      }

      cilk_sync;

      return 1;
   }




void find_triangles_v3_cilk(int* mat){
   
   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   int found = 0;

   int M = mat_get_M(mat);

   int *c = (int *)malloc(M * sizeof(int));


   for(int j=0; j<M; j++){

      cilk_spawn __find_triangles_v3_cilk_f1(mat, &j, &c, &found);

   }


   cilk_sync;

   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf("V3/Cilk took %f s, Found %d triangles.\n", delta_us, found); 


   return;
}