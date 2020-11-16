#include <stdio.h>
#include <mpi.h>
#include <random>

#define VECTOR_SIZE 12

using namespace std;

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

int* initArr(const int arrSize) {

    int* arr = new int[arrSize];

    for (int i = 0; i < arrSize; i++) {
//        arr[i] = rand() % 10;
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

    srand(time(NULL));

    int a = 2;
    int b = 3;

    // by default
    string mode = "mult";

    if (argc > 1) {
        mode = argv[1];
    }

    int processId, processCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {
        int* x = initArr(VECTOR_SIZE);
        int* y = initArr(VECTOR_SIZE);

        printArr(x, VECTOR_SIZE, "original x");
        printArr(y, VECTOR_SIZE, "original y");

        // equal blocks
        int blockSize = VECTOR_SIZE / (processCount - 1);

        if ((float) VECTOR_SIZE / (processCount - 1) - VECTOR_SIZE / (processCount - 1) > 0) {
            blockSize += 1;
        }

        if (processCount >= VECTOR_SIZE) {
            blockSize = 1;
        }

        printf("block size = %i, process count = %i\n", blockSize, processCount);

        int reminder_size = VECTOR_SIZE;

        int c = 0;

        // split by blocks
        for (int dest = 1; dest < processCount; dest++) {

            int size_arr = reminder_size - blockSize >= 0 ? blockSize : reminder_size;

            if (size_arr < 0) {
                size_arr = 0;
            }

            reminder_size -= blockSize;

            MPI_Send(&x[(dest - 1) * size_arr], size_arr, MPI_INT, dest, 100, MPI_COMM_WORLD);
            MPI_Send(&y[(dest - 1) * size_arr], size_arr, MPI_INT, dest, 111, MPI_COMM_WORLD);
        }

        if (mode != "swap") {

            int* z = new int[VECTOR_SIZE];

            for (int i = 1; i < processCount; i++) {
                int count;
                MPI_Status status;
                MPI_Probe(i, i, MPI_COMM_WORLD, &status);
                MPI_Get_count(&status, MPI_INT, &count);

                int* buf_z = new int[count];

                MPI_Recv(buf_z, count, MPI_INT, i, i, MPI_COMM_WORLD, &status);

                fillFinalArr(z, buf_z, blockSize, i);

            }
            printArr(z, VECTOR_SIZE, "final z");
        }

        else {

            int* x = new int[VECTOR_SIZE];
            int* y = new int[VECTOR_SIZE];

            for (int i = 1; i < processCount; i++) {
                int count;
                MPI_Status status;
                MPI_Probe(i, i * 10, MPI_COMM_WORLD, &status);
                MPI_Get_count(&status, MPI_INT, &count);

                int* buf_x = new int[count];
                int* buf_y = new int[count];

                MPI_Recv(buf_x, count, MPI_INT, i, i * 10, MPI_COMM_WORLD, &status);
                MPI_Recv(buf_y, count, MPI_INT, i, i * 20, MPI_COMM_WORLD, &status);

                fillFinalArr(x, buf_x, blockSize, i);
                fillFinalArr(y, buf_y, blockSize, i);
            }

            printArr(x, VECTOR_SIZE, "final x");
            printArr(y, VECTOR_SIZE, "final y");

        }
    }

    if (processId != 0) {
        int count;
        MPI_Status status;
        MPI_Probe(0, 100, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int buf_x[count];
        int buf_y[count];

        MPI_Recv(buf_x, count, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
        MPI_Recv(buf_y, count, MPI_INT, 0, 111, MPI_COMM_WORLD, &status);

        if (mode != "swap") {
            int result_arr[count];

            for (int i = 0; i < count; i++) {

                if (mode == "sum") {
                    result_arr[i] = buf_x[i] * a + buf_y[i] * b;
                }
                else if (mode == "mult") {
                    result_arr[i] = buf_x[i] * buf_y[i];
                }
            }

            printArr(result_arr, count, "z on process " + to_string(processId));

            MPI_Send(result_arr, count, MPI_INT, 0, processId, MPI_COMM_WORLD);
        }

        else {

            for (int i = 0; i < count; i++) {
                int savedX = buf_x[i];
                buf_x[i] = buf_y[i];
                buf_y[i] = savedX;
            }

            printArr(buf_x, count, "x on process " + to_string(processId));
            printArr(buf_y, count, "y on process " + to_string(processId));

            MPI_Send(buf_x, count, MPI_INT, 0, processId * 10, MPI_COMM_WORLD);
            MPI_Send(buf_y, count, MPI_INT, 0, processId * 20, MPI_COMM_WORLD);

        }
    }

    MPI_Finalize();

    return 0;
}