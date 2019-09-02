#ifndef INTEXP_C
#define INTEXP_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits


bool intexp(short ENTno)
{
    ENTAB	*entb;
    int		TDno;
	unsigned short OpCode;
	char	FLDtype;

    allenpt(ENTno, &entb);
    if ( entb )
	{
		OpCode = entb->TTno & 0xFE00;		// preserve state of Integer Flag bit 0x0200
		if ( entb->entype != 2 )
			goto LABEL_17;

		if ( entb->TTno & 0x0100 )			// Reserved word flag
		{
			dallenpt(&entb);
			return false;					// Not an integer exp.
		}
		if ( !(entb->TTno & 0x0200) )		// Integer calc flag. 
		{
			OpCode = entb->TTno & 0xFC00;
			/*
			switch (OpCode)
			{
			case 0x3000:
			case :
			case :

			default:
			}
			*/
			if ( OpCode != 0x3000 )	// asize()
			{
				if ( OpCode <= (signed int)0x3000u )
				{
					if ( !(entb->TTno & 0xFC00) )
					{
						if ( !(entb->TTno & 0x003F) )
							__assert_fail("(((exp->enun.Enop.Enoper) & 0x3FF) & (02|01|04|010|040|020))","intexp.c",0x3Au,"intexp");
						dallenpt(&entb);
						return (entb->TTno >> 9) & 1;
					}
					if ( OpCode != 0x2400 )	// slen()
					{
						dallenpt(&entb);
						return (entb->TTno >> 9) & 1;	// return state of Integer calc flag on this expression
					}
					goto LABEL_23;
				}
				if ( OpCode != 0x6C00 )			// match()
				{
					if ( OpCode != 0xAC00 )		// Array Subscript operator [] 
					{
						dallenpt(&entb);
						return (entb->TTno >> 9) & 1;
					}
	LABEL_17:
					gettf(ENTno, &TDno,&FLDtype);
					dallenpt(&entb);
					switch (FLDtype)
					{
					case 'I':
					case 'B':
					case 'N':
						return true;	// Is an integer expression
					default:
						return false;	// Is *not*
					}
				}
			}
		}
	}
LABEL_23:
	dallenpt(&entb);
    return true;	// exit_success. This *is* an Integer expression
}

#endif
