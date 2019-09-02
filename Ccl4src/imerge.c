#ifndef IMERGE_C
#define IMERGE_C

#include "DBdefs.h"
#include "cl4.h"

void _imerge(RLARR *RL, TDinfo *TDptr)
{
    RLARR *RLa; // edi@1

    for ( RLa = RL; RLa; RLa = RLa->NextRLARR )
    {
        relnode(TDptr->TDNodePtr);
        TDptr->TDNodePtr = 0;
        
		_ipos(&RLa->Dpos, RLa->PageNo, RLa->Depth, TDptr);
        RLa->field_1C = _ifetch(&RLa->DataStart, &RLa->Dpos, TDptr);
    }
}

#endif

