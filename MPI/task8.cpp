#include <stdio.h>
#include <mpi.h>
#include <random>

#define VECTOR_SIZE 12

#define M 3
#define N 4

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

    outStr += "]\n";

    printf("%s", outStr.c_str());
}

void printMatrix(int** matrix, const int lines, const int columns) {

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

int** getMatrix(const int lines, const int columns) {
    int** matrix = new int * [lines];
    for (int i = 0; i < lines; i++) {
        matrix[i] = new int [columns];
    }
    return matrix;
}

void fill(int** matrix, const int lines, const int columns) {

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

int **alloc_2d_int(int rows, int cols) {
    int* data = (int*) malloc(rows * cols * sizeof(int));
    int** array = (int**) malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        array[i] = &(data[cols * i]);
    }

    return array;
}


int main(int argc, char* argv[]) {

    int tagA = 100;

    int processId, processCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {
        int** A = alloc_2d_int(3, 4);
        fill(A, M, N);

        printMatrix(A, M, N);

        MPI_Send(&(A[0][0]), M * N, MPI_INT, 1, tagA, MPI_COMM_WORLD);

    }

    if (processId != 0) {
        int count;
        MPI_Status status;
        MPI_Probe(0, tagA, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        printf("count = %i\n", count);

        int** A = alloc_2d_int(M, N);

        MPI_Recv(&(A[0][0]), M * N, MPI_INT, 0, tagA, MPI_COMM_WORLD, &status);

        printMatrix(A, M, N);

    }

    MPI_Finalize();

    return 0;
}