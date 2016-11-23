#include <stdio.h>
#include <omp.h>

int main(int argc,char ** argv) {
#ifdef _OPENMP
    printf("OK");
#endif
    return 0;
}
