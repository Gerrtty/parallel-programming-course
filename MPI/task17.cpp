#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>

#include <mpi.h>

#define ROOT 0
#define N 4
#define M 4

using namespace std;

int main(int argc, char* argv[]) {

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    double starttime, endtime;

    if (processId == 0) {
        starttime = MPI_Wtime();
    }

    // пускай число x которое хранит каждый поток будет равно его номеру (для простоты проверки)
    int x = processId;
    int globalSumX = 0;

    MPI_Reduce(&x, &globalSumX, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Bcast(&globalSumX, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (processId == 0) {
        endtime = MPI_Wtime();
        printf("Time with MPI_Reduce and MPI_Bcast = %f\n", (endtime - starttime));
    }

    printf("Global sum x on process = %i: %i\n", processId, globalSumX);

    MPI_Finalize();

    return 0;
}