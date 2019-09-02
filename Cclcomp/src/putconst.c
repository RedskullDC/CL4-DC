#ifndef PUTCONST_C
#define PUTCONST_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

short putconst(char *nptr, char FLDtype)	// We hit a contstant in the program body. Save it as a variable
{
    FLDdesc *fld;
	double	v3;

    unsigned int v2;
    int		v10;
    int		VarSize;
    short	v7;
    short	v12;
    short	v13;
    char	arg[256];

	//eprint("putconst(\"%s\",%c)\n",nptr,FLDtype);

	v2 = strlen(nptr) + 1;
    if ( v2 - 1 > 19 )
    {
		++lit_no_0;
        sprintf(arg, ".?.%d", (unsigned short)lit_no_0);
        v13 = last_var++;
        v10 = v13;
	}
    else
    {
		if ( FLDtype == 'C' )					// make name for literal strings start with a single quote
			cdbcpystr(arg, "'", nptr, 0);
        else
			sprintf(arg, "%.*f", FLDtype - '0', (double)strtod(nptr, 0));

		troot = gettnode(troot, arg, strlen(arg), 1);
        v13 = last_fnd;
        v10 = last_fnd;
	}
//==========================================	
    if ( !getvars(v10) )						// check to see if Variable already defined
    {
		fld = getvarmem();
        fld->FLDname = getmem(strlen(arg) + 1);
        cdbcpystr(fld->FLDname, arg, 0);
        
        if ( FLDtype == 'C' )
			VarSize = v2 - 1;
		else
			VarSize = sizeof(double);	// 8; 

		v12 = VarSize;
        fld->FLDdata = getmem(VarSize + 1);		// Why an extra byte for double types?
        fld->FLDtype = FLDtype;
        switch ( FLDtype )
        {
		   case 'B':
				fld->FLDlen = 1;
				fld->TDFtype = 0x0044u;
				break;
		   case 'I':
				fld->FLDlen = 2;
				fld->TDFtype = 0x0044u;
				break;
		   case 'N':
				fld->FLDlen = 4;
				fld->TDFtype = 0x0044u;
				break;
		   case 'L':
				fld->FLDlen = 8;
				fld->TDFtype = 0x0044u;
				break;
		   case 'G':
				fld->FLDlen = 4;
				fld->TDFtype = 0x0404u;
				break;
		   case 'T':
				fld->FLDlen = 4;
				fld->TDFtype = 0x0800u;
				break;
		   case 'R':
				fld->FLDlen = 4;
				fld->TDFtype = 0x0604u;
				break;
		   case 'D':
				fld->FLDlen = 4;
				fld->TDFtype = 0x1040u;
				break;
		   case 'C':
				fld->FLDlen = v12;
				fld->TDFtype = 0x001Au;
				break;
		   default:					// Floating point types
				fld->FLDlen = 8;
				v7 = FLDtype - 48;	// ascii '0' -> '9' becomes 0x00 -> 0x09
				fld->TDFtype = FPtypes[v7];
				break;
		}
        if ( FLDtype == 'C' )
        {
			cdbcpystr((char *)fld->FLDdata, (char *)nptr, 0);
            if ( *nptr )
				fld->FLDstat &= ~fld_ZERO;	// 0xFFFBu;
			else
				fld->FLDstat |= fld_ZERO;
		}
        else
        {
			v3 = strtod(nptr, 0);
            if ( v3 == 0.0 )
				fld->FLDstat |= fld_ZERO;
			else
				fld->FLDstat &= ~fld_ZERO;	// 0xFFFBu;
			*(double *)fld->FLDdata = v3;
		}
        fld->FLDstat |= fld_VAR_USED;	// Indicate that variable is accessed in program - for prdebug() use
	}
    return v13;
}

#endif
