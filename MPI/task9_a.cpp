#include <stdio.h>
#include <mpi.h>

#define N 100000

int main (int argc, char* argv[]) {

    double starttime, endtime;

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {

        int* n = new int[N];

        for (int i = 0; i < N; i++) n[i] = i;

        starttime = MPI_Wtime();
        MPI_Send(n, N, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Status status;
        MPI_Recv(n, N, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
        endtime = MPI_Wtime();

        printf("Ping pong time = %f seconds with n = %i\n", endtime - starttime, N);
    }

    if (processId == 1) {

        MPI_Status status;

        int buf[N];

        MPI_Recv(buf, N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Send(buf, N, MPI_INT, 0, 1, MPI_COMM_WORLD);

    }

    MPI_Finalize();

    return 0;
}