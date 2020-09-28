#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include <random>

using namespace std;

void printMatrix(int** matrix, const int lines, const int columns) {
    printf("-----------\n");
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%i ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("-----------\n");
}

int** getMatrix(const int lines, const int columns) {
    int** matrix = new int * [lines];
    for (int i = 0; i < lines; i++) {
        matrix[i] = new int [columns];
    }
    return matrix;
}

void fill(int **matrix, int n, int m) {

    random_device r;
    default_random_engine generator(r());
    uniform_int_distribution<int> gen(0, 10);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            matrix[i][j] = gen(generator);
        }
    }

}

int main(int argc, char *argv[]) {
    int n = 6; int m = 8;

    int i, j;

    int **d = getMatrix(n, m);
    fill(d, 6, 8);

    float mean = 0; int count = 0; float sum;

    int min = d[0][0];
    int max = d[0][0];

    #pragma omp parallel private(i, j, sum) shared(d, mean, min, max, count)
    {
        #pragma omp sections
        {
            #pragma omp section

            {
                sum = 0;

                for (i = 0; i < n; i++) {
                    for (j = 0; j < m; j++) {
                        sum += d[i][j];
                    }
                }

                mean = sum / (6 * 8);
                printf("Номер потока = %i\n", omp_get_thread_num());
                printf("Mean = %f\n\n", mean);
            }

            #pragma omp section
            {

                for (i = 0; i < n; i++) {
                    for (j = 0; j < m; j++) {
                        int elem = d[i][j];
                        if (elem < min) {
                            min = elem;
                        }
                        if (elem > max) {
                            max = elem;
                        }
                    }
                }
                printf("Номер потока = %i\n", omp_get_thread_num());
                printf("Min = %i\n", min);
                printf("Max = %i\n\n", max);
            }

            #pragma omp section
            {

                for (i = 0; i < n; i++) {
                    for (j = 0; j < m; j++) {
                        int elem = d[i][j];
                        if (elem % 3 == 0) {
                            count++;
                        }
                    }
                }

                printf("Номер потока = %i\n", omp_get_thread_num());
                printf("Count = %i\n\n", count);
            }
        }
    }

    printf("After parallel\n");
    printf("Mean = %f\n", mean);
    printf("Min = %i\n", min);
    printf("Max = %i\n", max);
    printf("Count = %i\n", count);

    return 0;

}