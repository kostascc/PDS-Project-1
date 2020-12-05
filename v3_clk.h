#ifndef v3_clk_h__
#define v3_clk_h__

#include <stdbool.h>
#include <cilk/cilk.h>
#include <pthread.h>

void __find_triangles_v3_cilk_f1(int* mat, int** d_i, int** d_j, int j, int** c, int* found, pthread_mutex_t* mux);

void find_triangles_v3_cilk(int* mat, bool __show_c, bool __show_info, int __threads);

#endif