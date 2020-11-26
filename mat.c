#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "msort.h"




int * mat_get_u(int* mat){

    int * u = (int *) malloc((mat[0]+1) * sizeof(int));  

    // mat[0] is the length M
    u[0] = mat[0]+1;

    for(int i=0; i<mat[0]+1; i++){
        u[i+1] = mat[i+2];
    }


    return u;
}



int * mat_get_d(int* mat){

    int * d = (int *) malloc((mat[1]+1) * sizeof(int));  

    // mat[0] is the length M
    // mat[1] is the count NZ
    d[0] = mat[1];

    for(int i=0; i<mat[1]; i++){
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


int mat_xy(int* mat, int x, int y){

    // mat[0] is the length M
    // mat[1] is the count NZ

    if(x > mat[0] || x < 0){
        printf("mat_xy: Array out of bounds (%d).\n", x);
        return -1;
    }

    // Columns Begin
    int cols_begin = mat[x+2];

    // Columns End
    int cols_end = mat[x+3];

    if(cols_begin > cols_end){
        printf("mat_xy: Error decoding CSR data (%d-%d).\n", cols_begin, cols_end);
        return -1;
    }
    else if(cols_begin == cols_end){
        // No Connections
        return 0;
    }


    // Columns Exist
    // Try finding the requested Y

    int* d = slice(mat_get_d(mat), cols_begin+1, cols_end+1);

    for(int i=0; i<cols_end-cols_begin; i++){

        if(d[i]==y){
            //printf("Found (%d,%d)\n", x, y);
            return 1;
        }
    }

    return 0;
    

}



int mat_get_M(int* mat){
    int M = mat[0];
    if(M>0){
        return M;
    }
    else{
        printf("mat_get_M: Error / M out of bounds.\n");
        return -1;
    }
}



int mat_get_NZ(int* mat){
    int NZ = mat[1];
    if(NZ>0){
        return NZ;
    }
    else{
        printf("mat_get_NZ: Error / NZ out of bounds.\n");
        return -1;
    }
}


// int mat_r(int** m, int x, int y){
//     return (int)(*( (m+1 + (**(m)) * x + y) ));
// }


// void mat_w(int** m, int x, int y, int v){
//     printf("FindMult: %d\n", (*m));

//     //printf("w_addr: %u",*( m+1 + ( *(m) * x + y ) );
    
//     if (*m==NULL){
//         printf("Memory Allocation Failed while writing in [%d,%d] on a %dx%d Matrix.\n", x, y, m[0], m[0]); 
//         exit(EXIT_FAILURE);
//     }

//     *( m+1 + (int)(*m)  * x + y ) = v;

//     printf(" -> Success\n");
// }


// void mat_create(int** m, int x, int y){

//     printf("Creating Matrix %dx%d.\n", x, y);

//     *m = NULL;
//     *m = (int*) malloc(( x * y + 1 ) * sizeof(int)) ;

//     **(m) = x;

//     printf("asd2\n");
//     if(*m == NULL){
//         printf("Memory Allocation Failed Creating the Matrix (%dx%d / %dbytes) (Addr:%u)\n", x, y, x * y * sizeof(int), &m[1]); 
//         exit(EXIT_FAILURE); 
//     }

//     printf("Matrix Created (Addr:%u, mult:%d)\n", m+1, **(m));

//     return;
// }


// int mat_print(int** m){

//     int x_mult = *(m);

//     for(int i=0; i<x_mult; i++){
//         for(int j=0; j<x_mult; j++){
//             //if( *(mat + x_mult * i + j) != 0 ) {
//                 //printf("%02.19lg\t", *(mat + x_mult * i + j));
//                 printf(*(m+1 + x_mult * i + j) ? "o  " : "   ");
//             //}else{
//                 //printf(" ");
//             //}
//         }

//         printf("\n");
//     }
    
// }