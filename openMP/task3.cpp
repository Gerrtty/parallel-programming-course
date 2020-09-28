#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    int a = 10;
    int b = 10;

    printf("a before parallel = %i, b before parallel = %i\n", a, b);

    #pragma omp parallel num_threads(2) private(a) firstprivate(b)
    {
        int thread_num = omp_get_thread_num();
        a += thread_num;
        b += thread_num;
        printf("a = %i, b = %i\n", a, b);
    }
    printf("a after parallel = %i, b after parallel = %i\n", a, b);

    printf("\na before parallel = %i, b before parallel = %i\n", a, b);
    #pragma omp parallel num_threads(4) shared(a) firstprivate(b)
    {
        int thread_num = omp_get_thread_num();
        a -= thread_num;
        b -= thread_num;
        printf("a = %i, b = %i, thread_num = %i\n", a, b, thread_num);
    }
    printf("a after parallel = %i, b after parallel = %i\n", a, b);

    return 0;
}