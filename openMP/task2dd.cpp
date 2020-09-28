#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
#pragma omp parallel num_threads(3)
    {
        if (omp_get_num_threads() > 1) {
            printf("Hello from area one, thread num = id, common num of threads = %i\n",
                   omp_get_thread_num(), omp_get_num_threads());
        }
    }

#pragma omp parallel num_threads(1)
    {
        if (omp_get_num_threads() > 1) {
            printf("Hello from area two, thread num = %i, common num of threads = %i\n",
                   omp_get_thread_num(), omp_get_num_threads());
        }
    }
}