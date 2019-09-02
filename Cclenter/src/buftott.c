#ifndef BUFTOTT_C
#define BUFTOTT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

char *buftott(char *Buff, FLDdesc *fld)
{
	char	*bufst;
	
	signed int v12;
	short	Maxlen;
	signed short v13;
	
	char	Separator;
	char	v4;
	char	FLDtype;

	char	*x;

	double	v14;
	char	src[64];					// overkill for Date/Time values, but no buffer overrun checks!
	
    if ( TAB != ',' || *Buff != '"' && *Buff != '\'' )
        Separator = TAB;
    else
        Separator = *Buff++;			// Separator == " or '  , look for a matching quote at end.

//--------------------------
    v4 = 0;
    bufst = Buff;						// save string start for trim operation below
    
	while ( *Buff )
    {
        if ( *Buff == '\n' )			// newline char or \0 terminates input
		{
			*Buff++ = 0;
			break;
		}
        else
        {
			if ( v4 == '\\' )			// if *last* loop was a \, don't treat this char as a seperator
				v4 = 0;
			else if ( *Buff == Separator )
			{
				if ( *Buff )			// possibly ZERO if null string passed in
					*Buff++ = 0;
				break;
			}
			else
				v4 = *Buff;				// if a \ is found, ignore it next loop! 
			
			Buff++;
        }
    }
    
	if ( Separator != TAB && *Buff == TAB )
        *Buff++ = 0;

//--------------------------------------------------    
	
	trim(bufst);		// bufst points at string after seperator chars removed

    FLDtype = fld->FLDtype;
    if ( FLDtype == 'C' )
    {
        Maxlen = lenstr(bufst) + 1;
        if ( Maxlen > fld->FLDlen )
            Maxlen = fld->FLDlen;
        
		cpybuf((char *)fld->FLDdata, bufst, Maxlen);
        if ( *bufst )
        {
			fld->FLDstat &= ~fld_ZERO;						// clear ZERO flag
			fld->FLDstat |= fld_DATA_AVAIL;					// set DATA AVAIL flag
        }
		else	// null string
            fld->FLDstat |= (fld_ZERO|fld_DATA_AVAIL);		// set ZERO, DATA AVAIL flags
    }
    else if ( fld->FLDlen )
    {
		//printf("Fieldtype %C\n",FLDtype);
        switch ( FLDtype )
        {
            case 'N':
                v12 = strtol(bufst, 0, 10);
                v14 = (double)v12;
                break;
            case 'X':						// HEX
				if (x = strchr(bufst, 'x'))		// look for 0x...
				{
					v12 = strtol(x + 1, 0, 16);	// position past the x
					v14 = (double)v12;
				}
				else
					v14 = 0.0;
                break;
            case 'I':
                v13 = strtol(bufst, 0, 10);
                v14 = (double)v13;
                break;
            case 'B':
                v13 = strtol(bufst, 0, 10);
                if ( (unsigned short)(v13 + 127) >= 255u )
                    v13 = 0;
                v14 = (double)v13;
                break;
            case 'D':
                cdbcpystr(src, bufst, 0);
                v12 = clgetdate(src);
                if ( v12 < 0 )
                    v12 = 0;
                v14 = (double)v12;
                break;
            case 'T':
                cdbcpystr(src, bufst, 0);
                v14 = gettime(src);
                if ( v14 < 0.0 )
                    v14 = 0.0;
                break;
            default:
                v14 = strtod(bufst, 0);
                break;
        }
        *(double *)fld->FLDdata = v14;
        
		if ( v14 != 0.0 )
        {
			fld->FLDstat &= ~fld_ZERO;					// clear ZERO flag
			fld->FLDstat |= fld_DATA_AVAIL;				// set DATA AVAIL flag
        }
		else		// zero result
            fld->FLDstat |= (fld_ZERO|fld_DATA_AVAIL);	// set ZERO, DATA AVAIL flags
    }
    return Buff;	// return point after string for easy chaining!
}

#endif

