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

    int n = 5;

    float** A = alloc_2d_float(n, n);
    fill(A, n, n);

    for (int i = 0; i < n; i++) {
        int imax = getIndexOfMaxElemInColumn(A, n, i);
        if (i != imax) {
            swap(A, i, imax);
        }

        float Aii = A[i][i];

        if (Aii == 0) {
            break;
        }

        float* Aji = new float[n - i - 1];
        for (int j = 0; j < n - i - 1; j++) {
            Aji[j] = A[j + i + 1][i];
        }

        float* muls = new float[n - i - 1];
        for (int j = 0; j < n - i - 1; j++) {
            muls[j] = -Aji[j] / Aii;
        }

        float** sub1 = alloc_2d_float(n - i - 1, n - i);
        float** sub2 = alloc_2d_float(n - i - 1, n - i);

        for (int j = 0; j < n - i - 1; j++) {
            for (int k = 0; k < n - i; k++) {
                sub1[j][k] = A[j + i + 1][k + i];
                sub2[j][k] = A[i][k + i];
            }
        }

        for (int j = 0; j < n - i - 1; j++) {
            for (int k = 0; k < n - i; k++) {
                sub1[j][k] += sub2[j][k] * muls[j];
            }
        }

        for (int j = 0; j < n - i - 1; j++) {
            for (int k = 0; k < n - i; k++) {
                A[j + i + 1][k + i] = sub1[j][k];
            }
        }

//        for (int j = i + 1; j < n; j++) {
//            float mul = -A[j][i] / Aii;
//            for (int k = i; k < n; k++) {
//                A[j][k] += A[i][k] * mul;
//            }
//        }

    }

    printMatrix(A, n, n);

    return 0;
}