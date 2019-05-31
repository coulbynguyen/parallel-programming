#include <omp.h>
#include <stdio.h>
#include <math.h>

#define NUMT 1
#define ARRAYSIZE 100000
#define NUMTRIES 100

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];

int main(){
#ifndef _OPENMP
    fprintf(stderr, "OpenMP is not supported here -- sorry.\n");
    return 1;
#endif
    omp_set_num_threads(NUMT);
    fprintf(stderr, "Using %d threads\n", NUMT);

    double maxMegaMults = 0.0;
    double minTime = 0.0;

    for(int t = 0; t < NUMTRIES; t++){
        double time0 = omp_get_wtime();
        #pragma omp parallel for
        for(int i = 0; i < ARRAYSIZE; i++){
            C[i] = A[i] * B[i];
        }

        double time1 = omp_get_wtime();
        double megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.0;
        if(megaMults > maxMegaMults){
            minTime = (double)time1-time0;
            maxMegaMults = megaMults;
        }
    }
    printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
    printf("Time Elapsed for Peak Performance = %f Seconds\n", minTime);
    return 0;

}
