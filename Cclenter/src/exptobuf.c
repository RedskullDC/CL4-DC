#ifndef EXPTOBUF_C
#define EXPTOBUF_C

#include <stdio.h>
#include <stdarg.h>			// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

char *exptobuf(char *buff, ENTAB *entab, unsigned int MaxLen)
{
    double	doubVal;

	FLDdesc fld2;
	char	v39[1008];
	char	Dest[1008];
	char	FLDType;
	
    if ( entab )
    {
		//printf("exptobuf, type = %d, entab->TTno = x%04X\n",entab->entype,(unsigned short)entab->TTno);
		switch (entab->entype)
		{
		case 0x04:							// inline float. Real clcomp doesn't create these!  
            if ( MaxLen )
	            sprintf(buff, "%*.*f", MaxLen, typedp('6'), *(float *)&entab->TTno);
			else
				sprintf(buff, "%.*f", typedp('6'), *(float *)&entab->TTno);
			break;

		case 0x08:							// inline int.
            if ( MaxLen )
	            sprintf(buff, "%*ld", MaxLen, *(int *)&entab->TTno);
			else
				sprintf(buff, "%ld", *(int *)&entab->TTno);
			break;

		case 0x10:							// inline string. 3 chars or less.
			if ( MaxLen )
				sprintf(buff, "%-*.*s", MaxLen,MaxLen, (char *)&entab->TTno);
			else
				cdbcpystr(buff, (char *)&entab->TTno, 0);
			break;

		case 0x02:							// expression or function call?
            if (entab->TTno & 0x0200)		// integer calc flag
			{
				if ( MaxLen )
					sprintf(buff, "%*ld", MaxLen, evalint(entab));
				else
					sprintf(buff, "%ld", evalint(entab));
				break;
			}
			else
            {
                FLDType = getetype(entab);
				//printf("exptobuf Fieldtype %C\n",FLDType);

                if ( FLDType == 'C' )
                {
                    zap(v39, 1001u);
                    evalstr(entab, v39);
                }
                else
					doubVal = clround(evalnum(entab, FLDType), FLDType);

				switch ( FLDType )
                {
                    case 'C':
                        if ( MaxLen )
						    sprintf(buff, "%-*.*s", MaxLen, MaxLen, v39);
						else
							cdbcpystr(buff, v39, 0);
						break;

					case 'D':
                    case 'T':
						if ( doubVal <= 0.0 )
							cdbcpystr(Dest, " ", 0);
						else if ( FLDType == 'D' )
							makedate(Dest, doubVal);
						else
							maketime(Dest, doubVal);
                        
						if ( MaxLen )
 							sprintf(buff, "%-*.*s", MaxLen, MaxLen, Dest);
 						else
							cdbcpystr(buff, Dest, 0);
						break;

					case 'B':
                    case 'I':
						if ( MaxLen )
                            sprintf(buff, "%*d", MaxLen, (int)doubVal);
						else
							sprintf(buff, "%d", (int)doubVal);
						break;

					case 'N':
						if ( MaxLen )
                            sprintf(buff, "%*ld", MaxLen,(int)doubVal);
                        else
                            sprintf(buff, "%ld",(int)doubVal);
						break;
			
					case 'X':				// DC extension! 4 byte HEX value
					case 'P':				// DC extension! 4 byte POINTER value	Shouldn't be reachable?
						//printf("exptobuf Fieldtype %C, maxlen = %d\n",FLDType,MaxLen);
						if ( MaxLen )
							sprintf(buff, "%0*X", MaxLen, (int)doubVal);
						else
							sprintf(buff, "%04X", (int)doubVal);
						break;

					case 'K':		// Key/Sub records have no direct data as such
                    case 'S':
                        *buff = 0;
                        break;

                    default:
                        if ( MaxLen )
                            sprintf(buff, "%*.*f", MaxLen, typedp(FLDType), doubVal);
						else
 							sprintf(buff, "%.*f", typedp(FLDType), doubVal);
				}
            }
			break;

		default:	// entab->entype == 1		variable or field in a table
			return fldtobuf(buff, getftf(entab, 1, &fld2), MaxLen);
			break;
		}
		return buff;
    }
    *buff = 0;	// entab == null, return blank string.
    return buff;
}

#endif
