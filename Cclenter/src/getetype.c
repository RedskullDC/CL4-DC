#ifndef GETETYPE_C
#define GETETYPE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

char getetype(ENTAB *ent)	// get expression type
{
	FLDdesc	fld;
    int		TTno; 
    short	fno; 
	char	FLDtype;

    if ( ent )
	{
		switch (ent->entype)
		{
		case 0x01:
			fno = gettdfno(ent, &TTno, &FLDtype, 1);
			assert(fno >= 0);
			assert(TTno >= 0);
			//return ttab[TTno].TTfields[fno].FLDtype;	// Why not just return FLDtype??  copy und paste krapp!
			return FLDtype;
			break;

		case 0x02:
			if ( ent->TTno & 0x0100)					// reserved char* value/function
				return 'C';
			switch (ent->TTno & 0xFC00)
			{
				case 0x1C00:	//	ucase()
				case 0x2000:	//	lcase()
				case 0x2800:	//	day()
				case 0x2C00:	//	getarg()
				case 0x3800:	//	chr()
				case 0x4000:	//	getenv()
				case 0x6C00:	//	match()
				case 0x7400:	//	sdel()
				case 0x7800:	//	smult()
				case 0x7C00:	//	sword()
				case 0x8000:	//	setenv()
				case 0x8800:	//	sfmt()
				case 0x8C00:	//	getxml()
				case 0xA000:	//	getcgi()
				case 0xCC00:	//	substr()
				case 0xF400:	//	scat()
					return 'C';

				case 0x2400:	//	slen()
				case 0x3000:	//	asize()
				case 0x3400:	//	asc()
				case 0xBC00:	//	keyready()
					return 'N';

				case 0xB800:	//	random()
					return '9';

				case 0xAC00:	//	'['	array subscript operation
					return getftf(ent, 1, &fld)->FLDtype;
			
				default:
					return getetype(ENARR(ent->enleft));	// Recursion here!
			}
			break;

		case 0x04:				// float literal, RVALUE only			*real clcomp doesn't create these*
			return '6';
		case 0x08:				// int literal, RVALUE only
			return 'N';
		case 0x10:				// String literal, 3chars or less + \0
			return 'C';
		}
	}
	return 0;	// ent == 0 or invalid expression type
}

#endif

