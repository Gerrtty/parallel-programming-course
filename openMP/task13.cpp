#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>

int main(int argv, char* argc[]) {

    int a[30] = {1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1};

    int answer = 0;

    #pragma omp parallel for
    for (int i = 0; i < 30; i++) {
        answer += a[30 - i - 1] * pow(2, i);
    }

    printf("%i\n", answer);

    return 0;
}