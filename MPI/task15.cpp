#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>

#include <mpi.h>

#define ARR_SIZE 20
#define ELEMENTS_PER_PROCESS 5
#define ROOT 0

using namespace std;

void fill(int* arr, const int size) {
    for (unsigned int i = 0; i < size; i++) {
        arr[i] = i;
    }
}

void print(const int* arr, const unsigned int arrSize, string arrName) {

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

int main(int argc, char* argv[]) {

    int processId, size;
    int arrSizePerProcess = int(ARR_SIZE / 4);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int* sendBuff = new int[arrSizePerProcess];
    fill(sendBuff, arrSizePerProcess);

    if (processId != ROOT) {
        MPI_Send(sendBuff, arrSizePerProcess, MPI_INT, 0, processId, MPI_COMM_WORLD);
    }

    if (processId == ROOT) {
        int finalArr[ARR_SIZE];

        // gather all subarrays in one array
        for (int i = 0; i < size; i++) {
            int* recvBuff = new int[arrSizePerProcess];
            if (i == 0) {
                recvBuff = sendBuff;
            }
            else {
                MPI_Status status;
                MPI_Recv(recvBuff, arrSizePerProcess, MPI_INT, i, i, MPI_COMM_WORLD, &status);
            }
            for (int j = 0; j < arrSizePerProcess; j++) {
                finalArr[i * arrSizePerProcess + j] = recvBuff[j];
            }
        }

        print(finalArr, ARR_SIZE, "Final gather array");
    }

    MPI_Finalize();

    return 0;
}