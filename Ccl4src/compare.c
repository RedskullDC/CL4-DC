#ifndef COMPARE_C
#define COMPARE_C

#include <ctype.h>	// for isdigit() etc.
#include "DBdefs.h"
#include "cl4.h"

// Not public in libcl4
short numsize(char *a1)
{
	short v2;

	v2 = 0;
	while ( *a1 && isdigit(*a1))
	{
		v2++;
		a1++;
	}
	return v2;
}

int compare(char *a1, char *a2)
{
	char *v2;
	
	signed int result;
	int v7;
	signed int v8;
	signed int v11;
	signed int v12;
	signed int v13;
	
	char v14;
	char v15;

	//printf("compare(a1 = %s, a2 = %s)\n",a1,a2);

	v2 = a2;
	v12 = 0;

	// wildcard in second string
	// Only possible if 'like' is the operator?
	if ( getwpos(a2, strlen(a2)) >= 0 )	// Found a wildcard char, do wild match instead
		return owildmat(a1, a2, 8);		// 0x08 means case insensitive match
	
	v13 = 0;
	while ( *a1 && *v2 )
	{
		v15 = *a1;
		v14 = *v2;
		if ( *v2 == '*' && !v2[1])	// Reached last char in a2, it's a wildcard return 0 == MATCH.
			return 0;
		
		//printf("compare() v12 = %d , v15 = %C v14 = %C \n",v12, v15,v14);	
		if ( v15 != v14 && v14 != '?' )	// '?' matches any in test string
		{
			if ( isdigit(v15) && isdigit(v14))
			{
				// Both are digits. look ahead to see if numbers are different lengths
				if ( !v13 )
				{
					v8 = numsize(a1) - numsize(v2);
					if ( v8 )
						return v8;	// Different lengths, return the difference.
				}
				v13 = 1;
			}
			else
				v13 = 0;
			//-----------------------
			if (isupper(v15))
				v15 = tolower(v15);
			else if ( v12 && isdigit(v15))
				v15 |= 0x80u;
			//-----------------------
			if (isupper(v14))
				v14 = tolower(v14);
			else if ( v12 && isdigit(v14))
				v14 |= 0x80u;
			//-----------------------
				
			v8 = v15 - v14;
		//printf("compare#76 v8 = %d , v15 = [x%02X] v14 = [x%02X]\n",v8,v15,v14);	
			if ( v15 != v14 )
				return v8;
		}

		v12 = 0;
		if (isdigit(v15) && isdigit(v14))
			v12 = 1;	// Used next time around to indicate prev chars were numbers

		++a1;
		++v2;
	}

	if ( *a1 == *v2 || *v2 == '*' )
		return 0;
	
	if ( *a1 )
		result = 1;
	else
		result = -1;
	return result;
}

#endif
