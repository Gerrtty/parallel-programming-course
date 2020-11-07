#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>

#include <mpi.h>

#define SIZE 8000
#define ELEMENTS_PER_PROCESS 2000

using namespace std;

void fill(int* arr) {
    for (unsigned int i = 0; i < SIZE; i++) {
        arr[i] = i;
        if (i % 2 == 0) {
            arr[i] *= -1;
        }
    }
}

int arrAbs(const int* arr, const unsigned int arrLength) {
    int sum = 0;
    for (unsigned int i = 0; i < arrLength; i++) {
        sum += abs(arr[i]);
    }
    return sum;
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

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    int* arr;

    if (processId == 0) {
        arr = new int[SIZE];
        fill(arr);
    }

    int* sub_array = new int[ELEMENTS_PER_PROCESS];

    MPI_Scatter(arr, ELEMENTS_PER_PROCESS, MPI_INT, sub_array,
                ELEMENTS_PER_PROCESS, MPI_INT, 0, MPI_COMM_WORLD);

    int localAbsSum = arrAbs(sub_array, ELEMENTS_PER_PROCESS);

    int globalAbsSum;
    MPI_Reduce(&localAbsSum, &globalAbsSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (processId == 0) {
        printf("Abs sum = %i\n", globalAbsSum);
    }

    MPI_Finalize();

    return 0;
}