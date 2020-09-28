#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>

int main(int argv, char* argc[]) {
    int num = 210;
    int answer = 0;
    int i, j;

    #pragma omp parallel for private(i)
    for (i = 1; i < num * 2 ; i += 2) {
        #pragma omp atomic
        answer += i;
    }

    printf("%i\n", answer);

    return 0;
}