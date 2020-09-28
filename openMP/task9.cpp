#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <iostream>
#include <string>
#include <ctime>
#include <chrono>

#define N 6
#define M 8

using namespace std;

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


int main(int argv, char *argc[]) {

    int **matrix = getMatrix(N, M);

    fill(matrix, N, M);

    int min, max, i, j;

    min = matrix[0][0];
    max = matrix[0][0];

    #pragma omp parallel for schedule(static, N) private(i, j) shared(matrix, min, max)
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            if (matrix[i][j] < min) {
                #pragma omp critical
                if (matrix[i][j] < min) {
                    min = matrix[i][j];
                }
            }
            if (matrix[i][j] > max) {
                #pragma omp critical
                if (matrix[i][j] > max) {
                    max = matrix[i][j];
                }
            }
        }
    }

    printf("Min = %i, max = %i\n", min, max);

    return 0;

}

