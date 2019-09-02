#ifndef SIFT_C
#define SIFT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void swapref(REFR *refr, short Left, short Right)
{
    REFR	**v6;
	REFR	**v7;
	
	REFR	*R_left;
	REFR	*R_rght;
	char	*v5;
	
    R_left			= &refr[Left];
    R_rght			= &refr[Right];

    v5				= R_left->Offset;
    v6				= R_left->Data;
    
	R_left->Offset	= R_rght->Offset;
    v7				= R_rght->Data;
    R_left->Data	= R_rght->Data;
    *v7				= R_left;
    R_rght->Offset	= v5;
    R_rght->Data	= v6;
    *v6				= R_rght;
}

void _siftdown(REFR *ref, short a2, short a3, TDef *tdef)
{
    short v4;
	short idx;
	
    v4 = a3;
    for ( idx = (2 * a3 + 1); idx < a2; idx = (2 * idx + 1) )
    {
        if ( idx + 1 < a2 && _cmpkey(ref[idx + 1].Offset, ref[idx].Offset, tdef) <= -1 )
            idx++;

		if ( _cmpkey(ref[v4].Offset, ref[idx].Offset, tdef) <= 0 )
            break;

        swapref(ref, v4, idx);
        v4 = idx;
    }
}

void _siftup(REFR *ref, short a2, TDef *tdef)
{
    short idx;
	short v4;
	
    for ( idx = a2 - 1; ; idx = (idx - 1) / 2 )
    {
        v4 = (idx - 1) / 2;
        if ( v4 >= idx || _cmpkey(ref[idx].Offset, ref[v4].Offset, tdef) >= 0 )
            break;

		swapref(ref, idx, v4);        // i-1/2 == v4
        //swapref(ref, idx, (idx - 1) / 2);        // i-1/2 == v4
    }
    _siftdown(ref, a2, idx, tdef);
}


#endif
