#include <stdio.h>
#include <mpi.h>
#include <random>

#define Ma 3
#define Na 3

#define Mb 3
#define Nb 3

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

int main(int argc, char* argv[]) {

    int tagA = 100;
    int tagB = 200;
    int tagResult = 300;

    int processId, processCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {

        int** A = alloc_2d_int(Ma, Na);
        int** B = alloc_2d_int(Mb, Nb);
        int** C = alloc_2d_int(Ma, Nb);

        fill(A, Ma, Na);
        fill(B, Mb, Nb);

        int blockSize = Ma / (processCount - 1);

        if ((float) Ma / (processCount - 1) - Ma / (processCount - 1) > 0) {
            blockSize += 1;
        }

        printf("block size = %i, process count = %i\n", blockSize, processCount);

        int reminder_size = Ma;
        int c = 0;

        for (int dest = 1; dest < processCount; dest++) {

            int size_arr = reminder_size - blockSize >= 0 ? blockSize : reminder_size;

            if (size_arr < 0) {
                size_arr = 0;
            }

            reminder_size -= blockSize;

            int** sentArr = alloc_2d_int(size_arr, Na);

            for (int i = 0; i < size_arr; i++) {
                for (int j = 0; j < Na; j++) {
                    sentArr[i][j] = A[blockSize * (dest - 1) + i][j];
                }
            }

            MPI_Send(&(sentArr[0][0]), size_arr * Na, MPI_INT, dest, dest + tagA, MPI_COMM_WORLD);
            MPI_Send(&(B[0][0]), Mb * Nb, MPI_INT, dest, dest + tagB, MPI_COMM_WORLD);

            int count;
            MPI_Status status;
            MPI_Probe(dest, tagResult + dest, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);
            int arr_size = count / Na;

            int** result = alloc_2d_int(arr_size, Nb);
            MPI_Recv(&(result[0][0]), count, MPI_INT, dest, tagResult + dest, MPI_COMM_WORLD, &status);

            for (int i = 0; i < arr_size; i++) {
                C[(dest - 1) * blockSize + i] = result[i];
            }

        }

        printf("result matrix: \n");
        printMatrix(C, Ma, Nb);

    }

    if (processId != 0) {

        int countA;
        MPI_Status status;
        MPI_Probe(0, processId + tagA, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &countA);

        int arr_size = countA / Na;

        printf("count = %i\n", countA);

        int** buffA = alloc_2d_int(arr_size, Na);
        int** B = alloc_2d_int(Mb, Nb);

        MPI_Recv(&(buffA[0][0]), countA, MPI_INT, 0, processId + tagA, MPI_COMM_WORLD, &status);
        MPI_Recv(&(B[0][0]), Mb * Nb, MPI_INT, 0, processId + tagB, MPI_COMM_WORLD, &status);

        int** result = alloc_2d_int(arr_size, Nb);

        for (int row = 0; row < arr_size; row++) {
            for (int col = 0; col < Nb; col++) {
                int c = 0;
                for (int i = 0; i < Na; i++) {
                    c += buffA[row][i] * B[i][col];
                }
                result[row][col] = c;
            }
        }

        printf("proc id = %i\n", processId);
        printMatrix(result, arr_size, Nb);

        MPI_Send(&(result[0][0]), arr_size * Nb, MPI_INT, 0, tagResult + processId, MPI_COMM_WORLD);

    }


    MPI_Finalize();

    return 0;
}
