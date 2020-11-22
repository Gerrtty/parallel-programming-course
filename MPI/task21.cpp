#include <stdio.h>
#include <cstdlib>
#include <string>

#include <mpi.h>

#define BUFF_SIZE 300

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

int main(int argc, char* argv[]) {

    int processId, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int position;
    char buff[BUFF_SIZE];

    int ints[10];
    float floats[8];

    if (processId == 0) {

        for (int i = 0; i < 10; i++) {
            ints[i] = i;
            if (i < 8) {
                floats[i] = float(i) / 2;
            }
        }

        position = 0;
        MPI_Pack(ints, 10, MPI_INT, buff, BUFF_SIZE, &position, MPI_COMM_WORLD);
        MPI_Pack(floats, 8, MPI_FLOAT, buff, BUFF_SIZE, &position, MPI_COMM_WORLD);
    }

    position = 0;

    MPI_Bcast(buff, 72, MPI_PACKED, 0, MPI_COMM_WORLD);

    MPI_Unpack(buff, BUFF_SIZE, &position, ints, 10, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buff, BUFF_SIZE, &position, floats, 8, MPI_FLOAT, MPI_COMM_WORLD);

    print(ints, 10, "ints on process: " + to_string(processId));
    print(floats, 8, "floats on process: " + to_string(processId));

    MPI_Finalize();

    return 0;

}
