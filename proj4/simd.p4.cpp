#include "simd.p4.h"

void
SimdMul( float *a, float *b,   float *c,   int len )
{
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	float *atmp = a, *btmp = b, *ctmp = c;

	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		__asm
		(
			".att_syntax\n\t"
			"movups	(%0), %%xmm0\n\t"	// load the first sse register
			"movups	(%1), %%xmm1\n\t"	// load the second sse register
			"mulps	%%xmm1, %%xmm0\n\t"	// do the multiply
			"movups	%%xmm0, (%2)\n\t"	// store the result
			"addq $16, %0\n\t"
			"addq $16, %1\n\t"
			"addq $16, %2\n\t"
			: /* outputs */ "+r" (atmp), "+r" (btmp), "+r" (ctmp), "=m" (*ctmp)
			: /* inputs */ "m" (*atmp), "m" (*btmp)
			: /* clobbers */ "xmm0", "xmm1"
		);
	}

	for( int i = limit; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}

float
SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;

	{
		float *atmp = a, *btmp = b;
		register __int128 tmp __asm ("xmm2") = 0;
		for (int i = 0; i < limit; i += SSE_WIDTH) {
			__asm (
				".att_syntax\n\t"
				"movups	(%0), %%xmm0\n\t"	// load the first sse register
				"movups	(%1), %%xmm1\n\t"	// load the second sse register
				"mulps	%%xmm1, %%xmm0\n\t"	// do the multiply
				"addps	%%xmm0, %2\n\t"	// do the add
				"addq $16, %0\n\t"
				"addq $16, %1\n\t"
				: /* outputs */ "+r" (atmp), "+r" (btmp), "+x" (tmp)
				: /* inputs */ "m" (*atmp), "m" (*btmp)
				: /* clobbers */ "xmm0", "xmm1"
			);
		}
		__asm ("movups %1,%0" : "=m" (sum) : "x" (tmp) : /*no clobbers*/);
	}

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}


void
NonSimdMul( float *a, float *b, float *c, int len )
{
	for( int i = 0; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}

float
NonSimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };

	for( int i = 0; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0];
}
