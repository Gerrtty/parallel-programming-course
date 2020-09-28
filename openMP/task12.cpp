#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argv, char *argc[]) {

    // first solution
    #pragma omp parallel num_threads(8)
    {
        usleep(100 * (8 - omp_get_thread_num()));
        printf("Hello world, thread num = %i, num of threads = %i\n", omp_get_thread_num(),
               omp_get_num_threads());
    }

    printf("\n");

    // second solution
    #pragma omp parallel num_threads(8)
    {
        for(int i = omp_get_num_threads() - 1; i >= 0; i--) {
            #pragma omp barrier
            {
                if(i == omp_get_thread_num()) {
                    #pragma omp critical
                    printf("Hello world, thread num = %i, num of threads = %i\n", omp_get_thread_num(),
                            omp_get_num_threads());
                }
            }
        }
    }

    printf("\n");


//    #pragma omp parallel for num_threads(8)
//    for (int i = 7; i >= 0; i--) {
//        #pragma omp ordered
//        {
//            printf("Hello world, thread num = %i, num of threads = %i\n", omp_get_thread_num(),
//                   omp_get_num_threads());
//        }
//    }

    return 0;

}

