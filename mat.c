#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "mat.h"
#include "msort.h"



/**
 * Returns Upper row of CSR
 * (Row index)
 **/
int * mat_get_u(int* mat)
{

    int * u = (int *) malloc((mat[0]+1) * sizeof(int));  

    // mat[0] is the length M
    u[0] = mat[0]+1;

    for(int i=0; i<mat[0]+1; i++)
    {
        u[i+1] = mat[i+2];
    }


    return u;
}


/**
 * Returns lower row of CSR
 * (Columns)
 **/
int * mat_get_d(int* mat)
{

    int * d = (int *) malloc((mat[1]+1) * sizeof(int));  

    // mat[0] is the length M
    // mat[1] is the count NZ
    d[0] = mat[1];

    for(int i=0; i<mat[1]; i++)
    {
        d[i+1] = mat[i + mat[0]+3];
    }


    return d;

}



// int * mat_get_v(int* mat){
//     int * v = (int *) malloc((mat[1]+1) * sizeof(int));  

//     // mat[0] is the length M
//     // mat[1] is the count NZ
//     v[0] = mat[1];

//     for(int i=0; i<mat[1]; i++){
//         v[i+1] = mat[i + mat[1] + mat[0]+3];
//     }


//     return v;
// }


// int mat_xy(int* mat, int x, int y){

//     // // mat[0] is the length M
//     // // mat[1] is the count NZ

//     // if(x > mat[0] || x < 0){
//     //     printf("mat_xy: Array out of bounds (%d).\n", x);
//     //     return -1;
//     // }

//     // // Columns Begin
//     // int cols_begin = mat[x+2];

//     // // Columns End
//     // int cols_end = mat[x+3];

//     // if(cols_begin > cols_end){
//     //     printf("mat_xy: Error decoding CSR data (%d-%d).\n", cols_begin, cols_end);
//     //     return -1;
//     // }
//     // else if(cols_begin == cols_end){
//     //     // No Connections
//     //     return 0;
//     // }


//     // // Columns Exist
//     // // Try finding the requested Y

//     // int* d = slice(mat_get_d(mat), cols_begin+1, cols_end+1);

//     // for(int i=0; i<cols_end-cols_begin; i++){

//     //     if(d[i]==y){
//     //         //printf("Found (%d,%d)\n", x, y);
//     //         return 1;
//     //     }
//     // }

//     // return 0;


//     int* d = (int *)mat_cols(mat, x);

//     if(d == NULL){
//        printf("mat_xy_b: No Columns Found!\n");
//        exit(EXIT_FAILURE); 
//        return -1;
//     }else if(d[0]==-1){
//         return 0;
//     }




//     for(int i=0; i< sizeof(d[0])/sizeof(d) ; i++){

//         if(d[i]==y){
//             return 1;
//         }

//     }

//     return 0;
    

// }




// Read matrix at the (X,Y) point as boolean.
bool mat_xy_b(int* mat, int x, int y)
{

    // if(y>x)
    // {
    //     int t = x;
    //     x = y;
    //     y = t;
    // }
    
    if(x > mat[0] || x < 0)
    {
        printf("mat_xy_b: Array (x) out of bounds (%d).\n", x);
        exit(EXIT_FAILURE);
    }

    // Columns Begin
    int cols_begin = mat[x+2];

    // Columns End
    int cols_end = mat[x+3];

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
    //realloc(*d, s*sizeof(int));

    for (int i = cols_begin+1; i < cols_end+1; i++)
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

    // for(int i=0; i<size; i++){

    //     if(d[i]==y){
    //         return true;
    //     }

    // }

    // return false;
}



// Read Columns of a certain row.
void mat_cols(int* mat, int x, int** d, int* size)
{


    // int s = *size;
    
    *(size) = -1;

    if(x > mat[0] || x < 0)
    {
        printf("mat_cols: Array out of bounds (%d).\n", x);
        exit(EXIT_FAILURE);
        return;
    }

    // Columns Begin
    int cols_begin = mat[x+2];

    // Columns End
    int cols_end = mat[x+3];

    if(cols_begin > cols_end)
    {
        printf("mat_cols: Error decoding CSR data (%d-%d).\n", cols_begin, cols_end);
        exit(EXIT_FAILURE);
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
    //realloc(*d, s*sizeof(int));

    for (int i = cols_begin+1; i < cols_end+1; i++)
    {
        *(*d + i - cols_begin -1 ) = mat[i + mat[0]+2];
    }

    return;
}



int mat_get_M(int* mat)
{
    int M = mat[0];

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



int mat_get_NZ(int* mat)
{
    int NZ = mat[1];

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



void _coo_row(int* mat)
{
    int M = mat_get_M(mat);
    int* d = malloc(M*sizeof(int));
    int d_;



    for(int i=0; i<M; i++){
        mat_cols(mat, i, &d, &d_);
        for(int ii=0; ii<d_; ii++){
            printf("%d,", i+1);
        }
    }

    printf("\n");

    // for(int i=0; i<M; i++){
    //     mat_cols(mat, i, &d, &d_);
    //     for(int ii=0; ii<d_; ii++){
    //         printf("%d,", d[ii]);
    //     }
    // }
}


void _coo_col(int* mat)
{
    int M = mat_get_M(mat);
    int* d = malloc(M*sizeof(int));
    int d_;



    // for(int i=0; i<M; i++){
    //     mat_cols(mat, i, &d, &d_);
    //     for(int ii=0; ii<d_; ii++){
    //         printf("%d,", i);
    //     }
    // }

    // printf("\n");

    for(int i=0; i<M; i++){
        mat_cols(mat, i, &d, &d_);
        for(int ii=0; ii<d_; ii++){
            printf("%d,", d[ii]+1);
        }
    }

    printf("\n");

}


void _coo(int* mat)
{
    int M = mat_get_M(mat);
    int* d = malloc(M*sizeof(int));
    int d_;



    for(int i=0; i<M; i++){
        mat_cols(mat, i, &d, &d_);
        for(int ii=0; ii<d_; ii++){
            printf("%d %d\n", i+1, d[ii]+1);
        }
    }

}

