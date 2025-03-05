#ifndef SFLUSH_C
#define SFLUSH_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short extract(RLIST **RL_Head, short a2, RLIST *Dest)
{
    RLIST *rlst;
	RLIST *v4;
	//short v5;

    rlst = *RL_Head;
    v4 = Dest;
    a2--;
	while ( a2 >= 0 )
    {
		//v5 = rlst->Depth - 1;
        //rlst->Depth = v5;
		//cpybuf((char *)v4, (char *)&rlst[v5 + 1], sizeof(RLIST));	// sizeof(RLIST) = 8 

		cpybuf((char *)v4, (char *)&rlst[rlst->Depth--], sizeof(RLIST));	// sizeof(RLIST) = 8 

		if ( !rlst->Depth )
        {
			rlst = (RLIST *)nfree(rlst, (void *)rlst->PageNo);		// PageNo can also hold an RLIST*
            *RL_Head = rlst;										// Alters caller memory directly
		}
        v4++;
        --a2;
    }
    return v4 - Dest;	// pointer arithmetic
}

short count(RLIST *rlst)
{
    short Count;

    for ( Count = 0 ; rlst ; rlst = (RLIST *)rlst->PageNo )// PageNo and Depth are dual use fields here  **TODO** not X64 compat
        Count += rlst->Depth;

    return Count;
}

short _sflush(RLIST *rlst_dest, short MaxDepth, SALLOCBUF *SAp, TDinfo *TDptr)
{
    int		v8;
	short	v6;
	short	v9;
	short	v10;
	short	a2;
	RLIST	rlst_src[6];
	
	//printf("_sflush( RLIST*: x%08X , MaxDepth: %d, TDptr: x%08X\n", rlst_dest, MaxDepth, TDptr);
	//printf("_hflush#63 SAp->field_4 = %d\n",SAp->field_4); 
    
	a2 = 0;
    if ( SAp->field_4 )
    {
        a2 = 1;
        _hflush(SAp, rlst_src, TDptr);
    }
	//printf("_hflush#70 SAp->field_4 = %d\n",SAp->field_4); 
    
	SAp->field_4	= _mvheap(SAp->DataPtr, SAp->MaxRecs2, SAp->MaxRecs1);
    SAp->MaxRecs2	= SAp->MaxRecs1;

	//printf("_hflush#75 SAp->field_4 = %d\n",SAp->field_4); 

	for ( v10 = SAp->field_4 / 2; v10 >= 0; --v10 )
        _siftdown(SAp->DataPtr, SAp->field_4, v10, TDptr->TableDefs);

	//printf("_hflush#80 SAp->field_4 = %d\n",SAp->field_4); 

	if ( SAp->field_4 )
		_hflush(SAp, &rlst_src[a2++], TDptr);
	
	v6 = a2 + count(SAp->RL_Head);
    if ( v6 > 6 )
    {
        v8 = (v6 - 1) % 5;
        if ( v8 )
            v6 = v8 + 1;
        else
            v6 = 6;
    }
    v9 = a2 + extract(&SAp->RL_Head, v6 - a2, &rlst_src[a2]);

//-------------------------------------------------------

LABEL_14:
    a2 = v9;
    if ( SAp->RL_Head )
        goto LABEL_12;

    while ( MaxDepth < a2 )
    {
LABEL_12:
        _merge(rlst_src, a2, &SAp->refr, TDptr);
        a2 = 1;
        if ( SAp->RL_Head )
        {
            v9 = extract(&SAp->RL_Head, 5, &rlst_src[1]) + 1;
            goto LABEL_14;
        }
    }  
	
	for (v10 = 0 ; v10 < a2 ; v10++ )
		cpybuf((char *)&rlst_dest[v10], (char *)&rlst_src[v10], sizeof(RLIST));		// sizeof(RLIST) = 8 

	return a2;
}
#endif
