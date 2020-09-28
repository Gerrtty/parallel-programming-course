#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define len 10

int main(int argv, char *argc[]) {

    int a[] = {2, 2, 5, 7, 3, 1, 4, 5, 3, 2};
    int b[] = {2, 2, 5, 7, 3, 1, 4, 5, 3, 2};

    int min = a[0];
    int max = b[0];

    #pragma omp parallel num_threads(2)
    {

        if (omp_get_thread_num() == 0) {
            for (int i = 1; i < len; i++) {
                if (a[i] < min) {
                    min = a[i];
                }
            }
        }

        if (omp_get_thread_num() == 1) {
            for (int i = 1; i < len; i++) {
                if (b[i] > max) {
                    max = b[i];
                }
            }
        }

    }

    printf("max = %d\n", max);
    printf("min = %d\n", min);
}
