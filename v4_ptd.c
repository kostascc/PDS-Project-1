#include "v4_ptd.h"
#include "mat.h"


/******************
 * PThreads
 * ****************/


/**
 * Thread Data Struct
 **/

typedef struct _t_data 
{
    int* mat;
    int** d;
    int** c;
    int* found;
    int t;
    int* i;
    pthread_cond_t*  threadAvailSgnl;
    pthread_mutex_t* rwCriticalMux;
    bool *pts;
    int *availThreads;
    int M;
};


/**
 * Auxiliary function.
 * This is called for each pthread
 **/
void __v4_ptd_f1(void* t_data_);



/**
 * Main Function 
 **/
void v4_pthread(int* mat, bool __show_c, bool __show_info, int __threads)
{
    
    // Start Timer
    time_t t;
    srand((unsigned) time(&t));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    // Get Size M of the Matrix
    int M = mat_get_M(mat);

    // Set Threads
    int __threads_ = __threads ;
    // int __threads_ = __threads > 1 ? __threads-1 : 1 ;

    // Array of pThreads
    pthread_t ptd[__threads_];


    /**
     * Create an array of pointers, one
     * for every thread.
     * Each pointer will be initialized
     * with an dynamic array of static size
     **/
    int* d[__threads_];
    int found[__threads_];
    int* c[__threads_];
    
    // Initialize the above arrays 
    for(int tt=0; tt<__threads_; tt++)
    {
        d[tt]    = malloc(M * sizeof(int));
        c[tt]    = calloc(M, sizeof(int));
        found[tt]= 0;
    }



    pthread_mutex_t rwCriticalMux;      // Mutex: Critical Read/Write
    // pthread_mutexattr_settype(&rwCriticalMux, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&rwCriticalMux, NULL);


    // Global thread data
    struct _t_data t_data[__threads_];


    // The current Row.
    // This is incremented by the threads.
    int i = 0;
    

    // Initialize data for each thread
    for(int tt=0; tt<__threads_; tt++)
    {

        t_data[tt].mat      = mat;      // int*
        t_data[tt].d        = &d;       // int**
        t_data[tt].c        = &c;       // int**
        t_data[tt].found    = &found;   // int*
        t_data[tt].i        = &i;       // int*
        t_data[tt].M        = M;        // int
        t_data[tt].t        = tt;       // int

        // pthread_mutex_t*
        t_data[tt].rwCriticalMux = &rwCriticalMux;
        
    }


    int rc;         // Thread creation result.
    void* status;   // Thread status.


    /** 
     * For Every thread available, start
     * a job. The threads will find rows
     * to work on. 
     **/
    for(int tt=0; tt<__threads_; tt++)
    {

        // Start thread tt
        rc = pthread_create(
                &ptd[tt],                   // Thread
                NULL,                       // Parameters
                __v4_ptd_f1,                // Wrapper
                (void *) &(t_data[ tt ])    // Thread-Specific arguments
            );


        // Check thread creation
        if (rc)
        {
            // Errored, Exit
            printf("ERROR While creating thread (%d): %d\n", tt, rc);
            exit(-1);

        }

    }


    /**
     * Wait for every thread to finish
     **/
    for(int i=0; i<__threads_; i++)
    {

        rc = pthread_join( ptd[i], &status);

        if (rc) 
        {
            // Errored, Exit
            printf("ERROR: Thread %d didn't join. What a shame..\n", i);
            exit(-1);

        }
        
    }



    int c_ = 0;     // Collective triangles found


    /**
     * Now all threads are finished.
     * Each c vector has to be summed up
     * on cc.
     **/
    int* cc = calloc(M, sizeof(int));   // Collective C

    // For each thread
    for(int tt=0; tt<__threads_;tt++)
    {

        /**
         * For each row, add the value on
         * the Collective C vector.
         */
        for(int i=0; i<M; i++){
            cc[i] += c[tt][i];
        }

        c_ += found[tt];    
        
        // Free Unused arrays
        free(d[tt]);
        free(c[tt]);

    }


    // End Timer
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    float delta_us = (float) ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000)/ (1000000);


    // Show Info
    if(__show_info)
        printf(" > V4/PThreads took %f s, Found %d triangles.\n", delta_us, c_); 


    // Show c vector
    if(__show_c)
    {
        printf("\n");
        for(int i=0; i<M; i++){
            printf("%d ", cc[i]);   
        }

        printf("\n\n");
    }


    // Clean up
    free(cc);


    return;     // Finally gd

}





void __v4_ptd_f1(void* t_data_)
{

    /**
     * Thread Data.
     * Every thread has access to the
     * same variables.
     **/
    struct _t_data t_data = *((struct _t_data *)t_data_);


    // Thread Id
    int t = t_data.t;  


    /**
     * Row
     * No other Thread should ever have the
     * same i.
     **/ 
    int i;



    /** 
     * rwCriticalMux is locked only when making 
     * changes to the variable i.
     **/
    pthread_mutex_lock( (t_data.rwCriticalMux) );
    i = *(t_data.i);    // Take the next i
    *(t_data.i) = i+1;  // and increment by 1.
    pthread_mutex_unlock( (t_data.rwCriticalMux) );


    // If i is still in bounds
    while(i < t_data.M)
    {


        int d_;

        // Get Columns of i
        mat_cols(t_data.mat, i, &(t_data.d[t]), &d_);


        if(d_>0)    // i has columns
        {

            // For every column j
            for(int jj=0; jj<d_; jj++)
            {
                
                int j = t_data.d[t][jj];


                // For every column of i
                for(int i_=0; i_<d_; i_++){


                    // If there is an edge
                    if( mat_xy_b(t_data.mat, t_data.d[t][i_], j) ) 
                    {
                        // A triangle has been found!

                        t_data.found[ t ] += 1;


                        /**
                         * This is matrix multiplication.
                         * The rows and columns are added, 
                         * but the product is always 1, 
                         * so the sum is just incremented by 1.
                         **/

                        t_data.c[ t ][ i ] ++;
                        t_data.c[ t ][ j ] ++;
                        t_data.c[ t ][ t_data.d[ t ][ i_ ] ] ++;


                    }                
                        
                }
                
            }

        }


        
        pthread_mutex_lock( (t_data.rwCriticalMux) );
        i = *(t_data.i);    // Take the next i
        *(t_data.i) = i+1;  // and increment by 1.
        pthread_mutex_unlock( (t_data.rwCriticalMux) );
        
    }


    // Close Thread

    pthread_exit(NULL);

    return;

}
