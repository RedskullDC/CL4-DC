#ifndef ALLOC_C
#define ALLOC_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <string.h>		// for void *memset(void *s, int c, size_t n);
#include "DBdefs.h"
#include "cl4.h"

void* alloc(size_t size, void* vector)
{
	void **buff;
	char s[80];
	
	if ( size < sizeof(void*) )			// minimum size required to store vector
		size = sizeof(void*);			// 4 on X86, 8 on X64
	
	buff = malloc(size);
	if ( !buff )
	{
		sprintf(s, "out of heap (wanted %u bytes)", (unsigned int)size);
		errexit(s, 0);
	}

	memset(buff, 0, size);

	// GNU Linux malloc() returns blocks aligned to longword addresses.

	*buff = vector;		// init value // 4 on X86, 8 on X64
	return buff;
}

void* nalloc(size_t size, void* vector)
{
	void **buff;

	if ( size < sizeof(void*) )			// minimum size required to store vector
		size = sizeof(void*);			// 4 on X86, 8 on X64
	
	buff = malloc(size);
	if ( buff )
	{
		// GNU Linux malloc() returns blocks aligned to longword addresses.
		memset(buff, 0, size);		// clears the allocated memory area to ZERO
		*buff = vector;		// 4 on X86, 8 on X64
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
