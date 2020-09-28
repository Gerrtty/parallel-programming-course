#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]) {

    int i;

    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int b[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    float sumA = 0; float sumB = 0;
    float meanA = 0; float meanB = 0;

    #pragma omp parallel
    {
        #pragma omp for private(i) reduction(+: sumA, sumB)

            for (i = 0; i < 10; i++) {
                sumA += a[i];
                sumB += b[i];
            }

            meanA = sumA / 10;
            meanB = sumB / 10;
    }

    printf("mean a = %f\n", meanA);
    printf("mean b = %f\n", meanB);
    printf(a > b ? "a > b\n" : "b > a\n");

    return 0;
}