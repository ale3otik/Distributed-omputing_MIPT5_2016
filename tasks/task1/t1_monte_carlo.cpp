#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

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

#define TIME_PODGONIAN CLOCKS_PER_SEC / 70.0 * 18.6
int main(int argc , char ** argv) {
    const int MAX_THREADS_QNT = 30;
    pthread_t threads[MAX_THREADS_QNT];
    long long initializing[MAX_THREADS_QNT][2]; // [rand_param][task size]

    FILE * F = fopen("t1_time_v1.txt","w");

    struct timespec begin,end;
    double elapsed;

    int num_threads ;
    for(num_threads = 1; num_threads < 13; ++num_threads) {
        long long points_qnt = 1e8;
        if(argc > 1) {
            num_threads = atoi(argv[1]);
            assert(num_threads > 0 && num_threads < MAX_THREADS_QNT + 1);
        }
        if(argc > 2) {
            points_qnt = atol(argv[2]);
            assert(points_qnt > 0);
        }
        // printf("threads : %d \npoints : %lld\n" , num_threads , points_qnt);

        long long begin,end;

        long long  task_size = points_qnt/(long long)num_threads;
        srand(1234321277);

        clock_gettime(CLOCK_REALTIME,&begin);
        for(int i = 0; i < num_threads; ++i) {
            initializing[i][0] = rand();
            initializing[i][1] = task_size + (i == num_threads - 1 ? points_qnt % num_threads : 0);
            if(pthread_create(&threads[i], NULL, worker, (void*) initializing[i])) {
                fprintf(stderr, "\nError: can't create thread\n");
                exit(-1);
            }
        }

        long long accepted = 0;
        for(int i = 0; i < num_threads; ++i) {
            long long result;
            pthread_join(threads[i], (void**)(&result));
            accepted += result;
        }
        fprintf(stderr, "%s\n", );
        clock_gettime(CLOCK_REALTIME,&end);
        elapsed = end.tv_sec - begin.tv_sec;
        elapsed += (end.tv_nsec - begin.tv_sec)/1e9;

        // printf("accepted/all = %lld / %lld\n" , accepted , points_qnt);
        printf("value = %lf\n", (double)accepted / points_qnt * (PI * PI));
        fprintf(F, "%lf ," ,elapsed);
        // printf("nthr %d , time elapsed : %lf" , num_threads, (end - begin) / (double)TIME_PODGONIAN);
    }
    fclose(F);
    return 0;
}
