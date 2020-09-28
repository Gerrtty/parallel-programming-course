#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    int num_threads = 3;

#pragma omp parallel num_threads(num_threads) if (num_threads > 1)
    {
        printf("Hello from area one, thread num = %d, common num of threads = %d\n",
               omp_get_thread_num(), omp_get_num_threads());
    }

    num_threads = 1;

#pragma omp parallel num_threads(num_threads) if (num_threads > 1)
    {
        printf("Hello from area two, thread num = %d, common num of threads = %d\n",
               omp_get_thread_num(), omp_get_num_threads());
    }

    return 0;

}