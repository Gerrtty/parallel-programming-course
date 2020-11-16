#include <stdio.h>
#include <mpi.h>
#include <random>

#define M 6
#define N 6

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


int main(int argc, char* argv[]) {

    int tagA = 100;

    int processId, processCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {

        // init matrix
        int** A = alloc_2d_int(M, N);
        int** B = alloc_2d_int(M, N);
        fill(A, M, N);
        fill(B, M, N);

        printMatrix(A, M, N);
        printMatrix(B, M, N);

        int matrixSize = M * N;

        int blockSize = matrixSize / (processCount - 1);

        if ((float) matrixSize / (processCount - 1) - matrixSize / (processCount - 1) > 0) {
            blockSize += 1;
        }

        printf("block size = %i\n", blockSize);

        int reminder_size = matrixSize;

        int* finalArr = new int[matrixSize];
        int** finalMatrix = alloc_2d_int(M, N);


        int c = 0; int i = 0; int j = 0;

        for (int dest = 1; dest < processCount; dest++) {

            int size_arr = reminder_size - blockSize >= 0 ? blockSize : reminder_size;

            if (size_arr < 0) {
                size_arr = 0;
            }

            reminder_size -= blockSize;

            printf("size arr = %i\n", size_arr);

            auto a = &(A[0][0]);
            auto b = &(B[0][0]);

            MPI_Send(&a[(dest - 1) * size_arr], size_arr, MPI_INT, dest, dest + 100, MPI_COMM_WORLD);
            MPI_Send(&b[(dest - 1) * size_arr], size_arr, MPI_INT, dest, dest + 200, MPI_COMM_WORLD);

            int count;
            MPI_Status status;
            MPI_Probe(dest, dest + 1000, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);

            int* recvC = new int[count];

            MPI_Recv(recvC, count, MPI_INT, dest, dest + 1000, MPI_COMM_WORLD, &status);

//            printArr(recvC, count, "Final arr " + to_string(dest));

            fillFinalArr(finalArr, recvC, blockSize, dest);

        }

        fillFinalMatrix(finalMatrix, finalArr, N, M);

//        printArr(finalArr, M*N, "final");

        printMatrix(finalMatrix, M, N);

//        printMatrix(finalArr, M, N);

//        MPI_Send(&(A[0][0]), M * N, MPI_INT, 1, tagA, MPI_COMM_WORLD);

    }

    if (processId != 0) {
        int count;
        MPI_Status status;
        MPI_Probe(0, processId + 100, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

//        printf("count = %i\n", count);

//        int** A = alloc_2d_int(M, N);
//
//        MPI_Recv(&(A[0][0]), M * N, MPI_INT, 0, tagA, MPI_COMM_WORLD, &status);
//
//        printMatrix(A, M, N);

        int* partA = new int[count];
        int* partB = new int[count];

        int* partC = new int[count];

        MPI_Recv(partA, count, MPI_INT, 0, processId + 100, MPI_COMM_WORLD, &status);
        MPI_Recv(partB, count, MPI_INT, 0, processId + 200, MPI_COMM_WORLD, &status);

//        printArr(partA, count, "Part of A on process = " + to_string(processId));
//        printArr(partB, count, "Part of B on process = " + to_string(processId));

        for (int i = 0; i < count; i++) {
            partC[i] = partA[i] * partB[i];
        }

//        printArr(partC, count, "Part of C on process = " + to_string(processId));

        MPI_Send(partC, count, MPI_INT, 0, 1000 + processId, MPI_COMM_WORLD);

    }

    MPI_Finalize();

    return 0;
}