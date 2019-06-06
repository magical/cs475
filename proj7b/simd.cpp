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

float SimdAutoCorrelate( float *array, int len, int shift )
{
	const int SSE_WIDTH = 4;
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;

	{
		float *a0 = array, *a1 = &array[shift];
		register __int128 sum0 __asm ("xmm2") = 0;
		for (int i = 0; i < limit; i += SSE_WIDTH) {
			__asm (
				".att_syntax\n\t"
				"movups	(%0), %%xmm0\n\t"	// load the first sse register
				"movups	(%1), %%xmm1\n\t"	// load the second sse register
				"mulps	%%xmm1, %%xmm0\n\t"	// do the multiply
				"addps	%%xmm0, %2\n\t"	// do the add
				"addq $16, %0\n\t"
				"addq $16, %1\n\t"
				: /* outputs */ "+r" (a0), "+r" (a1), "+x" (sum0)
				: /* inputs */ "m" (*a0), "m" (*a1)
				: /* clobbers */ "xmm0", "xmm1"
			);
		}
		__asm ("movups %1,%0" : "=m" (sum) : "x" (sum0) : /*no clobbers*/);
	}

	for( int i = limit; i < len; i++ )
	{
		sum[0] += array[i] * array[i + shift];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
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
			sums[shift] = SimdAutoCorrelate(array, size, shift);
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
