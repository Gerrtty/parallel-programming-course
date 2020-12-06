#include "mpi.h"

#include <stdio.h>
#include <cstdlib>
#include <string>

#define N 3

using namespace std;

void print(auto* arr, const int len, string name) {

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

int main( int argc, char *argv[] ) {

    int worldSize, worldProcessId;

    MPI_Group worldGroup, newGroup1, newGroup2, newGroup3, newGroup4;

    MPI_Init(0, 0);

    MPI_Comm_rank(MPI_COMM_WORLD, &worldProcessId);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

    MPI_Comm newComm;
    // разделяем главную группу на 12 % 3 = 4 подгруппы
    MPI_Comm_split(MPI_COMM_WORLD, worldProcessId / 3, worldProcessId % 3, &newComm);

    int rank = -1;
    int size = -1;

    if (newComm != MPI_COMM_NULL) {
        MPI_Comm_rank(newComm, &rank);
        MPI_Comm_size(newComm, &size);
    }

    int g = worldProcessId / 3;
    int arr[4] = {g, g, g, g};

    int localArr[12];

    // собираем массивы
    MPI_Gather(&arr, 4, MPI_INT, &localArr, 4, MPI_INT, 0, newComm);

    // печатаем их
    if (rank == 0) {
        print(localArr, 12, "on group: " + to_string(g));
    }

    // для интеркоммутатора
    int tag = -1;
    int lead = -1;

    if (g == 0) {
        tag = 12;
        lead = 3;
    }

    if (g == 1) {
        tag = 12;
        lead = 0;
    }

    if (g == 2) {
        tag = 34;
        lead = 9;
    }

    if (g == 3) {
        tag = 34;
        lead = 6;
    }

    // создаем интеркоммутатор
    MPI_Comm intercomm;
    MPI_Intercomm_create(newComm, 0, MPI_COMM_WORLD, lead, tag, &intercomm);

    // обмен данными

    // данные с 0 на 1 и с 1 на 2
    if ((g == 0 || g == 2) && rank == 0) {
        MPI_Send(&localArr, 12, MPI_INT, 0, g, intercomm);
    }
    if ((g == 1 || g == 3) && rank == 0) {
        int bufArr[12];
        MPI_Recv(&bufArr, 12, MPI_INT, MPI_ANY_SOURCE, g - 1, intercomm, MPI_STATUS_IGNORE);
        print(bufArr, 12, "From group " + to_string(g - 1) + " on group " + to_string(g));

    }

    // данные с 1 на 0 и с 2 на 1
    if ((g == 1 || g == 3) && rank == 0) {
        MPI_Send(&localArr, 12, MPI_INT, 0, g * 100, intercomm);
    }
    if ((g == 0 || g == 2) && rank == 0) {
        int bufArr[12];
        MPI_Recv(&bufArr, 12, MPI_INT, MPI_ANY_SOURCE, (g + 1) * 100, intercomm, MPI_STATUS_IGNORE);
        print(bufArr, 12, "From group " + to_string(g + 1) + " on group " + to_string(g));
    }

    MPI_Finalize();

    return 0;
}