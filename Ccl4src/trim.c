#ifndef TRIM_C
#define TRIM_C

#include "DBdefs.h"
#include "cl4.h"
#include "ctype.h"	// for isspace()

char * ftrim(char *a1)
{
	short v1; // di@1

	//printf("ftrim(%s)\n",a1);

	v1 = trim_it;
	trim_it = 1;
	trim(a1);
	trim_it = v1;
	return a1;
}

char* trim(char *a1)
{
	char *v1;
	char *v2;
	char v4;

	//printf("trim(%s)\n",a1);

	if ( a1 && *a1 && trim_it != 2 )
	{
		v1 = a1;
		v2 = a1;
		//advance v2 past leading spaces
		while ( isspace(*v2) )
			++v2;
		do
		{
			v4 = *v2++;
			*v1++ = v4;
		}
		while ( v4 );
		// Point v1 to last char in string, then 
		// work back from end. trim trailing spaces.
		for ( v1 -= 2; a1 < v1 && isspace(*v1); --v1 )
			*v1 = 0;
	}
	return a1;
}

//remove_xml_filename() only used by cgi

#endif
