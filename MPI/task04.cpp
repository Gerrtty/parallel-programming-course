#include <stdio.h>
#include <mpi.h>
#include <random>

using namespace std;

int main (int argc, char* argv[]) {

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 1) {

        int a[10];
        for (int i = 0; i < 10; i++) {
            a[i] = rand() % 10;
        }

        for (int i = 0; i < 4; i++) {
            if (i != 1) {
                MPI_Send(a, 10, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
    }

    if (processId != 1) {
        int count;
        MPI_Status status;
        MPI_Probe(1, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int buf[count];

        MPI_Recv(buf, count, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

        for (int i = 0; i < 10; i++) {
            printf("%i ", buf[i]);
        }
        printf("\n");
    }

    MPI_Finalize();

    return 0;
}
