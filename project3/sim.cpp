#include <iostream>
#include <omp.h>
#include <math.h>
#include <cmath>
#include <time.h>
#include <stdlib.h>

using namespace std;

int	NowYear;		// 2019 - 2024
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int NowNumHuman;

int count; //count the month as the x axis label

const float GRAIN_GROWS_PER_MONTH =		8.0;
const float ONE_DEER_EATS_PER_MONTH =		0.5;

const float AVG_PRECIP_PER_MONTH =		6.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				50.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

float
SQR( float x ){
        return x*x;
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

void
change_state(){
  float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

  float temp = AVG_TEMP - AMP_TEMP * cos( ang );
  unsigned int seed = rand()%2500;
  NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

  float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
  NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
  if( NowPrecip < 0. )
	 NowPrecip = 0.;
}

void Grain(){
  while(NowYear < 2025){
    float local_height = NowHeight;
    float tempFactor = exp( -SQR((NowTemp - MIDTEMP) / 10. ));
    float precipFactor = exp( -SQR((NowPrecip - MIDPRECIP) / 10.));
    local_height += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
    local_height -= (float) NowNumDeer * ONE_DEER_EATS_PER_MONTH;
    if(local_height < 0 ){
      local_height = 0;
    }
    //Done computing barrier:
    #pragma omp barrier

    NowHeight = local_height;
    //Done assigning barrier:
    #pragma omp barrier

    //Done Printing barrier:
    #pragma omp barrier
  }
}

void GrainDeer(){
  while(NowYear < 2025){
    int local_deer = NowNumDeer;
    if(NowHeight > NowNumDeer){
      local_deer += 1;
    }
    else if(NowHeight < NowNumDeer){
      local_deer -= 1;
    }

    if(NowNumHuman > NowNumDeer){
      local_deer -= 1;
    }

    if(local_deer < 0){
      local_deer = 0;
    }
    //Done Computing Barrier:
    #pragma omp barrier

    NowNumDeer = local_deer;
    //Done Assigning Barrier:
    #pragma omp barrier

    //Done Printing Barrier:
    #pragma omp barrier
  }
}

void Watcher(){
  while(NowYear < 2025){
    //Done Computing Barrier:
    #pragma omp barrier

    //Done Assigning Barrier
    #pragma omp barrier


    float celsius = (5.0/9.0)*(NowTemp-32);
    // cout << NowMonth+1 << "," << NowYear << "," << NowPrecip << "," << NowTemp << "," << NowHeight << "," << NowNumDeer << "," << NowNumHuman << endl;
    cout << count << "," << NowPrecip << "," << celsius << "," << NowHeight << "," << NowNumDeer << "," << NowNumHuman << endl;

    change_state();
    count += 1;
    NowMonth += 1;
    if(NowMonth == 12){
      NowYear += 1;
      NowMonth = 0;
    }
    //Done printing barrier
    #pragma omp barrier
  }
}

void Human(){
  while(NowYear < 2025){
    int local_human = NowNumHuman;
    if(NowNumDeer > NowNumHuman){
      local_human += 1;
    }
    else if(NowNumDeer < NowNumHuman){
      local_human -= 1;
    }

    if(local_human < 0){
      local_human = 0;
    }
    //Done Computing Barrier:
    #pragma omp barrier

    NowNumHuman = local_human;
    //Done Assigning Barrier:
    #pragma omp barrier


    //Done Printing Barrier:
    #pragma omp barrier

  }
}

int
main(int argc, char *argv[]){
  NowMonth = 0;
  NowYear = 2019;

  NowNumDeer = 1;
  NowHeight = 1;
  NowNumHuman = 1;

  count = 0;

  srand(time(NULL));

  change_state();
  omp_set_num_threads(4); //change to 4 when adding in new agent
  #pragma omp parallel sections
  {
    #pragma omp section
    {
      Grain();
    }
    #pragma omp section
    {
      GrainDeer();
    }
    #pragma omp section
    {
      Watcher();
    }
    #pragma omp section
    {
      Human();
    }
  }



  return 0;
}
