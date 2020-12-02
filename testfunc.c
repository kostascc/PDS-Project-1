#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>



uint32_t* vertexWiseTriangleCounts( uint32_t *coo_row, uint32_t *coo_col, uint32_t M, uint32_t nz );
uint32_t * mmarket_aux(bool __show_info, uint32_t* I, uint32_t* J, uint32_t M, uint32_t nz);
uint32_t * mat_get_u(uint32_t* mat);
uint32_t * mat_get_d(uint32_t* mat);
bool mat_xy_b(uint32_t* mat, uint32_t x, uint32_t y);
void mat_cols(uint32_t* mat, uint32_t x, uint32_t** d, uint32_t* size);
uint32_t mat_get_M(uint32_t* mat);
uint32_t mat_get_NZ(uint32_t* mat);
uint32_t* slice(uint32_t *arr, uint32_t start, uint32_t end);
void merge(
    uint32_t *I, uint32_t *J, /*uint32_t *V,*/                     // Result
    uint32_t *left_I, uint32_t *left_J, /*uint32_t *left_V,*/      // Left
    uint32_t *right_I, uint32_t *right_J, /*uint32_t *right_V,*/   // Right
    uint32_t leftLen, uint32_t rightLen);
void mergeSort(uint32_t *I, uint32_t *J, /*uint32_t *V,*/ uint32_t len);
void switcharoo_to_lower_triangle(uint32_t *I, uint32_t *J, uint32_t nz);



int main(){

    uint32_t I[] = {1,2,3,3,4,5,5,5,6,6,6,7,8,9,10,10,11,11,11};
    uint32_t J[] = {0,1,0,2,1,0,1,4,0,3,5,1,7,7,1,2,1,2,10};

    uint32_t* c = vertexWiseTriangleCounts( I, J, 12, 19 );
    
    printf("\n");
    for(uint32_t i=0; i<12; i++){
        printf("%d ", c[i]);
    }

    printf("\n\n");
}




uint32_t* vertexWiseTriangleCounts( uint32_t *coo_row, uint32_t *coo_col, uint32_t M, uint32_t nz )
{


    uint32_t* mat = mmarket_aux(false, coo_row, coo_col, M, nz);

   uint32_t found = 0;

   // uint32_t M = mat_get_M(mat);

   uint32_t *d_i = (uint32_t *)malloc(M*sizeof(uint32_t));
   if(d_i==NULL) exit(EXIT_FAILURE);

   uint32_t *d_j = (uint32_t *)malloc(M*sizeof(uint32_t));
   if(d_j==NULL) exit(EXIT_FAILURE);

   uint32_t *c = (uint32_t *)calloc(M, sizeof(uint32_t));
   if(c==NULL) exit(EXIT_FAILURE);

   // i j k
   for(uint32_t j=0; j<M; j++)
   {

      uint32_t size_d_j = M;

      // Columns of j
      mat_cols(mat, j, &d_j, &size_d_j);

      if(size_d_j<1) continue;

      // For each Column
      // Find edges to the other columns of j
      for(uint32_t i_indx=0; i_indx<size_d_j; i_indx++)
      {

         uint32_t i = d_j[i_indx];

         if(i==j) continue;

         uint32_t size_d_i = M;
         mat_cols(mat, i, &d_i, &size_d_i);

         for(uint32_t e_indx=0; e_indx<size_d_i; e_indx++)
         {
            
            uint32_t e = d_i[e_indx];

            if(e==i) continue;
               
            for(uint32_t k_indx=0; k_indx<size_d_j; k_indx++)
            {
               
               uint32_t k = d_j[k_indx];

               if(i==e||e==j) continue;

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



   
   free(d_i);
   free(d_j);

   return c;

//    free(c);

}





/*********Libs**********/

uint32_t * mmarket_aux(bool __show_info, uint32_t* I, uint32_t* J, uint32_t M, uint32_t nz)
{


    /********************
    ** Sort COO Matrix **
    ********************/

    if(__show_info)
        printf("[Begin Sorting...]\n");

    switcharoo_to_lower_triangle(I, J, nz);

    mergeSort(I, J, nz);


    /************************
    **  Convert COO to CSR **
    ************************/

    if(__show_info)
        printf("[Begin Creating CSR...]\n");

    // Upper / Row Position
    uint32_t *u = malloc(nz * sizeof(uint32_t));


    // Initialize U  array
    for (uint32_t i=0; i<nz; i++)
    {
        u[i] = 0;
    }

    for (uint32_t i = 0; i < nz; i++)
    {
        u[I[i] + 1] += 1;
    }

    for (uint32_t i = 0; i < M; i++)
    {
        u[i + 1] += u[i];
    }

    

    /*********************
    ** Create final mat **
    *********************/
    if(__show_info)
        printf("[Begin Creating Matrix...]\n");

    // Size of mat: 3*nz +3.
    // (+1 for M in [0],
    // +1 for nz in [1]).
    long malloc_size = ( M + nz + 3 ) * sizeof(uint32_t);

    // Final Matrix.
    // ([0] = M, [1] = NZ)
    uint32_t *mat = (uint32_t *) malloc(malloc_size);
    
    if(mat == NULL)
    {
        printf("Memory Allocation Failed Creating the Matrix (%ld bytes).\n", malloc_size); 
        exit(EXIT_FAILURE); 
    }
   


    mat[0] = M;
    mat[1] = nz;


    for(uint32_t i=0; i<M+2; i++)
    {
        // U
        mat[i + 2] = u[i];
    }
    
    for(uint32_t i=0; i<nz; i++)
    {
        // D
        mat[  M + i + 2 + 1 ] = J[i];
    }




    /***************
    ** Clean Up ! **
    ***************/

    // free(I);
    // I = NULL;

    // free(J);

    // free(val);
    // val = NULL;

    free(u);
    u = NULL;

    // fflush(stdin);
    // fflush(stdout);



    // Return final CSR matrix
	return mat;

}




uint32_t * mat_get_u(uint32_t* mat)
{

    uint32_t * u = (uint32_t *) malloc((mat[0]+1) * sizeof(uint32_t));  

    // mat[0] is the length M
    u[0] = mat[0]+1;

    for(uint32_t i=0; i<mat[0]+1; i++)
    {
        u[i+1] = mat[i+2];
    }


    return u;
}



uint32_t * mat_get_d(uint32_t* mat)
{

    uint32_t * d = (uint32_t *) malloc((mat[1]+1) * sizeof(uint32_t));  

    // mat[0] is the length M
    // mat[1] is the count NZ
    d[0] = mat[1];

    for(uint32_t i=0; i<mat[1]; i++)
    {
        d[i+1] = mat[i + mat[0]+3];
    }


    return d;

}





// Read matrix at the (X,Y) pouint32_t as boolean.
bool mat_xy_b(uint32_t* mat, uint32_t x, uint32_t y)
{

    // if(y>x)
    // {
    //     uint32_t t = x;
    //     x = y;
    //     y = t;
    // }
    
    if(x > mat[0] || x < 0)
    {
        printf("mat_xy_b: Array (x) out of bounds (%d).\n", x);
        exit(EXIT_FAILURE);
    }

    // Columns Begin
    uint32_t cols_begin = mat[x+2];

    // Columns End
    uint32_t cols_end = mat[x+3];

    if(cols_begin > cols_end)
    {
        printf("mat_xy_b: Error decoding CSR data (begin > end) (%d-%d).\n", cols_begin, cols_end);
        exit(EXIT_FAILURE);
    }
    else if(cols_begin == cols_end)
    {
        // No Connections
        return false;
    }


    // Columns Exist
    // Try finding the requested Y

    // Realloc with selected size
    // Can be optimized to hold a size M
    // for any size of lower array
    //realloc(*d, s*sizeof(uint32_t));

    for (uint32_t i = cols_begin+1; i < cols_end+1; i++)
    {

        
        if( ( mat[i + mat[0]+2] )==y)
        {
            return true;
        }
    }


    return false;



    // if(d == NULL || size==-1){
    //    printf("mat_xy_b: No Columns Found!\n");
    //    exit(EXIT_FAILURE); 
    //    return false;
    // }else if(size==0){
    //     return false;
    // }

    // for(uint32_t i=0; i<size; i++){

    //     if(d[i]==y){
    //         return true;
    //     }

    // }

    // return false;
}



// Read Columns of a certain row.
void mat_cols(uint32_t* mat, uint32_t x, uint32_t** d, uint32_t* size)
{


    // uint32_t s = *size;
    
    *(size) = -1;

    if(x > mat[0] || x < 0)
    {
        printf("mat_cols: Array out of bounds (%d).\n", x);
        return;
    }

    // Columns Begin
    uint32_t cols_begin = mat[x+2];

    // Columns End
    uint32_t cols_end = mat[x+3];

    if(cols_begin > cols_end)
    {
        printf("mat_cols: Error decoding CSR data (%d-%d).\n", cols_begin, cols_end);
        return;
    }
    else if(cols_begin == cols_end)
    {
        // No Connections
        *(size) = 0;
        return;
    }


    // Columns Exist
    // Try finding the requested Y

    *(size) = cols_end-cols_begin;

    // Realloc with selected size
    // Can be optimized to hold a size M
    // for any size of lower array
    //realloc(*d, s*sizeof(uint32_t));

    for (uint32_t i = cols_begin+1; i < cols_end+1; i++)
    {
        *(*d + i - cols_begin -1 ) = mat[i + mat[0]+2];
    }

    return;
}



uint32_t mat_get_M(uint32_t* mat)
{
    uint32_t M = mat[0];

    if(M>0)
    {
        return M;
    }
    else
    {
        printf("mat_get_M: Error / M out of bounds.\n");
        return -1;
    }
}



uint32_t mat_get_NZ(uint32_t* mat)
{
    uint32_t NZ = mat[1];

    if(NZ>0)
    {
        return NZ;
    }
    else
    {
        printf("mat_get_NZ: Error / NZ out of bounds.\n");
        return -1;
    }
}



/** Make a copy of an array from start to end - 1.
Equivalent to Python's arr[start:end] */
uint32_t* slice(uint32_t *arr, uint32_t start, uint32_t end)
{
    uint32_t *result = (uint32_t *) malloc((end - start) * sizeof(uint32_t));
    uint32_t i;
    for (i = start; i < end; i++)
    {
        result[i - start] = arr[i];
    }
    return result;
}

/** Merge left and right into result, overwriting everything in result. */
void merge(
    uint32_t *I, uint32_t *J, /*uint32_t *V,*/                     // Result
    uint32_t *left_I, uint32_t *left_J, /*uint32_t *left_V,*/      // Left
    uint32_t *right_I, uint32_t *right_J, /*uint32_t *right_V,*/   // Right
    uint32_t leftLen, uint32_t rightLen)                      // Length
{


    uint32_t i, j;

    // I
    i = 0;
    j = 0;
    while(i < leftLen && j < rightLen)
    {
        if (left_I[i] < right_I[j] ||
            (left_I[i] == right_I[j] && left_J[i] < right_J[j]) 
        )
        {
            I[i + j] = left_I[i];
            J[i + j] = left_J[i];
            // V[i + j] = left_V[i];

            i++;
        }
        else
        {
            I[i + j] = right_I[j];
            J[i + j] = right_J[j];
            // V[i + j] = right_V[j];

            j++;
        }

    }

    for(; i < leftLen; i++)
    {
        I[i + j] = left_I[i];
        J[i + j] = left_J[i];
        // V[i + j] = left_V[i];
    }
    for(; j < rightLen; j++)
    {
        I[i + j] = right_I[j];
        J[i + j] = right_J[j];
        // V[i + j] = right_V[j];
    }



    free(left_I);
    free(right_I);
    free(left_J);
    free(right_J);
}

// in-place merge sort
void mergeSort(uint32_t *I, uint32_t *J, /*uint32_t *V,*/ uint32_t len)
{
    if (len <= 1)
    {
        return;
    }
    uint32_t *left_I = slice(I, 0, len / 2 + 1);
    uint32_t *right_I = slice(I, len / 2, len);

    uint32_t *left_J = slice(J, 0, len / 2 + 1);
    uint32_t *right_J = slice(J, len / 2, len);


    mergeSort(left_I, left_J, /*left_V,*/ len / 2);
    mergeSort(right_I, right_J, /*right_V,*/ len - (len / 2));


    merge(I, J, /*V,*/ left_I, left_J, /*left_V,*/ right_I, right_J, /*right_V,*/ len / 2, len - (len / 2));
}



void switcharoo_to_lower_triangle(uint32_t *I, uint32_t *J, uint32_t nz){

    uint32_t t;

    for(uint32_t i=0; i<nz; i++){
        if(J[i] > I[i])
        {
            t = I[i];
            I[i] = J[i];
            J[i] = t;
        }
    }

}



