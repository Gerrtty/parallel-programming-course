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

    int arr[SIZE], count, i;

    for (int i = 0; i < SIZE; i++) {
        arr[i] = gen(generator);
    }

    count = 0;

    #pragma omp parallel for private(i) shared(arr, count)
    for (i = 0; i < SIZE; i++) {
        if (arr[i] % 9 == 0) {
            #pragma omp atomic
            count++;
        }
    }

    printf("Количество чисел кратных 9 = %i\n", count);

    return 0;
}

