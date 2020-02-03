#ifndef RCHECK_C
#define RCHECK_C

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

void putmesg(char *Dest, unsigned int MaxLen, char *FormatStr, char *Param1, char *Param2)
{
	char	*v8;
	char	*v10;
	char	v9;

	while ( *FormatStr && MaxLen > 1 )
    {
		if (*FormatStr == '$')	// looking for $1 or $2 sequence here
		{
			v8 = FormatStr + 1;
			v9 = *v8;			// what is the next char?
			
			switch (v9)
			{
			case '$':			// '$$' results in just one $ in the output
			    *Dest++ = v9;
				break;

			case '1':			// found a $1 or $2 sequence
			case '2':
			    v10 = (v9 == '1') ? Param1 : Param2;
				for ( ; *v10 && MaxLen > 1; --MaxLen ) // copy param into dest buffer
			        *Dest++ = *v10++;
				break;

			default :			// everything else just copy verbatim
				*Dest++ = '$';
				--MaxLen;
				if ( MaxLen )
				    *Dest++ = v9;
				break;
			}
			FormatStr = v8 + 1;
		}
		else
        {
            *Dest++ = *FormatStr++;
            --MaxLen;
        }
    }
    *Dest = 0;
}

bool rcheck(XTAB *xtab, FLDdesc *fld, int SkipChk)		// return true, range ok.  false range error
{
    RATAB	*ratb;
	ENTAB	*entb;
	FLDdesc *v11;
	FLDdesc *v14;
	
    char	*v3;
	char	*v4;
	char	*Param1;
	char	*Param2;
	char	*a2a;
	char	*v35;
	    
	double	testVal;
	double	rangeHigh;
	double	rangeLow;
	
    int		DecPlaces;
	int		RangeType;
	short	SIGN = 0;		// used to indicate sign of result required: Pos,Neg
	
	bool	RangeErr;
	bool	Required;
	
    FLDdesc fld1;
    char	v44[32];
    char	Dest[32];
    char	v42[1008];
    char	v45[1008];
    char	v46[1008];
    char	result[1008];
    char	format[1008];
    char	v32;
	
    testVal		= 0.0;
    rangeHigh	= 0.0;
    rangeLow	= 0.0;
    
	v4		= 0;
    v35		= 0;
    v32		= 0;
    a2a		= 0;
    
//	range from to expr # numbers, dates or times
//	range positive # numbers
//	range negative # numbers
//	range string expr # characters
//	range name expr # a valid cl name

	//printf("rcheck 117 - xtab->Flags = x%04X, fld->FLDstat = x%04X, SkipChk = %d\n",(unsigned short)xtab->Flags,(unsigned short)fld->FLDstat,SkipChk);

	//if ( SkipChk || !(xtab->Flags & 0x0400) && fld->FLDstat & 0x0004 )
	if ( SkipChk || !(xtab->Flags & xtb_REQUIRED) && fld->FLDstat & fld_ZERO )
        return 1;					// exit success.
    
	//if ( xtab->Flags & 0x110 || !xtab->RangeID )
	if ( xtab->Flags & (xtb_JOIN | xtb_NOJOIN) || !xtab->RangeID )
    {
        //testVal = 0.0;	all set to 0.0 above
        //rangeHigh = 0.0;
        //rangeLow = 0.0;
        ratb = 0;
    }
	else
	{
	    ratb = &raarr.TableAddr[xtab->RangeID - 1];
	    RangeType = ratb->RangeType;
		//printf("rcheck 140 - RangeType = %d\n",RangeType);
		switch (RangeType)
		{
		case 1:									// POSITIVE
		case 2:									// NEGATIVE
	        SIGN = (RangeType == 1) ? 1 : -1 ;
	        if ( xtab->Flags & xtb_CHAR )		// expect string value
	            break;
			testVal = *(double *)fld->FLDdata;
			break;

		case 5:									// MATCH
	        SIGN = 0;
	        entb = ENARR(ratb->RangeFrom);
			assert(entb->entype == 0x02 && entb->Enun.Enop.Enoper == 0x6C00);	// 0x6C00 = match()

	        a2a = evalstr(ENARR(entb->enleft), v42);
			break;

		case 6:									// NAME	removed in cl4.4.4 ??
			break;

		//case 3:								// range FROM TO    eg: range 100 300 'must be between $1 and $2'
		//case 4:								// range 'one,two,three'
		default:
			SIGN = 0;
			v11 = getftf(ENARR(ratb->RangeFrom), 1, &fld1);
			v4 = (char *)v11->FLDdata;
			if ( !(xtab->Flags & xtb_CHAR) )		// expect string value
			{
			    if ( v11->FLDtype == 'C' && xtab->Flags & xtb_DATE )
			    {
			        cdbcpystr(Dest, v4, 0);
			        rangeLow = (double)clgetdate(Dest);
			    }
			    else
			        rangeLow = clround(*(double *)v4, v11->FLDtype);
			
				v14 = getftf(ENARR(ratb->RangeTo), 1, &fld1);
				if ( v14->FLDtype == 'C' && xtab->Flags & xtb_DATE )
				{
				    cdbcpystr(Dest, (char *)v14->FLDdata, 0);
				    rangeHigh = (double)clgetdate(Dest);
				}
				else
					rangeHigh = clround(*(double *)v14->FLDdata, v14->FLDtype);

				testVal = *(double *)fld->FLDdata;
				break;
			}
		}
	}
    
	RangeErr = false;
    Required = false;

	if ( xtab->Flags & xtb_REQUIRED )
        Required = (fld->FLDstat & fld_ZERO) ? true : false;

	if ( ratb )        // else goto LABEL_82;
	{
		if ( fld->FLDtype != 'C' )						// Numeric types
		{
			if (SIGN > 0)			// POSITIVE
				RangeErr = ( testVal <= 0.0 );
			
			else if (SIGN < 0)		// NEGATIVE
				RangeErr = ( testVal >= 0.0 );
			
			else	// SIGN == 0		FROM <> TO type range
				RangeErr = ( rangeLow > testVal || testVal > rangeHigh );
		}
		else if ( SIGN )	// pos/neg check on a string value. Try and interpret as numeric value
		{
			testVal = strtod((const char *)fld->FLDdata, 0);
			if ( SIGN > 0 )			// POSITIVE
				RangeErr = ( testVal <= 0.0 );
			else	// SIGN < 0		// NEGATIVE
				RangeErr = ( testVal >= 0.0 );
		}
		else		// string types
		{
			if ( ratb->RangeType == 6 )					// valid cl filename check
			{
			    v32 = isfname((char *)fld->FLDdata);
			    if ( !v32 )
			        goto LABEL_82;
			}
			else
			{
				if ( !a2a )
				{
				    v35 = v4;
				    RangeErr = true;
				    do
				    {
				        v3 = v44;
						while (*v4)		// split range string on commas
						{
							*v3++ = *v4++;
							if (*v4 == ',')
							{
								v4++;
								break;
							}
						}
			            *v3 = 0;

			            if ( !v44[0] )	// got to end of range string with no match!!
			                break;
			            
						if ( !compare((char *)fld->FLDdata, v44) )
			                RangeErr = false;
			        }
			        while ( RangeErr );
			        goto LABEL_82;
			    }
			    if ( smatch((char *)fld->FLDdata, a2a) > 0 )
			        goto LABEL_82;
			}
			RangeErr = true;
		}
	}

//==============================================

LABEL_82:

	if ( !Required && !RangeErr)
		return 1;					// exit_success

//-----------------------------------
    if ( ratb )
    {
        Param1 = "";
        Param2 = "";
        if ( SIGN > 0 )
            sprintf(result, "must be POSITIVE");
        
		else if ( SIGN < 0 )
            sprintf(result, "must be NEGATIVE");

		else if ( xtab->Flags & xtb_NAME )
        {
            sprintf(result, "invalid NAME character [%c]", v32);
            bncbuf_0[0] = v32;
            bncbuf_0[1] = 0;
            Param1 = bncbuf_0;
        }
		else
		{
			switch (fld->FLDtype)
			{
			case 'C':
				if ( a2a )
				{
				    sprintf(result, "must match: '%s'", a2a);
				    Param1 = a2a;
				}
				else
				{
				    sprintf(result, "allowed: '%s'", v35);
				    Param1 = v35;
				}
				break;

			case 'D':
				sprintf(result, "range: %s to %s", makedate(v46, rangeLow), makedate(v45, rangeHigh));
				Param1 = v46;
				Param2 = v45;
				break;

			case 'T':
				sprintf(result, "range: %s to %s", maketime(v46, rangeLow), maketime(v45, rangeHigh));
				Param1 = v46;
				Param2 = v45;
				break;

			default:
				DecPlaces = (short)typedp(fld->FLDtype);
				Param1 = v46;
				sprintf(v46, "%.*f", DecPlaces, rangeLow);
				Param2 = v45;
				sprintf(v45, "%.*f", DecPlaces, rangeHigh);
				sprintf(result, "range: %s to %s", v46, v45);
				break;
			}
        }
        if ( ratb->StringOffset )					// If program had a custom message, use that in preference to the defaults above.
            putmesg(result, 1001u, &strarr.StringTable[ratb->StringOffset - 1], Param1, Param2);
    }
	else
	{
		result[0] = 0;
	}

    if ( xtab->Flags & xtb_JOIN )
        cdbcpystr(result, result, " (try '*' or '+')", 0);
    
	if ( xtab->onesc )
        cdbcpystr(result, result, " (try ESC ' ')", 0);

	if ( Required )
    {
        cdbcpystr(format, "required", 0);
        if ( result[0] )
            cdbcpystr(format, format, " - ", result, 0);
	    prerr(format, 0, 1);
    }
    else
    {
	    prerr(result, 0, 1);
    }
    return 0;
}


#endif

