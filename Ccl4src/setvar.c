#ifndef SETVAR_C
#define SETVAR_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

void setvar(int VarNo, int Value)
{
    FLDdesc *fld;
	double	v3;
	
    fld = &ttab->TTfields[VarNo];
    v3 = (double)Value;
    *(double *)fld->FLDdata = v3;
    
	if ( v3 == 0.0 )				// quick zero flag
        fld->FLDstat |= fld_ZERO;	// 0x0004;
    else
        fld->FLDstat &= ~fld_ZERO;	// 0xFFFB;
}

int getvar(int VarNum)
{
    FLDdesc *fld;
	
    fld = &ttab->TTfields[VarNum];
    return (int)*(double *)fld->FLDdata;
}

int incvar(int VarNum, int a2)
{
    FLDdesc *fld;
	double	*v3;
	double	v4;
	short	v5;
	
    fld = &ttab->TTfields[VarNum];
    v3 = (double *)fld->FLDdata;
    v4 = (double)a2 + *v3;
    *v3 = v4;
    if ( v4 == 0.0 )					// check for quick zero flag
        fld->FLDstat |= fld_ZERO;	// 0x0004;
    else
        fld->FLDstat &= ~fld_ZERO;	// 0xFFFB;
    return (int)v4;
}

void setcvar(int VarNum, char *Src)
{
    FLDdesc *fld;

    fld = &ttab->TTfields[VarNum];
    if ( Src )
        cpybuf((char *)fld->FLDdata, Src, fld->FLDlen);
    else
        *(char *)fld->FLDdata = 0;

    if (*(char *)fld->FLDdata )		// quick zero flags
        fld->FLDstat &= ~fld_ZERO;	// 0xFFFB;
    else
        fld->FLDstat |= fld_ZERO;	// 0x0004;
}

// set an Array variable
void setavar(int VarNum, char *Src, int SubScript)
{
    FLDdesc *fld;
	char *Buffer;
	
    fld = &ttab->TTfields[VarNum];
    Buffer = &fld->FLDname[SubScript * (fld->FLDlen + 1)];	// Arrays hang off fld->FLDname
    if ( Src )
        cpybuf(Buffer, Src, fld->FLDlen);
    else
        *Buffer = 0;
}


#endif

