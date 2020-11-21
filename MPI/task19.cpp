#include <stdio.h>
#include <cstdlib>
#include <string>

#include <mpi.h>

using namespace std;

struct MyType {
    int rows[2][8];
};


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
    struct MyType myType;
    MPI_Datatype newType;
    MPI_Datatype type[1] = {MPI_INT};
    int blockLen[1] = {16};
    MPI_Aint disp[1] = {0};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if (processId == 0) {

        int a[8][8]; int c = 0;
        // заполняем матрицу числами от 0 до 8 * 8 - 1
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                a[i][j] = c++;
            }
        }
        // печатаем исходную матрицу
        printMatrix(*a, 8, 8, "original");

        for (int dest = 1; dest < size; dest++) {
            for (int j = 0; j < 8; j++) {
                myType.rows[0][j] = a[dest - 1][j];
                myType.rows[1][j] = a[dest + 3][j];
            }
            MPI_Type_create_struct(1, blockLen, disp, type, &newType);
            MPI_Type_commit(&newType);
            MPI_Send(&myType, 1, newType, dest, dest, MPI_COMM_WORLD);
        }
    }

    else {
        int d[2][8];
        // принимаем матрицы
        MPI_Recv(&d, 16, MPI_INT, 0, processId, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (processId == 4) {
            printMatrix(*d, 2, 8, "d");
        }

    }

    MPI_Finalize();

    return 0;
}