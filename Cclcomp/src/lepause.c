#ifndef LEPAUSE_C
#define LEPAUSE_C
#include "DBdefs.h"
#include "cl4.h"

bool lepause(PTAB *pt)
{
	ENTAB	*entb;
	short	expno;
	short	expno2;
	unsigned short ENno;
	
	symbol = getsym();
    if ( symbol == 930 )				// pause with no arguments is permissible
		return true;

	expno = loadexp(0, 1);
    if ( expno )
    {
		if ( symbol == 930 )			// <CR>, means only one argument
			expno2 = 0;
		else
			expno2 = loadexp(0, 1);		// optional second expression. Not defined in Manual
		ENno		  = getenmem();
        entb		  = ENARR(ENno);
		entb->enleft  = expno;
        entb->enright = expno2;
        pt->TABno	  = ENno;
        return true;
     }
     return false;	// error exit
}

#endif
