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

    int subN = int(N / 2);
    int subM = int(M / 2);
    int s = subN * subM;
    int sizes[] = {s, s, s, s};
    int displs[] = {0 * s, 1 * s, 2 * s, 3 * s};

    int subMatrix[subN][subM];
    int decomposeMatrix[M * N];
    int subDecomposeMatrix[subN * subM];

    if (processId == ROOT) {
        int matrix[N][M];
        int c = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                matrix[i][j] = c++;
            }
        }

        c = 0;
        for (int dest = 0; dest < size; dest++) {
            int subMatrix[subN][subM];
            int plus = 0;

            if (dest >= 2) {
                plus = M / 2;
            }

            for (int i = dest % 2 * (N / 2), ii = 0; i < N / abs(dest % 2 - 2); i++, ii++) {
                for (int j = 0 + plus, jj = 0; j < M / 2 + plus; j++, jj++) {
                    subMatrix[ii][jj] = matrix[i][j];
                    decomposeMatrix[c++] = subMatrix[ii][jj];
                }
            }

            // проверяем что декомпозиция матрицы выполнена правильно
            printf("dest = %i\n", dest);
            string outStr = "-----------\n";
            for (int i = 0; i < subN; i++) {
                for (int j = 0; j < subM; j++) {
                    outStr += to_string(subMatrix[i][j]) + " ";
                    if (subMatrix[i][j] < 10) {
                        outStr += " ";
                    }
                }
                outStr += "\n";
            }
            outStr += "-----------\n";

            printf("%s", outStr.c_str());
        }
    }

    MPI_Scatterv(decomposeMatrix, sizes, displs, MPI_INT, &subDecomposeMatrix,
                 subN * subM, MPI_INT, 0, MPI_COMM_WORLD);

    string outStr = "Sub part on process = " + to_string(processId) + ": ";
    for (int i = 0; i < s; i++) {
        outStr += to_string(subDecomposeMatrix[i]) + " ";
    }
    outStr += "\n";

    printf("%s", outStr.c_str());

    MPI_Finalize();

    return 0;
}