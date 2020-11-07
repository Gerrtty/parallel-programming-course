#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>

#include <mpi.h>

#define M 8   // rows
#define N 2  // columns

#define ELEMENTS_PER_PROCESS 2 // count of rows per process

using namespace std;

int main(int argc, char* argv[]) {

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int matrix[M][N];
    int vector[M];

    if (processId == 0) {
        int c = 1;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = c++;
            }
        }
        c = 1;
        for (int i = 0; i < N; i++) {
            vector[i] = c++;
        }
    }

    int subMatrix[ELEMENTS_PER_PROCESS][N];
    int subVector[ELEMENTS_PER_PROCESS];

    MPI_Bcast(vector, N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatter(matrix, ELEMENTS_PER_PROCESS * N, MPI_INT, &subMatrix,
                ELEMENTS_PER_PROCESS * N, MPI_INT, 0, MPI_COMM_WORLD);

    for (unsigned int i = 0; i < ELEMENTS_PER_PROCESS; i++) {
        int num = 0;
        for (unsigned int j = 0; j < N; j++) {
            num += subMatrix[i][j] * vector[j];
        }
        subVector[i] = num;
    }

    MPI_Gather(subVector, ELEMENTS_PER_PROCESS, MPI_INT, &vector, ELEMENTS_PER_PROCESS, MPI_INT, 0, MPI_COMM_WORLD);

    // print final matrix
    if (processId == 0) {
        for (int i = 0; i < M; i++) {
            printf("%i ", vector[i]);
        }
        printf("\n");
    }

    MPI_Finalize();

    return 0;
}