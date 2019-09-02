#ifndef DMERGE_C
#define DMERGE_C

#include "DBdefs.h"
#include "cl4.h"

void _dmerge(RLARR *RL, TDinfo *TDptr)
{
    RLARR *RLa; // edi@1

    for ( RLa = RL; RLa; RLa = RLa->NextRLARR )
    {
        relnode(TDptr->TDNodePtr);
        TDptr->TDNodePtr = 0;

        _dpos(&RLa->Dpos, RLa->PageNo, RLa->Depth, TDptr);
        RLa->field_1C = _dfetch(&RLa->DataStart, &RLa->Dpos, TDptr);
    }
}

#endif

