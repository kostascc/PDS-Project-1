#ifndef mmarket_h__
#define mmarket_h__

extern int * mmarket_import(char* filename, bool __show_info);

extern int * mmarket_aux(bool __show_info, int* I, int* J, int M, int nz);

#endif  // mmarket_h__