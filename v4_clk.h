#ifndef v4_clk_h__
#define v4_clk_h__

#include <stdbool.h>
#include <cilk/cilk.h>
#include <pthread.h>

void __v4_cilk_f1(int* mat, int** d, int i, int** c, int* found, pthread_mutex_t* mux);

void v4_cilk(int* mat, bool __show_c, bool __show_info, int __threads);

#endif