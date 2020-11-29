
void find_triangles_v3(int* mat)
{

   time_t t;
   srand((unsigned) time(&t));
   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   int found = 0;

   int M = mat_get_M(mat);

   int *d_i = malloc(M*sizeof(int));
   int *d_j = malloc(M*sizeof(int));

   int *c = (int *)malloc(M * sizeof(int));

   int i_g_j = 0;


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

                  c[i]+=1;
                  c[j]+=1;
                  c[k]+=1;
               }

            }

         }

      }

   }

   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf(" > V3 took %f s, Found %d triangles.\n", delta_us, found); 


   return;
}




// int __find_triangles_v3_cilk_f3(int* mat, int* j, int* i, int* d_j, int* size_d_j, int* e, int* d_i, int* size_d_i, int* c, int* found){

      
//    for(int k_indx=0; k_indx<*size_d_j; k_indx++){
      
      
//       int k = d_j[k_indx];


//       if(*i==*e||*e==*j) continue;

//       if(*e==k){
//             *(found)+=1;
//       }

//    }

//    return 1;
// }



   
// int __find_triangles_v3_cilk_f2(int* mat, int j, int i, int* d_j, int* size_d_j, int* c, int* found, int __threads)
//    {
      
//    int size_d_i;
//    int *d_i = mat_cols(mat, i, &size_d_i);


//    for(int e_indx=0; e_indx<size_d_i; e_indx++){

//       int e = d_i[e_indx];


//       if(e==i) continue;
      
//       cilk_spawn __find_triangles_v3_cilk_f3(mat, j, i, d_j, size_d_j, &e, d_i, &size_d_i, c, found);

//    }


//    //cilk_sync;

//    return 1;}
// }

   
   
   
   
// int __find_triangles_v3_cilk_f1(int* mat, int j, int* c,  int* found, int __threads){

//    int size_d_j;

//    // Columns of j
//    int *d_j = mat_cols(mat, *j, &size_d_j);



//    // For each Column
//    // Find edges to the other columns of j
//    for(int i_indx=0; i_indx<size_d_j; i_indx++){

      
//       int i = d_j[i_indx];

//       if(i==j) continue;

//       cilk_spawn __find_triangles_v3_cilk_f2(mat, j, i, d_j, &size_d_j, c, found);

//    }

//    //cilk_sync;

//    return 1;
// }








void __find_triangles_v3_cilk_f1(int* mat, int** d_i, int** d_j, int j, int* c, int* found, int __threads, int t)
{
   
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
            if(e==k) found[t]+=1;
               
         }

      }

   }

}


void find_triangles_v3_cilk(int* mat, int __threads)
{

   time_t t;
   srand((unsigned) time(&t));

   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   int M = mat_get_M(mat);

   int __threads_ = M > 1200 ? 1200 : M;

   int found[__threads_];

   int* c[__threads_];
   int* d_i[__threads_];
   int* d_j[__threads_]; 

   // Initialize Mallocs for each thread
   for(int tt=0; tt<__threads_; tt++)
   {
      d_i[tt]  = malloc(M * sizeof(int));
      d_j[tt]  = malloc(M * sizeof(int));
      c[tt]    = malloc(M * sizeof(int));
      found[tt]= 0;
   }


   // For each Second node j
   for(int j=0; j<M; j+=__threads_)
   {

      // For each thread
      for(int t=0; t<__threads_; t++)
      {

         if(t+j<M)
         {
            // Spawn Thread
            cilk_spawn __find_triangles_v3_cilk_f1(mat, &d_i, &d_j, (j+t), &c, &found, __threads_, t);
         }   
      }

      cilk_sync;

   }

   int found_ = 0;

   for(int tt=0; tt<__threads_; tt++)
   {
      found_ += found[tt];
   }
   
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);

   printf(" > V3/Cilk took %f s, Found %d triangles.\n", delta_us, found_); 


   return;
}