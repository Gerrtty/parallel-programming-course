#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

int main(int argv, char* argc[]) {

    int start = 2;
    int end = 10;

    int count = 0;

    #pragma omp parallel for
    for (int i = start; i < end; i++) {
        bool isSimple = true;
        for (int j = 2; j < i && isSimple; j++) {
            if (i % j == 0) {
                isSimple = false;
            }
        }
        if (isSimple) {
            #pragma omp atomic
            count++;
        }
    }


    printf("%i simple numbers in %i...%i\n", count, start, end);

    return 0;
}