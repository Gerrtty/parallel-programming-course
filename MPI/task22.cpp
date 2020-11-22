#include "mpi.h"

#include <stdio.h>
#include <cstdlib>
#include <string>

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

    MPI_Group worldGroup, newGroup;
    int worldSize, worldProcessId;
    int newGroupSize, newGroupProcessId;

    MPI_Init(0, 0);

    MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldProcessId);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int ranks[5] = {8, 3, 9, 1, 6};
    MPI_Group_incl(worldGroup, 5, ranks, &newGroup);

    MPI_Group_size(newGroup, &newGroupSize);
    MPI_Group_rank(newGroup, &newGroupProcessId);

    MPI_Comm newcomm;
    MPI_Comm_create(MPI_COMM_WORLD, newGroup, &newcomm);

    float arr[5];

    if (MPI_COMM_NULL != newcomm) {
        if (newGroupProcessId == 0) {

            for (int i = 0; i < 5; i++) {
                arr[i] = float(i) / 2;
            }

            print(arr, 5, "floats on zero process of new group");
        }

        MPI_Bcast(arr, 5, MPI_FLOAT, 0, newcomm);

        print(arr, 5, "on process in new group " + to_string(newGroupProcessId) + " and global process " + to_string(worldProcessId));

        if (newGroupProcessId == newGroupSize - 1) {
            MPI_Send(&arr, 5, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        }
    }

    if (worldProcessId == 0) {

        float buff[5];

        MPI_Recv(buff, 5, MPI_FLOAT, ranks[4], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        print(buff, 5, "on process 0 of original group");
    }

    MPI_Finalize();

    return 0;
}