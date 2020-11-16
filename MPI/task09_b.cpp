#include <stdio.h>
#include <mpi.h>

#define N 100000

int main (int argc, char* argv[]) {

    double starttime, endtime;
    int processId, size;

    MPI_Request request1, request2;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {

        int* n = new int[N];

        for (int i = 0; i < N; i++) n[i] = i;

        starttime = MPI_Wtime();

        MPI_Isend(n, N, MPI_INT, 1, 0, MPI_COMM_WORLD, &request1);
        MPI_Irecv(n, N, MPI_INT, 0, 1, MPI_COMM_WORLD, &request2);

        endtime = MPI_Wtime();

        printf("Time for proc id:%i = %f seconds\n", processId, endtime - starttime);

    }

    if (processId == 1) {

        int* n = new int[N];

        for (int i = 0; i < N; i++) n[i] = i;

        starttime = MPI_Wtime();

        MPI_Isend(n, N, MPI_INT, 0, 1, MPI_COMM_WORLD, &request2);
        MPI_Irecv(n, N, MPI_INT, 1, 0, MPI_COMM_WORLD, &request1);

        endtime = MPI_Wtime();

        printf("Time for proc id:%i = %f seconds\n", processId, endtime - starttime);

    }

    MPI_Finalize();

    return 0;
}