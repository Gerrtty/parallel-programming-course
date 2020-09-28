#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

/* При каждом новом запуске вывод может отличаться, так как по умолчанию
 * между потоками отсутвует синхронизация и поэтому последовательность вызова
 * потоков может отличаться.
 * */

int main(int argv, char *argc[]) {

    #pragma omp parallel num_threads(8)
    printf("Hello world, thread num = %i, num of threads = %i\n", omp_get_thread_num(),
            omp_get_num_threads());

}

