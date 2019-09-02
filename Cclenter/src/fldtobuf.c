#ifndef FIELD_TO_BUF_C
#define FIELD_TO_BUF_C

#include <sys/types.h>	// for user calls
#include <pwd.h>
#include "DBdefs.h"
#include "cl4.h"

// getshvar() not called in clenter

char *fldtobuf(char *Dest, FLDdesc *fld, unsigned int MaxLen)
{
    int		DecPl;
    double	DoubVal = 0.0; 
    char	Buff[1001];

    if ( fld->FLDtype != 'C' && fld->FLDtype != 'P' && fld->FLDlen )
        DoubVal = *(double *)fld->FLDdata;

	//printf("fldtobuf, type = %C, DoubVal = %G\n",fld->FLDtype,DoubVal);
	switch ( fld->FLDtype )
    {
        case 'C':
            if ( MaxLen )
		        sprintf(Dest, "%-*.*s", MaxLen,MaxLen,(char *)fld->FLDdata);
			else
				cdbcpystr(Dest, (char *)fld->FLDdata, 0);
			break;
//---------------------
        case 'D':
        case 'T':
            if ( DoubVal <= 0.0 )				// error. can't covert to valid date/time
                cdbcpystr(Buff, " ", 0);
            else if ( fld->FLDtype == 'D' )
				makedate(Buff, DoubVal);
            else
				maketime(Buff, DoubVal);

			if ( MaxLen )
            {
				if ( MaxLen <= strlen(Buff) )
					cdbcpystr(Dest,Buff, 0);
				else
					sprintf(Dest, "%-*.*s", MaxLen, MaxLen, Buff);
            }
			else
                cdbcpystr(Dest, trim(Buff), 0);
			break;
//-----------------------
		case 'B':
        case 'I':
            if ( MaxLen )
                sprintf(Dest, "%*d", MaxLen, (int)DoubVal);
			else
				sprintf(Dest, "%d", (int)DoubVal);
			break;
//-------------------------
		case 'N':
            if ( MaxLen )
                sprintf(Dest, "%*ld", MaxLen, (int)DoubVal);
            else
				sprintf(Dest, "%ld", (int)DoubVal);
			break;
//-------------------------
		case 'P':				// DC extension! 4 byte POINTER value
            if ( MaxLen )
                sprintf(Dest, "x%0*X", MaxLen, *(int *)fld->FLDdata);
            else
				sprintf(Dest, "x%04X", *(int *)fld->FLDdata);
			break;
//-------------------------
		case 'X':				// DC extension! 4 byte HEX value
            if ( MaxLen )
                sprintf(Dest, "%0*X", MaxLen, (int)DoubVal);	// leading Zeros
            else
				sprintf(Dest, "%04X", (int)DoubVal);
			break;
//-------------------------
		case 'K':				// Error! S/K types have no printable data
        case 'S':
            *Dest = 0;
			break;
//-------------------------
		default:
            DecPl = typedp(fld->FLDtype);
            if ( MaxLen )
                sprintf(Dest, "%*.*f", MaxLen, DecPl, DoubVal);
			else
				sprintf(Dest, "%.*f", DecPl, DoubVal);
			break;
    }
	return Dest;
}

#endif
