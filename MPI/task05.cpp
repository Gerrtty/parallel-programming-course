#include <stdio.h>
#include <mpi.h>
#include <random>

#define ARR_SIZE 12

using namespace std;

int main (int argc, char* argv[]) {

    srand(time(NULL));

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {

        // init array with random numbers
        int a[ARR_SIZE];
        for (int i = 0; i < ARR_SIZE; i++) {
            a[i] = rand() % 10;
        }

        // print
        printf("Original array = [");
        for (int i = 0; i < ARR_SIZE; i++) {
            if (i < ARR_SIZE - 1) {
                printf("%i ", a[i]);
            }
            else printf("%i]", a[i]);
        }
        printf("\n");

        const int block_size = ARR_SIZE / size + 1;

        printf("splitted_arrays_count = %i, block size = %i\n", size, block_size);

        int zero_process_arr[block_size];

        int reminder_size = ARR_SIZE;

        int c = 0;

        for (int i = 0; i < size; i++) {

            int size_arr;

            if (reminder_size - block_size >= 0) {
                size_arr = block_size;
            }

            else {
                size_arr = reminder_size;
            }

            if (size_arr < 0) {
                size_arr = 0;
            }

            int splitted_arr[size_arr];

            reminder_size -= block_size;

            for (int j = 0; j < size_arr; j++) {
                splitted_arr[j] = a[c++];
            }

            if (i == 0) {
                for (int j = 0; j < block_size; j++) {
                    zero_process_arr[j] = splitted_arr[j];
                }
            }

            if (i > 0) {
                MPI_Send(splitted_arr, size_arr, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

        }

        printf("Part of array on process 0 = [");
        for (int i = 0; i < block_size; i++) {
            if (i < block_size - 1) {
                printf("%i ", zero_process_arr[i]);
            } else {
                printf("%i", zero_process_arr[i]);
            }
        }
        printf("]\n");
    }

    if (processId != 0) {
        int count;
        MPI_Status status;
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int buf[count];

        MPI_Recv(buf, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        printf("Part of array on process %i = [", processId);
        for (int i = 0; i < count; i++) {
            if (i < count - 1) {
                printf("%i ", buf[i]);
            }
            else {
                printf("%i", buf[i]);
            }
        }
        printf("]\n");
    }

    MPI_Finalize();

    return 0;
}
