#include <stdio.h>
#include <mpi.h>
#include <random>

using namespace std;

int main (int argc, char* argv[]) {
    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {
        int a[10];
        for (int i = 0; i < 10; i++) {
            a[i] = rand() % 10;
        }
        MPI_Send(a, 10, MPI_INT, 1, 0, MPI_COMM_WORLD );
    }

    if (processId == 1) {
        int buf[10];
        MPI_Status status;
        MPI_Recv(buf, 10, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        for (int i = 0; i < 10; i++) {
            printf("%i\n", buf[i]);
        }
    }

    MPI_Finalize();

    return 0;
}

//         int s = MPI_Probe(0, 0, MPI_COMM_WORLD, &status);