#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "simd.p4.h"

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	10
#endif

// ranges for the random numbers:
const float RANDMIN =	-1.0;
const float RANDMAX =	+1.0;

// function prototypes:
float		Ranf( float, float );
void		TimeOfDaySeed( );

// main program:
int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
	fprintf( stderr, "No OpenMP support!\n" );
	return 1;
#endif

	int numthreads = 1;
	if (argc > 1) {
		numthreads = atoi(argv[1]);
	}

	long arraysize = 1024;
	if (argc > 2) {
		arraysize = atol(argv[2]);
		if (arraysize <= 0 || arraysize > (1<<30)) {
			fprintf(stderr, "invalid array size: %ld\n", arraysize);
			return 1;
		}
	}

	srand(0);
	//TimeOfDaySeed( );		// seed the random number generator

	// better to define these here so that the rand() calls don't get into the thread timing:
	float *arr1 = (float*)calloc(arraysize, sizeof arr1[0]);
	float *arr2 = (float*)calloc(arraysize, sizeof arr2[0]);
	float * rs = (float*)calloc(arraysize, sizeof rs[0]);

	// fill the random-value arrays:
	for( int n = 0; n < arraysize; n++ )
	{
		arr1[n] = Ranf( RANDMIN, RANDMAX );
		arr2[n] = Ranf( RANDMIN, RANDMAX );
		rs[n] = 0;
	}

	// get ready to record the maximum performance and the probability:
	double minSimdTime = INFINITY;
	double minCpuTime  = INFINITY;

	// looking for the maximum performance:

	float got_cpu = 0.;
	for( int t = 0; t < NUMTRIES; t++ )
	{
		double time0 = omp_get_wtime( );
		//got_cpu = NonSimdMulSum(arr1, arr2, arraysize);
		NonSimdMul(arr1, arr2, rs, arraysize);
		double time1 = omp_get_wtime( );
		double d = time1 - time0;
		if (d < minCpuTime) {
			minCpuTime = d;
		}
	}

	float got_simd = 0.;
	for( int t = 0; t < NUMTRIES; t++ )
	{
		double time0 = omp_get_wtime( );
		//got_simd = SimdMulSum(arr1, arr2, arraysize);
		SimdMul(arr1, arr2, rs, arraysize);
		double time1 = omp_get_wtime( );
		double d = time1 - time0;
		if (d < minSimdTime) {
			minSimdTime = d;
		}
	}

	if (fabs(got_simd - got_cpu) > 0.00001) {
		fprintf(stderr, "warning: simd %f != cpu %f\n", got_simd, got_cpu);
	}

	double mulSumSpeedup = minCpuTime / minSimdTime;

	printf("%d\t%ld\t%f\t%f\t%g\t%g\n", numthreads, arraysize, got_simd, mulSumSpeedup, minSimdTime, minCpuTime);

	return 0;
}

float Ranf( float low, float high )
{
	float r = (float) rand();		// 0 - RAND_MAX
	float t = r  /  (float) RAND_MAX;	// 0. - 1.

	return	 low  +  t * ( high - low );
}


void TimeOfDaySeed( )
{
	struct tm y2k = { };
	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time_t	timer;
	time( &timer );
	double seconds = difftime( timer, mktime(&y2k) );
	unsigned int seed = (unsigned int)( 1000.*seconds );	// milliseconds
	srand( seed );
}
