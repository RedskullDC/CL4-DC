#ifndef FILL_C
#define FILL_C

#include "DBdefs.h"
#include "cl4.h"

int fill(char *Buffer, int count, char value)
{
	int i;
	//printf("fill: x%08X, Count: %d, Value : x%02X [%c]\n",Buffer, count,(unsigned char)value , value);
	// unsigned in printf stops it extending to 4 bytes

	for ( i = count; i; --i )
		*Buffer++ = value;
	return count;
}

#endif
