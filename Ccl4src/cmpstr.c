#ifndef CMPSTR_C
#define CMPSTR_C

#include "DBdefs.h"
#include "cl4.h"

bool cmpstr(char *v2, char *v3)
{
	bool Result;

	//printf("cmpstr(v2 = %s, v3 = %s)\n" ,v2, v3);

	Result = *v2 == 0;	// default response if !*v3.
	
	while ( *v3 )
	{
		if ( *v2++ != *v3++ )
			return false;
		Result = *v2 == 0;
	}
	return Result;
}

#endif
