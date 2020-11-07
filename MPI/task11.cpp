#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>

#include <mpi.h>

#define SIZE 800
#define ELEMENTS_PER_PROCESS 200

using namespace std;

void fill(int* arr) {
    for (unsigned int i = 0; i < SIZE; i++) {
        arr[i] = i;
    }
}

int scalarProduct(const int* v1, const int* v2, const unsigned int arrLength) {
    int sum = 0;
    for (unsigned int i = 0; i < arrLength; i++) {
        sum += v1[i] * v2[i];
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
    int* v1; int* v2;

    if (processId == 0) {
        v1 = new int[SIZE];
        v2 = new int[SIZE];
        fill(v1);
        fill(v2);
    }

    int* subVector1 = new int[ELEMENTS_PER_PROCESS];
    int* subVector2 = new int[ELEMENTS_PER_PROCESS];

    MPI_Scatter(v1, ELEMENTS_PER_PROCESS, MPI_INT, subVector1,
                ELEMENTS_PER_PROCESS, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatter(v2, ELEMENTS_PER_PROCESS, MPI_INT, subVector2,
                ELEMENTS_PER_PROCESS, MPI_INT, 0, MPI_COMM_WORLD);

    int localScalarProduct = scalarProduct(subVector1, subVector2, ELEMENTS_PER_PROCESS);

    int globalScalarProduct;
    MPI_Reduce(&localScalarProduct, &globalScalarProduct, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (processId == 0) {
        printf("Scalar product = %i\n", globalScalarProduct);
    }

    MPI_Finalize();

    return 0;
}