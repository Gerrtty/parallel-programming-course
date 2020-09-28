#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <iostream>
#include <string>

#define SIZE 12

using namespace std;

int main(int argc, char *argv[]) {

    int a[SIZE], b[SIZE], c[SIZE], i, num;

    #pragma omp parallel num_threads(3) private(i) shared(a, b, num)
    {
        num = omp_get_num_threads();
        random_device r;
        default_random_engine generator(r());
        uniform_int_distribution<int> gen(0, 10);

        #pragma omp for schedule(static, SIZE / num)
        for (i = 0; i < SIZE; i++) {
            a[i] = gen(generator);
            b[i] = gen(generator);
        }

        printf("num threads = %i in parallel area 1\n", num);
        printf("thread num = %i in parallel area 1\n", omp_get_thread_num());
        string as = "";
        string bs = "";
        for (int i = 0; i < SIZE; i++) {
            as += to_string(a[i]) + " ";
        }
        for (int i = 0; i < SIZE; i++) {
            bs += to_string(b[i]) + " ";
        }
        string s = "A = " + as + "B = " + bs + "\n";
        cout << s;
    }

    #pragma omp parallel num_threads(4) shared(c, num)
    {
        num = omp_get_num_threads();

        #pragma omp for schedule(dynamic, SIZE / num)

        for (int i = 0; i < SIZE; i++) {
            c[i] = a[i] + b[i];
        }

        string cs = "";
        for (int i = 0; i < SIZE; i++) {
            cs += to_string(c[i]) + " ";
        }
        string s = "C = " + cs + "\n";
        cout << s;

        printf("num threads = %i in parallel area 2\n", num);
        printf("thread num = %i in parallel area 2\n", omp_get_thread_num());
    }

}