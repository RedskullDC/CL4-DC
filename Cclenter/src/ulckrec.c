#ifndef UNLOCK_RECORD_C
#define UNLOCK_RECORD_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int _ulckrec2(int TTno)		// name conflict in libcl4 with public function
{
	TDesc	*TDptr;
	int		v2;
	
	//printf("_ulckrec2(TTno = %3d)\n" ,TTno);
	TDptr = &ttab[TTno];	
	v2 = 0;
	if ( TDptr->TDlocked & tdp_LOCK ) // 0x0001
	{
		fix_buf(TTno, TDptr->TDworkarea2);
		v2 = ulockr(rtdget(TDptr), TDptr->TDworkarea2, 0);
		if ( v2 < 0 )
			dberror(v2, 0, rtdget(TDptr));
		TDptr->TDlocked &= ~tdp_LOCK;			// 0xFFFEu; // clear locked flag
	}
	return v2;
}

int ulckrec(int TTno)
{
	int v1;
	int i;
	
	//printf("ulckrec(TTno = %3d)\n" ,TTno);
	v1 = 0;
	if ( TTno > 0 )
		v1 = _ulckrec2(TTno);
	else	
	{
		for ( i = 0; i < no_ttabs; ++i )	// pass -1 in TTno to unlock all TT's
			v1 = _ulckrec2(i);
	}
	return v1;
}

#endif
