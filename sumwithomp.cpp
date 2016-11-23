#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <ctime>
using std::cout;
using std::endl;
using std::vector;

int main(int argc, char* argv[]) {
        int n = atoi(argv[1]);
        int th = atoi(argv[2]);

        vector <int> v(n);
        double start, end;

        srand(time(NULL));

        for(int i = 0;i < n;++i) {
                v[i] = rand();
        }

        start = omp_get_wtime();
        double check_sum = 0;
        for(int i = 0;i < n;++i) {
                check_sum += sin(v[i]);
        }
        end = omp_get_wtime();
        cout << "series time: " << (int)(end - start) << " s, n = " << n << ", th =" << th << endl;
        start = omp_get_wtime();
        double cur_sum = 0;
        #pragma omp parallel for reduction (+: cur_sum) num_threads(th)
        for(int i = 0;i < n;++i) {
                cur_sum += sin(v[i]);
        }
        #pragma end parallel
        assert(abs(cur_sum - check_sum) < 1e-18);
        end = omp_get_wtime();
        cout << "OpenMP time: " << (int)(end - start) << " s, n = " << n << ", th =" << th << endl
;
        return 0;
}

