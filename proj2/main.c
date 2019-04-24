#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

// number of threads to spin up
#define NUMTHREADS	1

// maximum number of trials in the monte carlo simulation:
#define MAXTRIALS	1000000

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	10
#endif

#define XMIN	 0.
#define XMAX	 3.
#define YMIN	 0.
#define YMAX	 3.


float Height( int iu, int iv, int numnodes );


int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
	fprintf( stderr, "No OpenMP support!\n" );
	return 1;
#endif

	int numthreads = NUMTHREADS;
	if (argc > 1) {
		numthreads = atoi(argv[1]);
	}

	int numtrials = MAXTRIALS;
	if (argc > 2) {
		numtrials = atoi(argv[2]);
		if (numtrials <= 0 || numtrials > MAXTRIALS) {
			numtrials = MAXTRIALS;
		}
	}

	int numnodes = numtrials; // TODO: rename numtrials

	omp_set_num_threads( numthreads ); // set the number of threads to use in the for-loop:`

	// get ready to record the maximum performance and the probability:
	float maxPerformance = 0;
	double volume = 0;

	// looking for the maximum performance:
	for( int t = 0; t < NUMTRIES; t++ )
	{
		double time0 = omp_get_wtime( );
		volume = 0;

		// -------------------------------

		// the area of a single full-sized tile:

		double fullTileArea = ((XMAX - XMIN) / (float)(numnodes-1)) *
		                      ((YMAX - YMIN) / (float)(numnodes-1));

		// sum up the weighted heights into the variable "volume"
		// using an OpenMP for loop and a reduction:

		// TODO
		#pragma omp parallel for default(none) shared(fullTileArea,numnodes) reduction(+:volume)
		for (int i = 0; i < numnodes*numnodes; i++) {
			int u = i % numnodes;
			int v = i / numnodes;
			double h = Height(u, v, numnodes);
			double area = fullTileArea;
			if (u == 0 || u == numnodes - 1) {
				area *= 0.5;
			}
			if (v == 0 || v == numnodes - 1) {
				area *= 0.5;
			}
			volume += h * area;
		}

		// -------------------------------

		double time1 = omp_get_wtime( );
		double megaTrialsPerSecond = (double)numnodes*(double)numnodes / ( time1 - time0 ) / 1000000.;
		if( megaTrialsPerSecond > maxPerformance ) {
			maxPerformance = megaTrialsPerSecond;
		}
	}

// XXX Print out: (1) the number of threads, (2) the number of trials, (3) the probability of hitting the plate, and (4) the MegaTrialsPerSecond. Printing this as a single line with tabs between the numbers is nice so that you can import these lines right into Excel.
	printf("%d\t%d\t%f\t%f\n", numthreads, numnodes, volume, maxPerformance);

	return 0;
}

// Returns the height of the surface at point iu,iv
//
// iu,iv = 0 .. numnodes-1
float
Height( int iu, int iv, int numnodes )
{
	float u = (float)iu / (float)(numnodes-1);
	float v = (float)iv / (float)(numnodes-1);

	// the basis functions:

	float bu0 = (1.-u) * (1.-u) * (1.-u);
	float bu1 = 3. * u * (1.-u) * (1.-u);
	float bu2 = 3. * u * u * (1.-u);
	float bu3 = u * u * u;

	float bv0 = (1.-v) * (1.-v) * (1.-v);
	float bv1 = 3. * v * (1.-v) * (1.-v);
	float bv2 = 3. * v * v * (1.-v);
	float bv3 = v * v * v;

#define TOPZ00	0.
#define TOPZ10	1.
#define TOPZ20	0.
#define TOPZ30	0.

#define TOPZ01	 1.
#define TOPZ11	12.
#define TOPZ21	 1.
#define TOPZ31	 0.

#define TOPZ02	0.
#define TOPZ12	1.
#define TOPZ22	0.
#define TOPZ32	4.

#define TOPZ03	3.
#define TOPZ13	2.
#define TOPZ23	3.
#define TOPZ33	3.

#define BOTZ00	0.
#define BOTZ10	-3.
#define BOTZ20	0.
#define BOTZ30	0.

#define BOTZ01	-2.
#define BOTZ11	10.
#define BOTZ21	-2.
#define BOTZ31	0.

#define BOTZ02	0.
#define BOTZ12	-5.
#define BOTZ22	0.
#define BOTZ32	-6.

#define BOTZ03	-3.
#define BOTZ13	 2.
#define BOTZ23	-8.
#define BOTZ33	-3.


	// finally, we get to compute something:
	float top =       bu0 * ( bv0*TOPZ00 + bv1*TOPZ01 + bv2*TOPZ02 + bv3*TOPZ03 )
	                + bu1 * ( bv0*TOPZ10 + bv1*TOPZ11 + bv2*TOPZ12 + bv3*TOPZ13 )
	                + bu2 * ( bv0*TOPZ20 + bv1*TOPZ21 + bv2*TOPZ22 + bv3*TOPZ23 )
	                + bu3 * ( bv0*TOPZ30 + bv1*TOPZ31 + bv2*TOPZ32 + bv3*TOPZ33 );

	float bot =       bu0 * ( bv0*BOTZ00 + bv1*BOTZ01 + bv2*BOTZ02 + bv3*BOTZ03 )
	                + bu1 * ( bv0*BOTZ10 + bv1*BOTZ11 + bv2*BOTZ12 + bv3*BOTZ13 )
	                + bu2 * ( bv0*BOTZ20 + bv1*BOTZ21 + bv2*BOTZ22 + bv3*BOTZ23 )
	                + bu3 * ( bv0*BOTZ30 + bv1*BOTZ31 + bv2*BOTZ32 + bv3*BOTZ33 );

	// if the bottom surface sticks out above the top surface
	// then that contribution to the overall volume is negative
	return top - bot;
}
