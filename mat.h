/**
 * Usage: 
 *   int *mat = mat_create(x, y);
 *   mat_w(mat, x, y, value);
 *   mat_r(mat, x, y);
**/



#ifndef mat_h__
#define mat_h__


// Get Upper / Row position of CSR.
// (First item contains the array size)
extern int * mat_get_u(int* mat);


// Get Lower / Column position of CSR.
// (First item contains the array size)
extern int * mat_get_d(int* mat);


// Get Value of CSR.
// (First item contains the array size)
extern int * mat_get_v(int* mat);


// Read matrix at the (X,Y) point.
extern int mat_xy(int* mat, int x, int y);

// Get length M of the matrix
extern int mat_get_M(int* mat);

// Get count NZ of the matrix
extern int mat_get_NZ(int* mat);



// extern void mat_w(int** m, int x, int y, int v);

// extern void mat_create(int** m, int x, int y);

// extern void mat_print(int** m);

#endif  // mat_h__