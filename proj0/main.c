#include <omp.h>
#include <stdio.h>
#include <math.h>

#define ARRAYSIZE 4096
#define NUMTRIES 100000

double A[ARRAYSIZE];
double B[ARRAYSIZE];
double C[ARRAYSIZE];

int main(int argc, char** argv) {
#ifndef _OPENMP
	fprintf(stderr, "OpenMP not supported\n");
	return 1;
#endif

	//int numt = omp_get_num_procs();
	int numt = 1;
	if (argc > 1) {
		numt = atoi(argv[1]);
	}
	omp_set_num_threads(numt);
	fprintf(stderr, "Using %d threads,\n", numt);

	double maxMegaMults = 0.;
	for(int t = 0; t <NUMTRIES; t++) {
		double time0 = omp_get_wtime();
		#pragma omp parallel for default(none) shared(A,B,C)
		for (int i = 0; i <  ARRAYSIZE; i++) {
			C[i] = A[i] *B[i];
		}
		double time1 = omp_get_wtime();
		double megaMults = (double)ARRAYSIZE / (time1 - time0) / 1e6;
		if (megaMults > maxMegaMults) {
			maxMegaMults = megaMults;
		}
	}

	printf( "Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults );

	return 0;

}
