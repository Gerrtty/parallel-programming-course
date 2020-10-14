#include <stdio.h>
#include <mpi.h>
#include <random>

#define VECTOR_SIZE 12

using namespace std;

void printArr(const int* arr, const int arrSize, string arrName) {
    printf("Arr %s = [", arrName.c_str());
    for (int i = 0; i < arrSize; i++) {
        if (i < arrSize - 1) {
            printf("%i ", arr[i]);
        } else {
            printf("%i", arr[i]);
        }
    }
    printf("]\n");
}

int* initArr(const int arrSize) {

    int* arr = new int[arrSize];

    for (int i = 0; i < arrSize; i++) {
        arr[i] = rand() % 10;
        arr[i] = i;
    }

    return arr;
}

void fillFinalArr(int* finalArr, const int* buffArr, const int blockSize, const int i) {
    for (int j = 0; j < blockSize; j++) {
        finalArr[(i - 1) * blockSize + j] = buffArr[j];
    }
}


int main(int argc, char* argv[]) {

    int processId, processCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int* res = new int[VECTOR_SIZE];

    if (processId == 0) {

        int* z = new int[VECTOR_SIZE];

        int* x = initArr(VECTOR_SIZE);

        printArr(x, VECTOR_SIZE, "x");

        // equal blocks
        int blockSize = VECTOR_SIZE / (processCount - 1);

        printf("block size = %i, process count = %i\n", blockSize, processCount);

        int reminder_size = VECTOR_SIZE;

        for (int dest = 1; dest < processCount; dest++) {

            int size_arr = reminder_size - blockSize >= 0 ? blockSize : reminder_size;

            if (size_arr < 0) {
                size_arr = 0;
            }

            reminder_size -= blockSize;

            MPI_Send(&x[(dest - 1) * size_arr], size_arr, MPI_INT, dest, 1234, MPI_COMM_WORLD);

            printf("P0 sent a %d elements to P%d.\n", size_arr, dest);

            int count;
            MPI_Status status;
            MPI_Probe(dest, dest, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);

            int* buf_z = new int[count];

            MPI_Recv(buf_z, count, MPI_INT, dest, dest, MPI_COMM_WORLD, &status);

            fillFinalArr(z, buf_z, blockSize, dest);
        }

        printArr(z, VECTOR_SIZE, "res");

    }

    if (processId != 0) {

        int count;
        MPI_Status status;
        MPI_Probe(0, 1234, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int *localArray = (int *) malloc(count * sizeof(int));

        MPI_Recv(localArray, count, MPI_INT, 0, 1234, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < count; i++) {
            localArray[i] *= 2;
        }

        string s = "on process " + to_string(processId);

        printArr(localArray, count, s);

        MPI_Send(localArray, count, MPI_INT, 0, processId, MPI_COMM_WORLD);

    }

    MPI_Finalize();

    return 0;
}
