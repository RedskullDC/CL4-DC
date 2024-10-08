#ifndef ECHECKNUM_C
#define ECHECKNUM_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

bool echknum(const char *nptr, char VarType)
{
    double	v3;
	double	MaxVal;
	short	v13;
	char	a2[128];
	
    v13 = (short) clisnumber((char *)nptr) & 0x00FF;
    if ( v13 )
    {
        prerr("invalid numeric [%s]", &v13, 1);
        return false;	// error_exit
    }
    v3 = strtod(nptr, 0);

    switch ( VarType )
    {
        case 'B':
            MaxVal = 127.0;
            break;
        case 'N':
            MaxVal = 2147483647.0;
            break;
        case 'I':
        case 'R':
            MaxVal = 32767.0;
            break;
        case 'F':
            MaxVal = 100000000.0;
            break;
        case '$':
            MaxVal = 1.0e12;
            break;
        case 'G':
            MaxVal = 8388607.0;
            break;
        case 'L':
            MaxVal = 1.0e14;
            break;
        //default:
		case '0' ... '9':
			//printf("echknum: default. MAX_nos[ VarType - '0'] = %G\n", MAX_nos[ VarType - '0']);
            //MaxVal = *((double *)&off_80A4200 + VarType);// this is wrong.
			//MaxVal = 0.0;
			MaxVal = MAX_nos[ VarType - '0'];	// '0' -> '9'
            break;
    }
    if ( v3 > MaxVal || -MaxVal > v3 )	// range check
    {
        sprintf(a2, "%.*f", typedp(VarType), MaxVal);
        prerr("valid range is +/-%s", a2, 1);
        return false;
    }
    return true;	// exit_success. Valid number, and in range
}
#endif
