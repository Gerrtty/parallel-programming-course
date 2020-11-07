#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>

#include <mpi.h>

#define M 8   // rows
#define N 10  // columns

#define ELEMENTS_PER_PROCESS 2 // count of rows per process

using namespace std;

int main(int argc, char* argv[]) {

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int matrix[M][N];

    if (processId == 0) {
        int c = 0;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = c++;
            }
        }
    }

    int subMatrix[ELEMENTS_PER_PROCESS][N];

    MPI_Scatter(matrix, ELEMENTS_PER_PROCESS * N, MPI_INT, &subMatrix,
                ELEMENTS_PER_PROCESS * N, MPI_INT, 0, MPI_COMM_WORLD);

    // norm of sub matrix
    int localMax = subMatrix[0][0];
    for (unsigned int i = 0; i < ELEMENTS_PER_PROCESS; i++) {
        int sum = 0;
        for (unsigned int j = 0; j < N; j++) {
            sum += abs(subMatrix[i][j]);
        }
        if (sum > localMax) {
            localMax = sum;
        }
    }

    int globalMax;
    MPI_Reduce(&localMax, &globalMax, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (processId == 0) {
        printf("Norm of matrix = %i\n", globalMax);
    }

    MPI_Finalize();

    return 0;
}