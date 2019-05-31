#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <iostream>

using namespace std;

//setting the number of threads:
#ifndef NUMT
#define NUMT 1
#endif

//setting the number of trials in the monte carlo simulation
#ifndef NUMTRIALS
#define NUMTRIALS 1000000
#endif

//how many tries to discover the max performance:
#ifndef NUMTRIES
#define NUMTRIES 10
#endif

//rnages for the random numbers:
const float XCMIN = -1.0;
const float XCMAX = 1.0;
const float YCMIN = 0.0;
const float YCMAX = 2.0;
const float RMIN = 0.5;
const float RMAX = 2.0;

//function prototypes:
float
Ranf(float low, float high){
    float r = (float)rand();
    float t = r / (float) RAND_MAX;
    return low + t *(high - low);
}

int
Ranf(int ilow, int ihigh){
    float low = (float)ilow;
    float high = ceil((float)ihigh);
    return (int)Ranf(low,high);
}
void TimeOfDaySeed();

void
TimeOfDaySeed(){
    struct tm y2k = {0};
    y2k.tm_hour = 0;
    y2k.tm_min = 0;
    y2k.tm_sec = 0;
    y2k.tm_year = 100;
    y2k.tm_mon = 0;
    y2k.tm_mday = 1;

    time_t timer;
    time(&timer);
    double seconds = difftime(timer, mktime(&y2k));
    unsigned int seed = (unsigned int)(1000.0*seconds);
    srand(seed);
}

//main program:
int
main(int argc, char *argv[]){
#ifndef _OPENMP
    fprintf(stderr, "No OpenMP support!\n");
    return 1;
#endif

    TimeOfDaySeed();
    omp_set_num_threads(NUMT);
    float *xcs = new float[NUMTRIALS];
    float *ycs = new float[NUMTRIALS];
    float *rs  = new float[NUMTRIALS];

    for(int n = 0; n < NUMTRIALS; n++){
        xcs[n] = Ranf(XCMIN, XCMAX);
        ycs[n] = Ranf(YCMIN, YCMAX);
         rs[n] = Ranf(RMIN, RMAX);
    }

    //get ready to record the maximum performance and the probability
    float maxPerformance = 0.0;
    float currentProb;
    for(int t = 0; t < NUMTRIES; t++){
        double time0 = omp_get_wtime();
        int numHits = 0;
        #pragma omp parallel for default(none) shared(xcs, ycs, rs) reduction(+:numHits)
        for(int n = 0; n < NUMTRIALS; n++){
            float xc = xcs[n];
            float yc = ycs[n];
            float  r =  rs[n];

            float a = 2.0;
            float b = -2.0*(xc+yc);
            float c = xc*xc + yc*yc - r*r;
            float d = b*b - 4.0*a*c;
            if(d < 0){
                //do nothing
                //completely missed
                //continue;
            }
            else{
                //hits the circle
                //get the first intersection
                d = sqrt(d);
                float t1 = (-b + d) / (2.0* a);
                float t2 = (-b - d) / (2.0* a);
                float tmin = t1 < t2 ? t1 : t2;
                if(tmin < 0){
                    //do nothing
                    //the circle engulfs the circle
                    //continue;
                }
                else{
                    float xcir = tmin;
                    float ycir = tmin;

                    float nx = xcir - xc;
                    float ny = ycir - yc;
                    float n = sqrt(nx*nx + ny*ny);
                    nx /= n;
                    ny /= n;

                    float inx = xcir - 0.0;
                    float iny = ycir - 0.0;
                    float in = sqrt(inx*inx + iny*iny);
                    inx /= in;
                    iny /= in;

                    float dot = inx*nx + iny*ny;
                    float outx = inx - 2.0*nx*dot;
                    float outy = iny - 2.0*ny*dot;

                    float t = (0.0 - ycir) / outy;
                    if(t < 0){
                        //do nothing
                        //the beam went up
                        //continue;
                    }
                    else{
                        numHits += 1;
                    }
                }
            }
        }
        double time1 = omp_get_wtime();
        double megaTrialsPerSecond = (double)NUMTRIALS / (time1 - time0) / 1000000.0;
        if(megaTrialsPerSecond > maxPerformance){
            maxPerformance = megaTrialsPerSecond;
        }
        currentProb = (float)numHits/(float)NUMTRIALS;
    }
    cout << NUMT << "," << NUMTRIALS << "," << currentProb << "," << maxPerformance << endl;
}
