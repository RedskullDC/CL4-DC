#ifndef PFLUSH_C
#define PFLUSH_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

void _pflush(NODE **NodeArray, REFR *refr, RLIST *rlst, TDinfo *TDptr)
{
    NODE	**v4;
    NODE	*nodep;
    int		PageNo;

    PageNo = 0;
    v4 = NodeArray;
    for ( nodep = *NodeArray; *v4; nodep = *v4 )
    {
        nodep->NODE2ptr[nodep->NumEntries].PageNo = PageNo;
        PageNo = nodep->PageNo;
        putnode(TDptr, nodep);
        _msqueeze(refr[1].Data, refr->Data, (REFR*)nodep->NODE1ptr, (REFR*)&nodep->NODE1ptr[nodep->NumEntries]);
        relnode(nodep);
        *v4++ = 0;
    }
    rlst->PageNo	= PageNo;				// update caller memory
    rlst->Depth		= v4 - NodeArray;
}
#endif
