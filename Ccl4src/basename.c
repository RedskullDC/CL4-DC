#ifndef BASENAME_C
#define BASENAME_C

#include "DBdefs.h"
#include "cl4.h"

char *clbasename(char *Buffer)
{
	char *v2;

	while ( (v2 = strrchr(Buffer, '/')) )	// find rightmost '/'
	{
		if ( v2[1] )
		   return v2 + 1;				// if something follows the '/', return that

		if ( v2 == Buffer )
			return Buffer;
		*v2 = 0;
	}
	return Buffer;
}

// rmexten() not called in clenter

#endif
