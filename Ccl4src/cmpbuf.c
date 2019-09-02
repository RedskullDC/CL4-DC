#ifndef CMPBUF_C
#define CMPBUF_C

#include "DBdefs.h"
#include "cl4.h"

bool cmpbuf(char *a1, char *a2, int a3)
{

	//printf("cmpbuf(%s, %s, %d)\n",a1,a2,a3);
	do
	{
		if ( --a3 == -1 )
			break;
	}
	while ( *a1++ == *a2++ );

	//printf("cmpbuf#2 v5 = %d, v5>>31 = %d\n",v5, v5 >> 31);
	return (a3 < 0);		// Returns true if match all bytes == TRUE
}
#endif
