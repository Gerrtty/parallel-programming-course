#include <stdio.h>
#include <cstdlib>
#include <string>

#include <mpi.h>

using namespace std;

struct MyType {
    int rows[8][8];
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
    MPI_Datatype type[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    int blockLen[4] = {8, 8, 8, 8};
    MPI_Aint disp[4] = {0, 0, 0, 0};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int a[8][8]; int c = 0;
    // заполняем матрицу числами от 0 до 8 * 8 - 1
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            a[i][j] = c++;
            myType.rows[i][j] = a[i][j];
        }
    }
    // печатаем исходную матрицу
    if (processId == 0) {
        printMatrix(*a, 8, 8, "original");
    }

    MPI_Type_create_struct(2, blockLen, disp, type, &newType);
    MPI_Type_commit(&newType);

    int d[2][8];

    MPI_Scatter(&myType, 16, newType, &d, 16, newType, 0, MPI_COMM_WORLD);

    if (processId == 0) {
        printMatrix(*d, 8, 8, "d");
    }

    MPI_Finalize();

    return 0;
}