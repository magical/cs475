#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "read.h"

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	10
#endif

float AutoCorrelate(float *array, int size, int shift) {
	float sum = 0.;
	for (int i = 0; i < size; i++) {
		sum += array[i] * array[i + shift];
	}
	return sum;
}

// main program:
int
main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
	fprintf( stderr, "No OpenMP support!\n" );
	return 1;
#endif

	int numthreads = 1;
	if (argc > 1) {
		numthreads = atoi(argv[1]);
	}

	float *array = NULL;
	float *sums = NULL;
	int size = 0;

	ReadData("signal.txt", &array, &size);

	omp_set_num_threads( numthreads );	// set the number of threads to use in the for-loop:`

	sums = new float[size];

	// get ready to record the maximum performance and the probability:
	double maxPerformance = 0.;

	// looking for the maximum performance:
	for( int t = 0; t < NUMTRIES; t++ )
	{
		double time0 = omp_get_wtime( );

		#pragma omp parallel for default(none) shared(array, sums, size)
		for( int shift = 0; shift < size; shift++ )
		{
			sums[shift] = AutoCorrelate(array, size, shift);
		}
		double time1 = omp_get_wtime( );
		double d = time1 - time0;
		double kiloAutoCorrelatesPerSecond = (double)size / d / 1e3;
		if (maxPerformance < kiloAutoCorrelatesPerSecond) {
			maxPerformance = kiloAutoCorrelatesPerSecond;
		}
	}

	fprintf( stderr, "Threads = %d, Array Size = %10d, KiloAutoCorrelates/Second = %10.2lf\n", numthreads, size, maxPerformance );
	for (int i = 1; i <= 512; i++) {
		printf("%d\t%f\n", i, sums[i]);
	}

	delete sums;
	delete array;

	return 0;
}
