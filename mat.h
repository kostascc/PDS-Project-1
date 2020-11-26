#ifndef mat_h__
#define mat_h__


// Get Upper / Row position of CSR.
// (First item contains the array size)
int * mat_get_u(int* mat);


// Get Lower / Column position of CSR.
// (First item contains the array size)
int * mat_get_d(int* mat);


// Get Value of CSR.
// (First item contains the array size)
int * mat_get_v(int* mat);


// Read matrix at the (X,Y) point.
// extern int mat_xy(int* mat, int x, int y);


// Read matrix at the (X,Y) point as boolean.
bool mat_xy_b(int* mat, int x, int y);


// Read Columns of a certain row.
int * mat_cols(int* mat, int x, int* size);


// Get length M of the matrix
int mat_get_M(int* mat);

// Get count NZ of the matrix
int mat_get_NZ(int* mat);


#endif  // mat_h__