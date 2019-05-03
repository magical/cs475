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
const float XCMIN =	-1.0;
const float XCMAX =	+1.0;
const float YCMIN =	 0.0;
const float YCMAX =	 2.0;
const float RMIN  =	 0.5;
const float RMAX  =	 2.0;

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

	TimeOfDaySeed( );		// seed the random number generator

	omp_set_num_threads( numthreads );	// set the number of threads to use in the for-loop:`

	// better to define these here so that the rand() calls don't get into the thread timing:
	float *xcs = (float*)calloc(arraysize, sizeof xcs[0]);
	float *ycs = (float*)calloc(arraysize, sizeof ycs[0]);
	float * rs = (float*)calloc(arraysize, sizeof rs[0]);

	// fill the random-value arrays:
	for( int n = 0; n < arraysize; n++ )
	{
		xcs[n] = Ranf( XCMIN, XCMAX );
		ycs[n] = Ranf( YCMIN, YCMAX );
		rs[n] = Ranf(  RMIN,  RMAX );
	}

	// get ready to record the maximum performance and the probability:
	float maxPerformance = 0.;

	float out_cpu = NonSimdMulSum(xcs, ycs, arraysize);
	// looking for the maximum performance:
	for( int t = 0; t < NUMTRIES; t++ )
	{
		double time0 = omp_get_wtime( );

		// XXX do simd here

		float out_simd = SimdMulSum(xcs, ycs, arraysize);
		if (fabs(out_simd - out_cpu) > 0.0001) {
			fprintf(stderr, "warning: simd %f != cpu %f\n", out_simd, out_cpu);
		}

		//SimdMul(xcs, ycs, rs, arraysize);


		double time1 = omp_get_wtime( );
		double megaMulsPerSecond = (double)arraysize / ( time1 - time0 ) / 1000000.;
		if( megaMulsPerSecond > maxPerformance ) {
			maxPerformance = megaMulsPerSecond;
		}
	}

	printf("%d\t%ld\t0\t%f\n", numthreads, arraysize, maxPerformance);

	return 0;
}

float Ranf( float low, float high )
{
	float r = (float) rand();		// 0 - RAND_MAX
	float t = r  /	(float) RAND_MAX;	// 0. - 1.

	return	 low  +  t * ( high - low );
}


void TimeOfDaySeed( )
{
	struct tm y2k = { 0 };
	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time_t	timer;
	time( &timer );
	double seconds = difftime( timer, mktime(&y2k) );
	unsigned int seed = (unsigned int)( 1000.*seconds );	// milliseconds
	srand( seed );
}
