#ifndef GETTF_C
#define GETTF_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

short gettf(short ENTno, int *TDno, char *FLDtype)
{
	ENTAB	*entab;
	FLDdesc *fld;
	short	FieldNo;
	int		TTno;

	entab = ENARR(ENTno);
	while (entab && entab->enleft)
		entab = ENARR(entab->enleft);

	*TDno	= 0;
    FieldNo = -1;
	
	if ( !entab || (entab->entype == 2 && entab->Enun.Enop.Enoper & 0x0100))	// 0x0100 == Reserved Name flag
	{
        *FLDtype	= 0;
        // return		FieldNo;	// -1
	}
	else
	{
		switch ( entab->entype )
		{
		case 4:						// float literal. Unused. *real* clcomp doesn't appear to create these!!
			*FLDtype = '6';
			break;
		case 8:						// Expression holds an integer
			*FLDtype = 'N';
			break;
		case 0x10:					// Expression holds a 3 char (or less) string
			*FLDtype = 'C';
			break;
		case 1:
			TTno	= entab->Enun.Enref.TTno;
			FieldNo = entab->Enun.Enref.VarNum;
			*TDno	= TTno;
			if ( TTno )
				fld = &ttab[TTno].TTfields[FieldNo];
			else
				fld = getvars(FieldNo);
			*FLDtype = fld->FLDtype;
			break;
		default:
			__assert_fail("((t)->entype == 0x1)", "gettf.c", 0x2Au, "gettf");
			break;
		}
	}
	return FieldNo;
}

int gettfs(unsigned short ENTABno, int *TTno, char *FLDtype, short *FLDstat)
{
    ENTAB	*entb;
    FLDdesc *fld;
    int		FieldNo;
    int		v6;

    fld		= 0;
    *TTno	= 0;		// Update caller memory directly
    FieldNo = -1;
	
	//	ent = ENARR(ENTno);
	//	while (ent && ent->enleft)
	//		ent = ENARR(ent->enleft);
	
	entb	= ENARR(ENTABno);
	if ( !entb )
		goto LABEL_23;
	while ( entb->enleft )
    {
		entb = &enarr.TableAddr[entb->enleft - 1];
        if ( !entb )
			goto LABEL_23;
	}
    if ( !entb )				// should be unreachable
		goto LABEL_23;
	
	if ( entb->entype == 2 && entb->Enun.Enop.Enoper & 0x0100 )
    {
LABEL_23:
        *FLDtype	= 0;
        return		FieldNo;
	}
	//printf("entb->entype = %d\n",entb->entype);
    switch ( entb->entype )
    {
	case 0x0004:
        *FLDtype = '6';			// embedded float literal ** REAL ** clcomp doesn't create these!!
		break;
	case 0x0008:
		*FLDtype = 'N';			// embedded int
        break;
	case 0x0010:				
		*FLDtype = 'C';			// embedded String
        break;
	case 0x0001:
		v6		= entb->Enun.Enref.TTno;
        *TTno	= v6;								// Update caller memory directly
        FieldNo = entb->Enun.Enref.VarNum;
        if ( v6 )
			fld = &ttab[*TTno].TTfields[FieldNo];	// Table Field
		else
			fld = getvars(FieldNo);					// Normal variable
		*FLDtype = fld->FLDtype;					// Update caller memory directly
		break;
	default:										// **ERROR**
		__assert_fail("((t)->entype == 0x1)", "gettf.c", 0x5Cu, "gettfs");
        break;
	}
    *FLDstat = fld->FLDstat;						// Update caller memory directly. ##wrong## fld uninitiliased for most types!!!
    return FieldNo;
}

#endif
