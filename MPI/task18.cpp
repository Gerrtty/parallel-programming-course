#include <stdio.h>
#include <cstdlib>
#include <string>

#include <mpi.h>

using namespace std;

void printMatrix(int* matrix, const int rows, const int columns, string name) {

    string out = "Matrix " + name + "\n-----------------------\n";
    printf("%s", out.c_str());

    int k = 0;
    for (int i = 0; i < rows * columns; i++) {
        k++;
        if (matrix[i] > 10) {
            printf("%i ", matrix[i]);
        }
        else {
            printf("%i  ", matrix[i]);
        }
        if (k == columns) {
            printf("\n");
            k = 0;
        }
    }

    printf("-----------------------\n");

}

int main(int argc, char* argv[]) {

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {

        int a[8][8]; int c = 0;

        // заполняем матрицу числами от 0 до 1
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                a[i][j] = c++;
            }
        }

        // печатаем исходную матрицу
        printMatrix(*a, 8, 8, "original");

        MPI_Datatype newType;
        MPI_Type_vector(4, 8, 16, MPI_INT, &newType);
        MPI_Type_commit(&newType);

        // отправляем чётные строки
        MPI_Send(&a[0][0], 1, newType, 1, 0, MPI_COMM_WORLD);
        // отправляем нечётные строки
        MPI_Send(&a[1][0], 1, newType, 1, 10, MPI_COMM_WORLD);

    }

    else {
        int b[4][8];
        int c[4][8];

        // принимаем матрицы
        MPI_Recv(&b, 32, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&c, 32, MPI_INT, 0, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // печатаем их
        printMatrix(*b, 4, 8, "a");
        printMatrix(*c, 4, 8, "b");

    }

    MPI_Finalize();

    return 0;
}