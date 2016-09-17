#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
/***
*
* [0: PI]
* PI = 3,1415926535
* 
*
***/

#define PI 3.1415926535

inline double f(double x, double y) {
    return x * y;
}

inline bool check_y(double x, double y) {
    return y < sin(x);
}

inline bool check_point(double x , double y , double z) {
    return check_y(x,y) && (z < f(x,y));
}


void * worker(void * arg) {
    long long * param = (long long *) arg;
    unsigned int rand_param = param[0];
    long long size = param[1];

    long long result = 0;
    for(long long i = 0; i < size; ++i) {
        double x = (double)rand_r(&rand_param)/RAND_MAX * PI;
        double y = (double)rand_r(&rand_param)/RAND_MAX;
        double z = (double)rand_r(&rand_param)/RAND_MAX * PI;

        if(check_point(x, y, z)) ++result;
    }
    return (void *) result;
}


int main(int argc , char ** argv) {  
    const int MAX_THREADS_QNT = 30;
    pthread_t threads[MAX_THREADS_QNT];
    long long initializing[MAX_THREADS_QNT][2]; // [rand_param][task size]

    int num_threads = 8;
    long long points_qnt = 1e9;
    if(argc > 1) {
        num_threads = atoi(argv[1]);
        assert(num_threads > 0 && num_threads < MAX_THREADS_QNT + 1);
    }
    if(argc > 2) {
        points_qnt = atol(argv[2]);
        assert(points_qnt > 0);
    }
    printf("threads : %d \npoints : %lld\n" , num_threads , points_qnt);
    
    long long  task_size = points_qnt/(long long)num_threads;
    srand(1234321277);
    for(int i = 0; i < num_threads; ++i) {
        initializing[i][0] = rand(); 
        initializing[i][1] = task_size + (i == num_threads - 1 ? points_qnt % num_threads : 0);
        if(pthread_create(&threads[i], NULL, worker, (void*) initializing[i])) {
            fprintf(stderr, "\nError: can't create thread\n");
            exit(-1);
        }
    }

    long long accepted;
    for(int i = 0; i < num_threads; ++i) {
        long long result;
        pthread_join(threads[i], (void**)(&result));
        accepted += result;
    }

    printf("accepted/all = %lld / %lld\n" , accepted , points_qnt);
    printf("value = %lf\n", (double)accepted / points_qnt * (PI * PI));
    return 0;
}