#include <stdio.h>
#include <cstdlib>
#include <string>

#include <mpi.h>

using namespace std;

#define M 4

void printMatrix(int* matrix, const int rows, const int columns, string name) {

    string out = "Matrix " + name + "\n-----------------------\n";
    printf("%s", out.c_str());

    for (int i = 0; i < rows * columns; i++) {
        if (matrix[i] > 10) {
            printf("%i ", matrix[i]);
        }
        else {
            printf("%i  ", matrix[i]);
        }

        if ((i + 1) % columns == 0) {
            printf("\n");
        }
    }

    printf("-----------------------\n");

}

void print(int* arr, const int len, string name) {

    string out = "Arr " + name + " = [";

    for (int i = 0; i < len; i++) {
        out += to_string(arr[i]);
        if (i < len - 1) {
            out += " ";
        }
    }

    out += "]\n";

    printf("%s", out.c_str());

}

int main(int argc, char* argv[]) {

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    MPI_Datatype myType;

    // исходная верхнетреугольная матрица
    int a[M][M];

    int countOfNonZeroElements = 0;
    for (int i = 0; i < M; i++) {
        countOfNonZeroElements += i + 1;
    }

    int blockLen[M];
    int displacement[M];
    int nonZeroElements[countOfNonZeroElements];

    if (processId == 0) {

        int c = 1;
        // заполняем верхнетреугольную матрицу
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                if (j > i) {
                    a[i][j] = 0;
                }
                else {
                    a[i][j] = c++;
                }
            }
        }

        // печатаем исходную матрицу
        printMatrix(*a, M, M, "original");

        // индексы ненулевых элементов
        for (int i = 0; i < M; i++) {
            blockLen[i] = i + 1;
            displacement[i] = i * M;
        }

        MPI_Type_indexed(M, blockLen, displacement, MPI_INT, &myType);
        MPI_Type_commit(&myType);

        MPI_Send(&a, 1, myType, 1, 0, MPI_COMM_WORLD);

    }

    if (processId == 1) {
        MPI_Recv(nonZeroElements, countOfNonZeroElements, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // отправляем ненулевые элементы на все процессы
    MPI_Bcast(nonZeroElements, countOfNonZeroElements, MPI_INT, 1, MPI_COMM_WORLD);

    string arrName = "on process " + to_string(processId);
    print(nonZeroElements, countOfNonZeroElements, arrName);

    MPI_Finalize();

    return 0;
}