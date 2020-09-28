#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <iostream>
#include <string>
#include <ctime>
#include <chrono>

#define SIZE 10000

using namespace std;

void print(int** matrix, const int lines, const int columns) {
    printf("-----------\n");
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%i ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("-----------\n");
}

void print(int* vec, const int n) {
    printf("-----------\n");
    for (int i = 0; i < n; i++) {
        printf("%i ", vec[i]);
    }
    printf("\n-----------\n");
}

int** getMatrix(const int lines, const int columns) {
    int** matrix = new int * [lines];
    for (int i = 0; i < lines; i++) {
        matrix[i] = new int [columns];
    }
    return matrix;
}

int* getVector(const int n) {
    int* v = new int[n];
    return v;
}

void fill(int *vec, int n) {
    for (int i = 0; i < n; i++) {
        vec[i] = i+1;
    }
}

void fill(int **matrix, int n, int m) {

    random_device r;
    default_random_engine generator(r());
    uniform_int_distribution<int> gen(0, 10);

    int c = 1;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            matrix[i][j] = gen(generator);
            matrix[i][j] = i * j;
            matrix[i][j] = c++;
        }
    }

}

auto consecutive(int **mat, int* vec) {
    int* resultVector = getVector(SIZE);

    auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < SIZE; i++) {
        int* s = mat[i];
        int res = 0;
        for (int j = 0; j < SIZE; j++) {
            res += s[j] * vec[j];
        }
        resultVector[i] = res;
    }

    auto end_time = std::chrono::steady_clock::now();

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    return elapsed_ms.count();
}

auto parallel(int **mat, int* vec) {

    int* resultVector = getVector(SIZE);
    auto start_time = std::chrono::steady_clock::now();

    int res; int i, j;

    #pragma omp parallel for shared(mat, vec) schedule(static, SIZE / 100) private(i, j)
    for (i = 0; i < SIZE; i++) {
        int* s = mat[i];
        res = 0;
//        #pragma omp parallel for shared(mat, vec) reduction(+:res) schedule(static, SIZE / 100) private(i, j)
        for (j = 0; j < SIZE; j++) {
            res += s[j] * vec[j];
        }
        resultVector[i] = res;
    }

    auto end_time = std::chrono::steady_clock::now();

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    return elapsed_ms.count();

}

int main(int argc, char *argv[]) {

    int** mat = getMatrix(SIZE, SIZE);
    int* vec = getVector(SIZE);
    fill(mat, SIZE, SIZE);
    fill(vec, SIZE);

    int consTime = consecutive(mat, vec);
    int parallelTime = parallel(mat, vec);

    printf("For SIZE = %i time for consecutive calculation = %i and for parallel calculation = %i\n", SIZE, consTime, parallelTime);
}