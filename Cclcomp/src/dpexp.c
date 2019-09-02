#ifndef DPEXP_C
#define DPEXP_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

short dpexp(short ENTno)		// Return the number of decimal places expected for this expression
{
	ENTAB	*entb;
    int		TDno;
	unsigned short OpCode;
	short	v4;
	short	v5;
	char	FLDtype;

    if ( !ENTno )
		return 0;

    allenpt(ENTno, &entb);
    if ( entb->entype != 2 )
    {
		dallenpt(&entb);
        gettf(ENTno, &TDno, &FLDtype);
        return typedp(FLDtype);
	}
    if ( ((unsigned short)entb->TTno >> 8) & 3 )    // bit 9 signifies Integer calcs, 8 = reserved string flag    0x0300
    //if ( entb->un.s.OpCode & 0x0300 )    // bit 9 signifies Integer calcs, 8 = reserved string flag    0x0300
	{
		dallenpt(&entb);
        return 0;		// Integer Calcs, *NO* decimal places
	}
    
	OpCode = entb->TTno & 0xFC00;
	//OpCode = entb->un.s.OpCode & 0xFC00;
	switch (OpCode)
	{
	case f_MUL :					// "*" MULTIPLY
		dallenpt(&entb);
        return dpexp(entb->enright) + dpexp(entb->enleft);	// add decimal places of each expression.
		break;

	case f_ARRSTART:				// "[" Array operation
        gettf(ENTno, &TDno, &FLDtype);
        dallenpt(&entb);
        return typedp(FLDtype);
		break;
	
	case f_RANDOM:					// random()
		dallenpt(&entb);
        return 9;
		break;

	case f_SLEN:					// slen()
	case f_MOD :					// "%" MOD
	case f_MATCH:					// match()
	case f_KEYREADY:				// keyready()
		dallenpt(&entb);
		return 0;
		break;

	case f_ADD :					// "+" ADD
	case f_SUB :					// "-" SUBTRACT
		v4 = dpexp(entb->enleft);		// ** recursive **
		v5 = dpexp(entb->enright);
		dallenpt(&entb);
		return v5 > v4 ? v5 : v4;	// return whichever has the most decimal places
		break;

	case 0x0000:					// *ERROR*
		//__assert_fail("0", "dpexp.c", 0x24u, "dpexp");
		assert(0);
		break;

	default:						// all others, default to destination number of places
		dallenpt(&entb);
		return 99;
		break;
	}
}


#endif
