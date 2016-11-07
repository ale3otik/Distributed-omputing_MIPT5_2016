#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <omp.h>

// USING CRITICAL SECTION

const double eps = 0.001;
const double abs_error_max = 0.00314159265358;
const int num_of_sections = 1e7;
const double PI = 3.14159265358;

double f(double x) {
    return 4.0 / (1.0 + x*x);
}

double square_of_section(double x1, double x2) {
    return (x2 - x1) * (f(x2) + f(x1)) / 2.0;
}

double calc_result() {
    double result_all = 0;
    double step = 1.0 / (double)num_of_sections;

    #pragma omp parallel reduction (+:result_all)
    {
        #pragma omp for
        for(int i = 0; i < num_of_sections; ++i) {
            result_all = result_all + square_of_section(step * i, step*(i+1));
        }
    }
    return result_all;
}

int main(int argc , char ** argv) {
    #ifndef _OPENMP
        printf("open mp is not supported");
        exit(-1);
    #endif

    
    FILE * f = fopen("integral2.txt","w");
    for(int nthr = 1; nthr < 13; ++nthr) {
        omp_set_num_threads(nthr);

        double begin,end;
        begin = omp_get_wtime();
        double result = calc_result();
        end = omp_get_wtime();
        printf("I = %lf ",result);
        printf("time elapsed : %lf\n",end-begin);
        fprintf(f,"%lf, ",end-begin);
        assert(fabs(result - PI) < abs_error_max);
    }

    fclose(f);
    
    return 0;
}