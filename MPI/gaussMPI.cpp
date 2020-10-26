#include <stdio.h>
#include <mpi.h>
#include <random>

#define M 3
#define N 3

using namespace std;

int* initArr(const int arrSize) {

    int* arr = new int[arrSize];

    for (int i = 0; i < arrSize; i++) {
        arr[i] = i;
    }

    return arr;
}

void printArr(const int* arr, const int arrSize, string arrName) {

    string outStr = "Arr " + arrName + " = [";

    for (int i = 0; i < arrSize; i++) {
        outStr += to_string(arr[i]);
        if (i < arrSize - 1) {
            outStr += " ";
        }
    }

    outStr += "]\n\n";

    printf("%s", outStr.c_str());
}

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

void fillFinalArr(int* finalArr, const int* buffArr, const int blockSize, const int i) {
    for (int j = 0; j < blockSize; j++) {
        finalArr[(i - 1) * blockSize + j] = buffArr[j];
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

int main(int argc, char* argv[]) {

    int processId, processCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    float** A = alloc_2d_float(M, N);
    fill(A, M, N);

    for (int i = 0; i < M; i++) {
        int imax = getIndexOfMaxElemInColumn(A, N, i);
        if (i != imax) {
            swap(A, i, imax);
        }

        if (processId == 0) {
            for (int dest = 1; dest < processCount; dest++) {

                float** sentArr = alloc_2d_float(N - i - 1, N - i);
                printf("N-i-1 = %i\n", N - i - 1);

                int u = 0;
                for (int j = i + 1; j < N; j++) {
                    int uu = 0;
                    for (int k = i; k < N; k++) {
                        printf("u = %i\n", u);
                        sentArr[u++][uu++] = A[i][k];
                    }
                }
//                MPI_Status status;
//                MPI_Send(&(sentArr[0][0]), (N - i - 1) * (N - i), MPI_FLOAT, dest, dest, MPI_COMM_WORLD);
//                MPI_Recv(&(sentArr[0][0]), (N - i - 1) * (N - i), MPI_FLOAT, dest, 100, MPI_COMM_WORLD, &status);
            }

        }

//        if (processId != 0) {
//
//            int count;
//            MPI_Status status;
//            MPI_Probe(0, processId , MPI_COMM_WORLD, &status);
//            MPI_Get_count(&status, MPI_INT, &count);
//
//            float** recvArr = alloc_2d_float(N - i - 1, N - i);
//            MPI_Recv(&(recvArr[0][0]), count, MPI_FLOAT, 0, processId, MPI_COMM_WORLD, &status);
//
//            for (int j = 0; j < N - i - 1; j++) {
//                float mul = -recvArr[j][i] / recvArr[i][i];
//                for (int k = 0; k < N - i; k++) {
//                    recvArr[j][k] += recvArr[i][k] * mul;
//                }
//            }
//
//            MPI_Send(&(recvArr[0][0]), count, MPI_FLOAT, 0, 100, MPI_COMM_WORLD);
//
//        }

    }

    MPI_Finalize();

    return 0;
}
