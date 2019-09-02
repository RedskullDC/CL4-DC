#ifndef CPYBUF_C
#define CPYBUF_C

#include "DBdefs.h"
#include "cl4.h"
int cpybuf(void *DEST, void *SOURCE, int Size)
{
	int i;

	for ( i = Size; i; --i )
		*(char*)DEST++ = *(char*)SOURCE++;
	return Size;
}

#endif
