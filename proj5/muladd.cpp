// 1. Program header

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <omp.h>

#include "CL/cl.h"
#include "CL/cl_platform.h"


#ifndef NMB
#define	NMB			64
#endif

#define NUM_ELEMENTS		NMB*1024*1024

#ifndef LOCAL_SIZE
#define	LOCAL_SIZE		64
#endif

#define	NUM_WORK_GROUPS		NUM_ELEMENTS/LOCAL_SIZE

const char *			CL_FILE_NAME = { "muladd.cl" };
const float			TOL = 0.0001f;

void				Wait( cl_command_queue );
int				LookAtTheBits( float );

void die(const char* msg) {
	fprintf( stderr, "%s", msg );
	exit(1);
}

bool endswith(const char* s, char suffix) {
	int i;
	if (s[0] == '\0') {
		return false;
	}
	for (i = 1; s[i] != '\0'; i++) {
		/* find the end of s */
	}
	return s[i-1] == suffix;
}

int
main( int argc, char *argv[ ] )
{
	// 0. read program arguments

	ssize_t numElements = NUM_ELEMENTS;
	ssize_t localSize = LOCAL_SIZE;

	if (argc > 2) {
		unsigned int v = atoi(argv[2]);
		if (v > 0) {
			// round up to a power of 2
			// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;
			v |= v >> 16;
			v++;

			localSize = v;
		}
	}
	if (argc > 1) {
		int v = atoi(argv[1]);
		if (endswith(argv[1], 'M')) {
			numElements = (ssize_t)v << 20; // mebibytes
		} else if (endswith(argv[1], 'K')) {
			numElements = (ssize_t)v << 10; // kibibytes
		} else {
			numElements = (ssize_t)v;
		}
		// round up to a multiple of localSize
		numElements += (-numElements)&(localSize-1);
	}

	ssize_t numWorkgroups = numElements/localSize;

	// see if we can even open the opencl kernel program
	// (no point going on if we can't):

	FILE *fp;
#ifdef WIN32
	errno_t err = fopen_s( &fp, CL_FILE_NAME, "r" );
	if( err != 0 )
#else
	fp = fopen( CL_FILE_NAME, "r" );
	if( fp == NULL )
#endif
	{
		fprintf( stderr, "Cannot open OpenCL source file '%s'\n", CL_FILE_NAME );
		return 1;
	}

	cl_int status;		// returned status from opencl calls
				// test against CL_SUCCESS

	// get the platform id:

	cl_platform_id platform;
	status = clGetPlatformIDs( 1, &platform, NULL );
	if( status != CL_SUCCESS )
		die( "clGetPlatformIDs failed (2)\n" );

	// get the device id:

	// TODO: change back to GPU instead of ALL
	cl_device_id device;
	status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL );
	if( status != CL_SUCCESS )
		die( "clGetDeviceIDs failed (2)\n" );

	// 2. allocate the host memory buffers:

	float *hA = new float[ numElements ];
	float *hB = new float[ numElements ];
	float *hC = new float[ numElements ];
	float *hD = new float[ numElements ];

	// fill the host memory buffers:

	for( int i = 0; i < numElements; i++ )
	{
		hA[i] = hB[i] = hC[i] = (float) sqrt(  (double)i  );
	}

	size_t dataSize = numElements * sizeof(float);

	// 3. create an opencl context:

	cl_context context = clCreateContext( NULL, 1, &device, NULL, NULL, &status );
	if( status != CL_SUCCESS )
		die( "clCreateContext failed\n" );

	// 4. create an opencl command queue:

	cl_command_queue cmdQueue = clCreateCommandQueue( context, device, 0, &status );
	if( status != CL_SUCCESS )
		die( "clCreateCommandQueue failed\n" );

	// 5. allocate the device memory buffers:

	cl_mem dA = clCreateBuffer( context, CL_MEM_READ_ONLY,  dataSize, NULL, &status );
	if( status != CL_SUCCESS )
		die( "clCreateBuffer failed (1)\n" );

	cl_mem dB = clCreateBuffer( context, CL_MEM_READ_ONLY,  dataSize, NULL, &status );
	if( status != CL_SUCCESS )
		die( "clCreateBuffer failed (2)\n" );

	cl_mem dC = clCreateBuffer( context, CL_MEM_READ_ONLY, dataSize, NULL, &status );
	if( status != CL_SUCCESS )
		die( "clCreateBuffer failed (3)\n" );

	cl_mem dD = clCreateBuffer( context, CL_MEM_WRITE_ONLY, dataSize, NULL, &status );
	if( status != CL_SUCCESS )
		die( "clCreateBuffer failed (4)\n" );

	// 6. enqueue the 2 commands to write the data from the host buffers to the device buffers:

	status = clEnqueueWriteBuffer( cmdQueue, dA, CL_FALSE, 0, dataSize, hA, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		die( "clEnqueueWriteBuffer failed (1)\n" );

	status = clEnqueueWriteBuffer( cmdQueue, dB, CL_FALSE, 0, dataSize, hB, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		die( "clEnqueueWriteBuffer failed (2)\n" );

	status = clEnqueueWriteBuffer( cmdQueue, dC, CL_FALSE, 0, dataSize, hC, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		die( "clEnqueueWriteBuffer failed (2)\n" );

	Wait( cmdQueue );

	// 7. read the kernel code from a file:

	fseek( fp, 0, SEEK_END );
	size_t fileSize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	char *clProgramText = new char[ fileSize+1 ];		// leave room for '\0'
	size_t n = fread( clProgramText, 1, fileSize, fp );
	clProgramText[fileSize] = '\0';
	fclose( fp );
	if( n != fileSize )
		fprintf( stderr, "Expected to read %zd bytes read from '%s' -- actually read %zd.\n", fileSize, CL_FILE_NAME, n );

	// create the text for the kernel program:

	char *strings[1];
	strings[0] = clProgramText;
	cl_program program = clCreateProgramWithSource( context, 1, (const char **)strings, NULL, &status );
	if( status != CL_SUCCESS )
		die( "clCreateProgramWithSource failed\n" );
	delete [ ] clProgramText;

	// 8. compile and link the kernel code:

	const char *options = "";
	status = clBuildProgram( program, 1, &device, options, NULL, NULL );
	if( status != CL_SUCCESS )
	{
		size_t size;
		clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size );
		cl_char *log = new cl_char[ size ];
		clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG, size, log, NULL );
		fprintf( stderr, "clBuildProgram failed:\n%s\n", log );
		delete [ ] log;
	}

	// 9. create the kernel object:

	cl_kernel kernel = clCreateKernel( program, "ArrayMultAdd", &status );
	if( status != CL_SUCCESS )
		die( "clCreateKernel failed\n" );

	// 10. setup the arguments to the kernel object:

	status = clSetKernelArg( kernel, 0, sizeof(cl_mem), &dA );
	if( status != CL_SUCCESS )
		die( "clSetKernelArg failed (1)\n" );

	status = clSetKernelArg( kernel, 1, sizeof(cl_mem), &dB );
	if( status != CL_SUCCESS )
		die( "clSetKernelArg failed (2)\n" );

	status = clSetKernelArg( kernel, 2, sizeof(cl_mem), &dC );
	if( status != CL_SUCCESS )
		die( "clSetKernelArg failed (3)\n" );

	status = clSetKernelArg( kernel, 3, sizeof(cl_mem), &dD );
	if( status != CL_SUCCESS )
		die( "clSetKernelArg failed (4)\n" );

	// 11. enqueue the kernel object for execution:

	size_t globalWorkSize[3] = { (size_t)numElements, 1, 1 };
	size_t localWorkSize[3]  = { (size_t)localSize,   1, 1 };

	Wait( cmdQueue );
	double time0 = omp_get_wtime( );

	time0 = omp_get_wtime( );

	status = clEnqueueNDRangeKernel( cmdQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL );
	if( status != CL_SUCCESS ) {
		fprintf( stderr, "clEnqueueNDRangeKernel failed: %d\n", status );
		exit(1);
	}

	Wait( cmdQueue );
	double time1 = omp_get_wtime( );

	// 12. read the results buffer back from the device to the host:

	status = clEnqueueReadBuffer( cmdQueue, dD, CL_TRUE, 0, dataSize, hD, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		die( "clEnqueueReadBuffer failed\n" );

	// did it work?

	for( int i = 0; i < numElements; i++ )
	{
		float expected = (double)hA[i] * hB[i] + hC[i];
		if( fabs( hD[i] - expected ) > TOL )
		{
			fprintf( stderr, "%4d: %13.6f * %13.6f + %13.6f wrongly produced %13.6f instead of %13.6f (%13.8f)\n",
				i, hA[i], hB[i], hC[i], hD[i], expected, fabs(hD[i]-expected) );
			fprintf( stderr, "%4d:    0x%08x *    0x%08x +    0x%08x wrongly produced    0x%08x instead of    0x%08x\n",
				i, LookAtTheBits(hA[i]), LookAtTheBits(hB[i]), LookAtTheBits(hC[i]), LookAtTheBits(hD[i]), LookAtTheBits(expected) );
		}
	}

	printf( "%8zd\t%4zd\t%10zd\t%10.3lf GigaMultsPerSecond\n",
		numElements, localSize, numWorkgroups, (double)numElements/(time1-time0)/1000000000. );

#ifdef WIN32
	Sleep( 2000 );
#endif


	// 13. clean everything up:

	clReleaseKernel(        kernel   );
	clReleaseProgram(       program  );
	clReleaseCommandQueue(  cmdQueue );
	clReleaseMemObject(     dA  );
	clReleaseMemObject(     dB  );
	clReleaseMemObject(     dC  );
	clReleaseMemObject(     dD  );

	delete [ ] hA;
	delete [ ] hB;
	delete [ ] hC;
	delete [ ] hD;

	return 0;
}


int
LookAtTheBits( float fp )
{
	int *ip = (int *)&fp;
	return *ip;
}


// wait until all queued tasks have taken place:

void
Wait( cl_command_queue queue )
{
      cl_event wait;
      cl_int      status;

      status = clEnqueueMarker( queue, &wait );
      if( status != CL_SUCCESS )
	      die( "Wait: clEnqueueMarker failed\n" );

      status = clWaitForEvents( 1, &wait );
      if( status != CL_SUCCESS )
	      die( "Wait: clWaitForEvents failed\n" );
}
