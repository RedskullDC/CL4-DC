#ifndef ALLOC_C
#define ALLOC_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <string.h>		// for void *memset(void *s, int c, size_t n);
#include "DBdefs.h"
#include "cl4.h"

void* alloc(unsigned int size, void* vector)
{
	char *buff;
	char s[80];
	
	if ( size < 4 )			// minimum size required to store vector
		size = 4;
	
	buff = malloc(size);
	if ( !buff )
	{
		sprintf(s, "out of heap (wanted %u bytes)", size);
		errexit(s, 0);
	}

	memset(buff, 0, size);

	// GNU Linux malloc() returns blocks aligned to longword addresses.

	*(int*)buff = (int)vector;	// init value
	return buff;
}

void* nalloc(unsigned int size, void* vector)
{
	char *buff;

	if ( size < 4 )			// minimum size required to store vector
		size = 4;
	
	buff = malloc(size);
	if ( buff )
	{
		// GNU Linux malloc() returns blocks aligned to longword addresses.
		memset(buff, 0, size);		// clears the allocated memory area to ZERO
		*(int*)buff = (int)vector;
	}
	return buff;
}

void* nfree(void *ptr, void *vector)
{
  if ( ptr )
    free(ptr);
  return vector;
}

#endif
