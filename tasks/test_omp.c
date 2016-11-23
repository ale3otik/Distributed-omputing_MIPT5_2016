#include<omp.h>
#include<stdio.h>
int main() {
#ifdef _OPENMP
    printf("wwooork:");
#endif
    printf("end");
    return 0;
}
