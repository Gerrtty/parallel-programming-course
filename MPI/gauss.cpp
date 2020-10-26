#include <stdio.h>
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

void printMatrix(float** matrix, const int lines, const int columns) {

    string outStr = "-----------\n";
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            outStr += to_string(matrix[i][j]) + " ";
            if (matrix[i][j] < 10) {
                outStr += " ";
            }
        }
        outStr += "\n";
    }
    outStr += "-----------\n";

    printf("%s", outStr.c_str());
}

void fill(float** matrix, const int lines, const int columns) {

    int c = 0;
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            matrix[i][j] = c++;
        }
    }

}

float **alloc_2d_float(int rows, int cols) {
    float* data = (float*) malloc(rows * cols * sizeof(float));
    float** array = (float**) malloc(rows * sizeof(float*));
    for (int i = 0; i < rows; i++) {
        array[i] = &(data[cols * i]);
    }

    return array;
}

int getIndexOfMaxElemInColumn(float** matrix, int n, int col) {

    float max = matrix[col][col];
    int maxPos = col;
    for (int i = col + 1; i < n; ++i) {
        float element = matrix[i][col];
        if (element > max) {
            max = element;
            maxPos = i;
        }
    }
    return maxPos;

}

void swap(float** matrix, int i, int j) {

    float* temp = matrix[i];
    matrix[i] = matrix[j];
    matrix[j] = temp;

}

int main (int argc, char* argv[]) {

    float** A = alloc_2d_float(3, 3);
    fill(A, 3, 3);
    printMatrix(A, 3, 3);

    int n = 3;

    for (int i = 0; i < n; i++) {
        int imax = getIndexOfMaxElemInColumn(A, n, i);
        if (i != imax) {
            swap(A, i, imax);
        }

        for (int j = i + 1; j < n; j++) {
            float mul = -A[j][i] / A[i][i];
            for (int k = i; k < n; k++) {
                A[j][k] += A[i][k] * mul;
            }
        }
    }

    printMatrix(A, 3, 3);

    return 0;
}