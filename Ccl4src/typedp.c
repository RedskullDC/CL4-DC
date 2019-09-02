#ifndef TYPEDECPL_C
#define TYPEDECPL_C

#include <ctype.h>
#include "DBdefs.h"
#include "cl4.h"

// return number of decimal places based on char FLDtype

int typedp(char a1)
{
	int result;
	switch (a1)
	{
		case 'D':
			result = 6;
			break;
		case 'G':
			result = 2;
			break;
		case 'R':
			result = 4;
			break;
		case 'T':
			result = 7;
			break;
		case '0' ... '9':		// Floating Point vars.  Warning GCC switch extension!
			result = a1 - '0';	// '0' = 48
			break;
		default:				// rest are all Int values
			result = 0;
			break;			
	}
	return result;
}

#endif
