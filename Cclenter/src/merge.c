#ifndef MERGE_C
#define MERGE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void _merge(RLIST *rlst, short Depth, REFR *refr, TDinfo *TDptr)
{
    REFR	**v10;
	REFR	*v9;
	int		v8;
	short	count;
	short	v7;
	short	v13;
	
	bool	MaxDepth;
	
	REFR	v14[6];
	NODE	*NodeArray[8];
	DPOS	dpos[7];
	
	count = 0;
	while (count < 6)			// Max Depth == 6
        NodeArray[count++] = 0;

	for ( count = 0 ; count < Depth ; count++)
		_ipos(&dpos[count], rlst[count].PageNo, rlst[count].Depth, TDptr);

    MaxDepth = false;
    v13 = 0;
    while ( 1 )
    {
        if ( MaxDepth )
		{
			count = 0;
			v13	  = 0;
			while ( Depth > count )
			{
				if ( v14[count].Offset && (!v14[v13].Offset || _cmpkey(v14[count].Offset, v14[v13].Offset, TDptr->TableDefs) < 0) )
					v13 = count;
				count++;
			}
			if ( !v14[v13].Offset )
			    break;

			_append(refr, NodeArray, &v14[v13], TDptr);
		}

		v7 = _ifetch(refr->Offset, &dpos[v13], TDptr);
        v8 = v7;
        if ( v7 )
        {
            _mvref(&v14[v13], refr);
            v9 = refr;
            v9->Offset += v8;
            if ( v9->Data < (REFR **)v9->Offset )	// Buffer overrun. Shouldn't be possible
                derror(17, 0, TDptr);				// 
			
			v10 = refr->Data - 1;
            --refr->Data;
            *v10 = refr;
        }
        else
        {
            _sparetree(TDptr, rlst[v13].PageNo, rlst[v13].Depth);		// return pages to the system
            v14[v13].Offset = 0;
        }

        v13++;
        if ( v13 == Depth )
            MaxDepth = true;
    }
    _pflush(NodeArray, refr, rlst, TDptr);
}


#endif
