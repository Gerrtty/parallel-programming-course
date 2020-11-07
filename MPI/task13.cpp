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

    int A[M][N];
    int B[M][N];
    int C[M][N];

    if (processId == 0) {
        int c = 0;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = c;
                B[i][j] = c++;
            }
        }
    }

    int subMatrixA[ELEMENTS_PER_PROCESS][N];
    int subMatrixB[ELEMENTS_PER_PROCESS][N];
    int subMatrixC[ELEMENTS_PER_PROCESS][N];

    MPI_Scatter(A, ELEMENTS_PER_PROCESS * N, MPI_INT, &subMatrixA,
                ELEMENTS_PER_PROCESS * N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatter(B, ELEMENTS_PER_PROCESS * N, MPI_INT, &subMatrixB,
                ELEMENTS_PER_PROCESS * N, MPI_INT, 0, MPI_COMM_WORLD);

    // C of sub matrix
    for (unsigned int i = 0; i < ELEMENTS_PER_PROCESS; i++) {
        for (unsigned int j = 0; j < N; j++) {
            subMatrixC[i][j] = subMatrixA[i][j] * subMatrixB[i][j];
        }
    }

    MPI_Gather(subMatrixC, ELEMENTS_PER_PROCESS * N, MPI_INT,
               &C, ELEMENTS_PER_PROCESS * N, MPI_INT, 0, MPI_COMM_WORLD);

    // print final matrix
    if (processId == 0) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                printf("%i ", C[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();

    return 0;
}