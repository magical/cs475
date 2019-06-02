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

#include "read.h"

const int BLOCKSIZE = 256; // number of threads per block


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

	// read input data & allocate host memory:

	float *hArray = NULL;
	int size = 0;
	ReadData("signal.txt", &hArray, &size);

	float *hSums = new float[size];

	// allocate device memory:

	float *dArray, *dSums;

	checkCudaErrors(cudaMalloc( reinterpret_cast<void **>(&dArray), 2*size*sizeof(float) ));
	checkCudaErrors(cudaMalloc( reinterpret_cast<void **>(&dSums), size*sizeof(float) ));

	// copy host memory to the device:

	checkCudaErrors(cudaMemcpy( dArray, hArray, 2*size*sizeof(float), cudaMemcpyHostToDevice ));

	// setup the execution parameters:

	dim3 threads(BLOCKSIZE, 1, 1 );
	dim3 grid( size / threads.x, 1, 1 );

	// Create and start timer

	cudaDeviceSynchronize( );

	// allocate CUDA events that we'll use for timing:

	cudaEvent_t start, stop;
	checkCudaErrors(cudaEventCreate( &start ));
	checkCudaErrors(cudaEventCreate( &stop ));

	// record the start event:

	checkCudaErrors(cudaEventRecord( start, NULL ));

	// execute the kernel:

	AutoCorrelate<<< grid, threads >>>( dArray, dSums, size );

	// record the stop event:

	checkCudaErrors(cudaEventRecord( stop, NULL ));

	// wait for the stop event to complete:

	checkCudaErrors(cudaEventSynchronize( stop ));

	float msecTotal = 0.0f;
	checkCudaErrors(cudaEventElapsedTime( &msecTotal, start, stop ));

	// compute and print the performance

	double secondsTotal = 0.001 * (double)msecTotal;
	double multsPerSecond = (float)size / secondsTotal;
	double kiloMultsPerSecond = multsPerSecond / 1e3;
	fprintf( stderr, "Array Size = %10d, KiloAutoCorrelates/Second = %10.2lf\n", size, kiloMultsPerSecond );

	// copy result from the device to the host:

	checkCudaErrors(cudaMemcpy( hSums, dSums, size*sizeof(float), cudaMemcpyDeviceToHost ));

	for (int i = 1; i <= 512; i++ ) {
		printf("%d %f\n", i, hSums[i]);
	}

	// clean up memory:
	delete [ ] hArray;
	delete [ ] hSums;

	checkCudaErrors(cudaFree( dArray ));
	checkCudaErrors(cudaFree( dSums ));


	return 0;
}

