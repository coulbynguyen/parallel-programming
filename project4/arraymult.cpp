#include "simd.p4.h"
#include <omp.h>
#include <iostream>
#include <time.h>

using namespace std;

#ifndef SIZE
#define SIZE 1000
#endif

#ifndef NUMTRIES
#define NUMTRIES 10
#endif

void
NonSimdMul(float *a, float *b, float *c, int len){
  for(int i = 0; i < len; i++){
    a[i] = b[i] * c[i];
  }
}


int main(){
  double max_performance_simd_mul = 0.0;
  double max_performance_non_simd_mul = 0.0;
  double max_performance_simd_mul_sum = 0.0;
  double max_performance_non_simd_mul_sum = 0.0;

  // for(int i = 0; i < NUMTRIES; i++){
  //   float *a = new float[SIZE];
  //   // a = {0.0}
  //   float *b = new float[SIZE];
  //   // b = {0.0}
  //   float *c = new float[SIZE];
  //   // c = {0.0};
  //   double time0 = omp_get_wtime();
  //
  //   SimdMul(a, b, c, SIZE);
  //
  //   double time1 = omp_get_wtime();
  //   double megaEvalPerSecond = (double)SIZE / (time1 - time0) / 1000000.0;
  //   if(megaEvalPerSecond > max_performance_simd_mul){
  //     max_performance_simd_mul = megaEvalPerSecond;
  //   }
  //
  // }
  //
  // for(int i = 0; i < NUMTRIES; i++){
  //   float *a = new float[SIZE];
  //   // a = {0.0};
  //   float *b = new float[SIZE];
  //   // b = {0.0};
  //   float *c = new float[SIZE];
  //   // c = {0.0};
  //
  //   double time0 = omp_get_wtime();
  //
  //   NonSimdMul(a, b, c, SIZE);
  //
  //   double time1 = omp_get_wtime();
  //   double megaEvalPerSecond = (double)SIZE / (time1 - time0) / 1000000.0;
  //   if(megaEvalPerSecond > max_performance_non_simd_mul){
  //     max_performance_non_simd_mul = megaEvalPerSecond;
  //   }
  //
  // }

  for(int i = 0; i < NUMTRIES; i++){
    float *a = new float[SIZE];
    float *b = new float[SIZE];

    double time0 = omp_get_wtime();

    SimdMulSum(a, b, SIZE);

    double time1 = omp_get_wtime();
    double megaEvalPerSecond = (double)SIZE / (time1 - time0) / 1000000.0;
    if(megaEvalPerSecond > max_performance_simd_mul_sum){
      max_performance_simd_mul_sum = megaEvalPerSecond;
    }
    delete [] a;
    delete [] b;

  }

  for(int i = 0; i < NUMTRIES; i++){
    float *a = new float[SIZE];
    float *b = new float[SIZE];

    double time0 = omp_get_wtime();

    NonSimdMulSum(a, b, SIZE);

    double time1 = omp_get_wtime();
    double megaEvalPerSecond = (double)SIZE / (time1 - time0) / 1000000.0;
    if(megaEvalPerSecond > max_performance_non_simd_mul_sum){
      max_performance_non_simd_mul_sum = megaEvalPerSecond;
    }
    delete [] a;
    delete [] b;
  }

  cout << SIZE
  // << "," << max_performance_simd_mul/max_performance_non_simd_mul
  << "," << max_performance_simd_mul_sum/max_performance_non_simd_mul_sum
  << endl;
  return 0;
}
