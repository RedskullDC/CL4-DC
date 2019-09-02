#ifndef GETDWID_C
#define GETDWID_C

#include "DBdefs.h"
#include "cl4.h"


// get default display width of a field for a given variable type

int getdwid(FLDdesc *FLDptr)
{
	//printf("getdwid(FLDptr = %08X)\n" ,FLDptr);
	if ( FLDptr->FLDqlen && FLDptr->FLDqlen->Qdeflen)
		return FLDptr->FLDqlen->Qdeflen;

	else
	{
		switch ( FLDptr->FLDtype )
		{
		case 'C':
			return FLDptr->FLDlen;		// strings are naturally variable
			break;
		case 'B':						// byte
			return 4;
			break;
		case 'I':						// short int
			return 6;
			break;
		case 'N':						// normal int
			return 9;
			break;
		case 'D':						// date
			return defDateWidth;
			break;
		case 'T':						// time
			return 5;					// 'hh:mm'
			break;
		case 'G':						// float/double
		case 'R':
			return 10;
			break;
		case 'K':						// Sub/Key records are not displayable!!!
		case 'S':
			return 0;
			break;
		default:						// everything else
			return 14;
			break;
		}
	}
}
#endif
