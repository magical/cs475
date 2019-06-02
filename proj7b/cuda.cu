// Array multiplication: C = A * B:
// vim: syntax=cuda noet sw=8

// System includes
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

// CUDA runtime
#include <cuda_runtime.h>

// Helper functions and utilities to work with CUDA
//#include "helper_functions.h"
#include "helper_cuda.h"


#ifndef BLOCKSIZE
#define BLOCKSIZE		32		// number of threads per block
#endif

#ifndef SIZE
#define SIZE			1*1024*1024	// array size
#endif


__global__ void AutoCorrelate(float *dArray, float *dSums, int size) {
	int gid = blockIdx.x*blockDim.x + threadIdx.x;

	int shift = gid;
	float sum = 0.;
	for (int i = 0; i < size; i++) {
		sum += dArray[i] * dArray[i + shift];
	}
	dSums[shift] = sum;
}


// main program:

int
main( int argc, char* argv[ ] )
{
	int dev = findCudaDevice(argc, (const char **)argv);

	// allocate host memory:

	float * hA = new float [ SIZE ];
	float * hB = new float [ SIZE ];
	float * hC = new float [ SIZE/BLOCKSIZE ];

	for( int i = 0; i < SIZE; i++ )
	{
		hA[i] = hB[i] = (float) sqrt(  (float)(i+1)  );
	}

	// allocate device memory:

	float *dA, *dB, *dC;

	dim3 dimsA( SIZE, 1, 1 );
	dim3 dimsB( SIZE, 1, 1 );
	dim3 dimsC( SIZE/BLOCKSIZE, 1, 1 );

	//__shared__ float prods[SIZE/BLOCKSIZE];


	checkCudaErrors(cudaMalloc( reinterpret_cast<void **>(&dA), SIZE*sizeof(float) ));
	checkCudaErrors(cudaMalloc( reinterpret_cast<void **>(&dB), SIZE*sizeof(float) ));
	checkCudaErrors(cudaMalloc( reinterpret_cast<void **>(&dC), (SIZE/BLOCKSIZE)*sizeof(float) ));


	// copy host memory to the device:

	checkCudaErrors(cudaMemcpy( dA, hA, SIZE*sizeof(float), cudaMemcpyHostToDevice ));
	checkCudaErrors(cudaMemcpy( dB, hB, SIZE*sizeof(float), cudaMemcpyHostToDevice ));

	// setup the execution parameters:

	dim3 threads(BLOCKSIZE, 1, 1 );
	dim3 grid( SIZE / threads.x, 1, 1 );

	// Create and start timer

	cudaDeviceSynchronize( );

	// allocate CUDA events that we'll use for timing:

	cudaEvent_t start, stop;
	checkCudaErrors(cudaEventCreate( &start ));
	checkCudaErrors(cudaEventCreate( &stop ));

	// record the start event:

	checkCudaErrors(cudaEventRecord( start, NULL ));

	// execute the kernel:

	AutoCorrelate<<< grid, threads >>>( dA, dB, dC );

	// record the stop event:

	checkCudaErrors(cudaEventRecord( stop, NULL ));

	// wait for the stop event to complete:

	checkCudaErrors(cudaEventSynchronize( stop ));

	float msecTotal = 0.0f;
	checkCudaErrors(cudaEventElapsedTime( &msecTotal, start, stop ));

	// compute and print the performance

	double secondsTotal = 0.001 * (double)msecTotal;
	double multsPerSecond = (float)SIZE / secondsTotal;
	double megaMultsPerSecond = multsPerSecond / 1000000.;
	fprintf( stderr, "Array Size = %10d, MegaMultReductions/Second = %10.2lf\n", SIZE, megaMultsPerSecond );

	// copy result from the device to the host:

	checkCudaErrors(cudaMemcpy( hC, dC, (SIZE/BLOCKSIZE)*sizeof(float), cudaMemcpyDeviceToHost ));

	// clean up memory:
	delete [ ] hA;
	delete [ ] hB;
	delete [ ] hC;

	checkCudaErrors(cudaFree( dA ));
	checkCudaErrors(cudaFree( dB ));
	checkCudaErrors(cudaFree( dC ));


	return 0;
}

