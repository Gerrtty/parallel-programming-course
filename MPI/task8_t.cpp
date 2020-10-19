#include <stdio.h>
#include <mpi.h>
#include <random>

#define M 6
#define N 5

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


void fillFinalMatrix(int** finalMatrix, int* finalArr, int m, int n) {
    int c = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            finalMatrix[i][j] = finalArr[c++];
        }
    }
}


void fillTMatirix(int** Tmat, int** part, int dest, int block)  {

    int s = (dest - 1) * block;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < block; j++) {
            Tmat[i][j + s] = part[i][j];
        }
    }

//    for (int j = 0; j < block; j++) {
//        Tmat[(i - 1) * block + j] = buffArr[j];
//    }
}


int main(int argc, char* argv[]) {

    int processId, processCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {

        int** A = alloc_2d_int(M, N);
        fill(A, M, N);

        int** At = alloc_2d_int(N, M);

        printMatrix(A, M, N);

        int blockSize = M / (processCount - 1);

        if ((float) M / (processCount - 1) - M / (processCount - 1) > 0) {
            blockSize += 1;
        }

        printf("block size = %i, process count = %i\n", blockSize, processCount);

        int reminder_size = M;
        int c = 0;

        for (int dest = 1; dest < processCount; dest++) {

            int size_arr = reminder_size - blockSize >= 0 ? blockSize : reminder_size;

            if (size_arr < 0) {
                size_arr = 0;
            }

            reminder_size -= blockSize;

            int** sendedArr = alloc_2d_int(size_arr, N);

            for (int i = 0; i < size_arr; i++) {
                for (int j = 0; j < N; j++) {
                    sendedArr[i][j] = A[blockSize * (dest - 1) + i][j];
                }
            }

//            printMatrix(sendedArr, size_arr, N);

            MPI_Send(&(sendedArr[0][0]), size_arr * N, MPI_INT, dest, dest, MPI_COMM_WORLD);

            int count;
            MPI_Status status;
            MPI_Probe(dest, 100 + dest, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);

            int** Tpart = alloc_2d_int(N, count / N);

            MPI_Recv(&(Tpart[0][0]), count, MPI_INT, dest, 100 + dest, MPI_COMM_WORLD, &status);

//            printMatrix(Tpart, N, count / N);

            fillTMatirix(At, Tpart, dest, count / N);

        }

        printMatrix(At, N, M);

    }

    if (processId != 0) {

        int count;
        MPI_Status status;
        MPI_Probe(0, processId, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int arr_size = count / N;

        printf("count = %i\n", count);

        int** buffA = alloc_2d_int(arr_size, N);
        MPI_Recv(&(buffA[0][0]), count, MPI_INT, 0, processId, MPI_COMM_WORLD, &status);

//        printMatrix(buffA, arr_size, N);

        int** buffAT = alloc_2d_int(N, arr_size);

        for (int i = 0; i < arr_size; i++) {
            for (int j = 0; j < N; j++) {
                buffAT[j][i] = buffA[i][j];
            }
        }

//        printMatrix(buffAT, N, arr_size);

        MPI_Send(&(buffAT[0][0]), arr_size * N, MPI_INT, 0, 100 + processId, MPI_COMM_WORLD);

    }


    MPI_Finalize();

    return 0;
}