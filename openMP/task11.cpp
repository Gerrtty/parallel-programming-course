#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <iostream>
#include <string>
#include <ctime>
#include <chrono>

#define SIZE 300000

using namespace std;

int main(int argv, char *argc[]) {

    random_device r;
    default_random_engine generator(r());
    uniform_int_distribution<int> gen(0, 10);

    int arr[SIZE], count, i, max;

    for (int i = 0; i < SIZE; i++) {
        arr[i] = gen(generator);
    }

    count = 0;
    max = arr[0];

    #pragma omp parallel for private(i) shared(max) schedule(dynamic)

    for (i = 1; i < SIZE; i++) {
        if (arr[i] % 7 == 0) {
            #pragma omp critical
            if (arr[i] > max) {
                max = arr[i];
            }
        }
    }

    printf("Максимальный элемент, кратный 7 = %i\n", max);

    return 0;
}

