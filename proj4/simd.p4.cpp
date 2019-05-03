#include "simd.p4.h"

// July 13, 2017: mjb -- changed rbx register to r8
// The convention is that rbx needs to be saved by the callee (i.e., pushed and popped),
// 	but rcx, rdx, r8, and r9 do not
// This fixed the bug that showed up in cs 475/575 project #5 in SQ 2017


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

	__asm
	(
		".att_syntax\n\t"
		"movq    -40(%rbp), %r8\n\t"		// a
		"movq    -48(%rbp), %rcx\n\t"		// b
		"leaq    -32(%rbp), %rdx\n\t"		// &sum[0]
		"movups	 (%rdx), %xmm2\n\t"		// 4 copies of 0. in xmm2
	);

	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		__asm
		(
			".att_syntax\n\t"
			"movups	(%r8), %xmm0\n\t"	// load the first sse register
			"movups	(%rcx), %xmm1\n\t"	// load the second sse register
			"mulps	%xmm1, %xmm0\n\t"	// do the multiply
			"addps	%xmm0, %xmm2\n\t"	// do the add
			"addq $16, %r8\n\t"
			"addq $16, %rcx\n\t"
		);
	}

	__asm
	(
		".att_syntax\n\t"
		"movups	 %xmm2, (%rdx)\n\t"	// copy the sums back to sum[ ]
	);

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
	//int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;

	for( int i = 0; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0];
}