#include <stdio.h>
#include <mpi.h>
#include <random>

#define VECTOR_SIZE 12

using namespace std;

int main (int argc, char* argv[]) {

    srand(time(NULL));

    int a = 2;
    int b = 4;

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {

        // init vectors x and y with random numbers
        int x[VECTOR_SIZE];
        int y[VECTOR_SIZE];

        for (int i = 0; i < VECTOR_SIZE; i++) {
            x[i] = rand() % 10;
            y[i] = rand() % 10;
            x[i] = i;
            y[i] = i;
        }

        // print x
        printf("Original array x = [");
        for (int i = 0; i < VECTOR_SIZE; i++) {
            if (i < VECTOR_SIZE - 1) {
                printf("%i ", x[i]);
            }
            else printf("%i]", x[i]);
        }
        printf("\n");

        // print y
        printf("Original array y = [");
        for (int i = 0; i < VECTOR_SIZE; i++) {
            if (i < VECTOR_SIZE - 1) {
                printf("%i ", y[i]);
            }
            else printf("%i]", y[i]);
        }
        printf("\n");

        int recv_size = size - 1;

        const int block_size = VECTOR_SIZE / recv_size + 1;

        printf("splitted_arrays_count = %i, block size = %i\n", recv_size, block_size);

        int reminder_size = VECTOR_SIZE;

        int c = 0;

        for (int i = 1; i < size; i++) {

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

            int splitted_arr_x[size_arr];
            int splitted_arr_y[size_arr];

            reminder_size -= block_size;

            for (int j = 0; j < size_arr; j++) {
                splitted_arr_x[j] = x[c];
                splitted_arr_y[j] = y[c++];
            }

            MPI_Send(splitted_arr_x, size_arr, MPI_INT, i, 100, MPI_COMM_WORLD);
            MPI_Send(splitted_arr_y, size_arr, MPI_INT, i, 111, MPI_COMM_WORLD);
        }

        int z[VECTOR_SIZE];

        for (int i = 1; i < size; i++) {
            int count;
            MPI_Status status;
            MPI_Probe(i, i, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);
            int buf_z[count];

            MPI_Recv(buf_z, count, MPI_INT, i, i, MPI_COMM_WORLD, &status);

            int c = 0;
            for (int j = 0; j < count; j++) {
                int s = count;
                if (count < block_size) {
                    s = block_size;
                }
                z[(i-1) * s + j] = buf_z[j];
            }
        }

        printf("vector z = ");
        for (int i = 0; i < VECTOR_SIZE; i++) {
            printf("%i ", z[i]);
        }
        printf("\n");
    }

    if (processId != 0) {
        int count;
        MPI_Status status;
        MPI_Probe(0, 100, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int buf_x[count];
        int buf_y[count];

        MPI_Recv(buf_x, count, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
        MPI_Recv(buf_y, count, MPI_INT, 0, 111, MPI_COMM_WORLD, &status);

        int result_arr[count];

        for (int i = 0; i < count; i++) {
            result_arr[i] = buf_x[i] * a + buf_y[i] * b;
        }

        printf("Part of array on process %i = [", processId);
        for (int i = 0; i < count; i++) {
            if (i < count - 1) {
                printf("%i ", result_arr[i]);
            }
            else {
                printf("%i", result_arr[i]);
            }
        }
        printf("]\n");

        MPI_Send(result_arr, count, MPI_INT, 0, processId, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}