// Monte carlo beam simulation

// System includes
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

// CUDA runtime
#include <cuda_runtime.h>

// Helper functions and utilities to work with CUDA
#include "helper_functions.h"
#include "helper_cuda.h"

// ranges for the random numbers:
const float XCMIN =	 0.0;
const float XCMAX =	+2.0;
const float YCMIN =	 0.0;
const float YCMAX =	 2.0;
const float RMIN  =	 0.5;
const float RMAX  =	 2.0;


#ifndef BLOCKSIZE
#define BLOCKSIZE		32		// number of threads per block
#endif

#ifndef SIZE
#define SIZE			1*1024*1024	// array size
#endif

#ifndef TOLERANCE
#define TOLERANCE		0.00001f	// tolerance to relative error
#endif

float Ranf( float, float );

// array multiplication (CUDA Kernel) on the device: C = A * B

__global__  void ArrayMul( float *A, float *B, float *C )
{
	__shared__ float prods[BLOCKSIZE];

	unsigned int numItems = blockDim.x;
	unsigned int tnum = threadIdx.x;
	unsigned int wgNum = blockIdx.x;
	unsigned int gid = blockIdx.x*blockDim.x + threadIdx.x;

	prods[tnum] = A[gid] * B[gid];

	for (int offset = 1; offset < numItems; offset *= 2)
	{
		int mask = 2 * offset - 1;
		__syncthreads();
		if ((tnum & mask) == 0)
		{
			prods[tnum] += prods[tnum + offset];
		}
	}

	__syncthreads();
	if (tnum == 0)
		C[wgNum] = prods[0];
}

// monte carlo
__global__ void MonteCarlo( float *xcs, float *ycs, float *rs, int *hits )
{
	unsigned int globalThreadId = blockIdx.x * blockDim.x + threadIdx.x;

	// randomize the location and radius of the circle:
	float xc = xcs[globalThreadId];
	float yc = ycs[globalThreadId];
	float  r =  rs[globalThreadId];

	// solve for the intersection using the quadratic formula:
	float a = 2.;
	float b = -2.*( xc + yc );
	float c = xc*xc + yc*yc - r*r;
	float d = b*b - 4.*a*c;

	// If d is less than 0., then the circle was completely missed. (Case A) Doesn't hit.
	if (d < 0) {
		return;
	}

	// hits the circle:
	// get the first intersection:
	d = sqrt( d );
	float t1 = (-b + d ) / ( 2.*a );	// time to intersect the circle
	float t2 = (-b - d ) / ( 2.*a );	// time to intersect the circle
	float tmin = t1 < t2 ? t1 : t2;		// only care about the first intersection

	// If tmin is less than 0., then the circle completely engulfs the laser pointer. (Case B) Doesn't hit.
	if (tmin < 0) {
		return;
	}

	// where does it intersect the circle?
	float xcir = tmin;
	float ycir = tmin;

	// get the unitized normal vector at the point of intersection:
	float nx = xcir - xc;
	float ny = ycir - yc;
	float n = sqrt( nx*nx + ny*ny );
	nx /= n;	// unit vector
	ny /= n;	// unit vector

	// get the unitized incoming vector:
	float inx = xcir - 0.;
	float iny = ycir - 0.;
	float in = sqrt( inx*inx + iny*iny );
	inx /= in;	// unit vector
	iny /= in;	// unit vector

	// get the outgoing (bounced) vector:
	float dot = inx*nx + iny*ny;
	float outx = inx - 2.*nx*dot;	// angle of reflection = angle of incidence`
	float outy = iny - 2.*ny*dot;	// angle of reflection = angle of incidence`

	// find out if it hits the infinite plate:
	float t = ( 0. - ycir ) / outy;

	// If t is less than 0., then the reflected beam went up instead of down. Doesn't hit.
	if (t < 0) {
		return;
	}

	// Otherwise, this beam hit the infinite plate. (Case D)
	hits[globalThreadId] = 1;

}

float Ranf( float low, float high )
{
	float r = (float) rand();		// 0 - RAND_MAX
	float t = r  / (float) RAND_MAX;	// 0. - 1.

	return low  +  t * ( high - low );
}

// main program:

int
main( int argc, char* argv[ ] )
{
	int dev = findCudaDevice(argc, (const char **)argv);

	// allocate host memory:
	int numtrials = SIZE;

	// better to define these here so that the rand() calls don't get into the thread timing:
	float *xcs = new float[ numtrials ];
	float *ycs = new float[ numtrials ];
	float * rs = new float[ numtrials ];
	int * hits = new int[ numtrials ]; // TODO: int8_t?

	// fill the random-value arrays:
	for( int n = 0; n < numtrials; n++ )
	{
		xcs[n] = Ranf( XCMIN, XCMAX );
		ycs[n] = Ranf( YCMIN, YCMAX );
		rs[n] = Ranf(  RMIN,  RMAX );
		hits[n] = 0;
	}

	// allocate device memory:

	float *dXcs, *dYcs, *dRs;
	int *dHits;

	//dim3 dimsA( SIZE, 1, 1 );
	//dim3 dimsB( SIZE, 1, 1 );
	//dim3 dimsC( SIZE/BLOCKSIZE, 1, 1 );

	//__shared__ float prods[SIZE/BLOCKSIZE];


	cudaError_t status;
	status = cudaMalloc( reinterpret_cast<void **>(&dXcs), numtrials*sizeof(float) );
	checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&dYcs), numtrials*sizeof(float) );
	checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&dRs), numtrials*sizeof(float) );
	checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&dHits), numtrials*sizeof(int) );
	checkCudaErrors( status );


	// copy host memory to the device:

	status = cudaMemcpy( dXcs, xcs, numtrials*sizeof(float), cudaMemcpyHostToDevice );
	checkCudaErrors( status );
	status = cudaMemcpy( dYcs, ycs, numtrials*sizeof(float), cudaMemcpyHostToDevice );
	checkCudaErrors( status );
	status = cudaMemcpy( dRs, rs, numtrials*sizeof(float), cudaMemcpyHostToDevice );
	checkCudaErrors( status );

	// setup the execution parameters:

	dim3 threads(BLOCKSIZE, 1, 1 );
	dim3 grid( numtrials / threads.x, 1, 1 );

	// Create and start timer

	cudaDeviceSynchronize( );

	// allocate CUDA events that we'll use for timing:

	cudaEvent_t start, stop;
	status = cudaEventCreate( &start );
		checkCudaErrors( status );
	status = cudaEventCreate( &stop );
		checkCudaErrors( status );

	// record the start event:

	status = cudaEventRecord( start, NULL );
		checkCudaErrors( status );

	// execute the kernel:

	MonteCarlo<<< grid, threads >>>( dXcs, dYcs, dRs, dHits );

	// record the stop event:

	status = cudaEventRecord( stop, NULL );
		checkCudaErrors( status );

	// wait for the stop event to complete:

	status = cudaEventSynchronize( stop );
		checkCudaErrors( status );

	float msecTotal = 0.0f;
	status = cudaEventElapsedTime( &msecTotal, start, stop );
		checkCudaErrors( status );

	// compute and print the performance

	double secondsTotal = 0.001 * (double)msecTotal;
	double multsPerSecond = (float)numtrials / secondsTotal;
	double megaMultsPerSecond = multsPerSecond / 1000000.;
	fprintf( stderr, "Array Size = %10d, MegaMultReductions/Second = %10.2lf\n", numtrials, megaMultsPerSecond );

	// copy result from the device to the host:

	status = cudaMemcpy( hits, dHits, numtrials*sizeof(int), cudaMemcpyDeviceToHost );
	checkCudaErrors( status );

	int numHits = 0;
	for (int i = 0; i < numtrials; i++) {
		numHits += hits[i];
	}

	printf("Num hits = %d, hit rate = %.4f\n", numHits, (double)numHits / numtrials);

	/* 
	status = cudaMemcpy( hC, dC, (SIZE/BLOCKSIZE)*sizeof(float), cudaMemcpyDeviceToHost );
		checkCudaErrors( status );

	// check the sum :

	double sum = 0.;
	for(int i = 0; i < SIZE/BLOCKSIZE; i++ )
	{
		//fprintf(stderr, "hC[%6d] = %10.2f\n", i, hC[i]);
		sum += (double)hC[i];
	}
	fprintf( stderr, "\nsum = %10.2lf\n", sum );
	*/

	// clean up memory:
	delete [ ] xcs;
	delete [ ] ycs;
	delete [ ] rs;
	delete [ ] hits;

	status = cudaFree( dXcs ); checkCudaErrors( status );
	status = cudaFree( dYcs ); checkCudaErrors( status );
	status = cudaFree( dRs ); checkCudaErrors( status );
	status = cudaFree( dHits ); checkCudaErrors( status );


	return 0;
}

