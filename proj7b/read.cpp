#include <stdlib.h>
#include <stdio.h>

void ReadData(const char* filename, float **arrayp, int *sizep) {
	FILE *fp = fopen( filename, "r" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open file '%s'\n", filename );
		exit( 1 );
	}
	int size;
	fscanf( fp, "%d", &size );
	float *array = new float[ 2*size ];
	for( int i = 0; i < size; i++ )
	{
		fscanf( fp, "%f", &array[i] );
	}
	fclose( fp );
	// duplicate the array
	for( int i = 0; i < size; i++ )
	{
		array[i+size] = array[i];
	}
	*arrayp = array;
	*sizep = size;
}
