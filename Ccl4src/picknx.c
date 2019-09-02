#ifndef PICKNEXT_C
#define PICKNEXT_C

#include <pthread.h>		// for pthread_mutex fns()
#include "DBdefs.h"
#include "cl4.h"

short _picknxt(TDinfo *TDptr, char *WorkArea, short SkipRecs, POS *pos)
{
    RLARR *v4;
	RLARR *RLa;
	short v6;
	int v8;
	short v9;
	
    if ( TDptr->TDFlags & 0x1000 )		// 0x1000 indicates TDptr->TDSallocBuf holds an RLARR structure
    {
        TDptr->TDFlags &= 0xEFFFu;
        _imerge((RLARR *)TDptr->TDSallocBuf, TDptr);  // If we fall into this routine, TDptr->TDSallocBuf holds a pointer to a RLARR list
    }

	if ( pos )
        v8 = cdbrindex(pos, 0);
    else
        v8 = TDptr->TDRecSize;

	v9 = 0;
    while ( SkipRecs > v9 )
    {
		v4 = 0; 
        for ( RLa = (RLARR *)TDptr->TDSallocBuf; RLa; RLa = RLa->NextRLARR )
        {
			if ( RLa->field_1C )
            {
				if ( v4 && (v6 = _cmpkey(&RLa->DataStart, &v4->DataStart, TDptr->TableDefs), v6 >= 0) )
                {
					if ( !v6 && !(((unsigned short)TDptr->TDFlags >> 8) & 4) )
                    {
						if ( !(((unsigned short)TDptr->TDFlags >> 8) & 8) )
							_addtuple(&v4->DataStart, &RLa->DataStart, TDptr->TableDefs);// 
                                                // 
						RLa->field_1C = _ifetch(&RLa->DataStart, &RLa->Dpos, TDptr);
					}
				}
                else
                {
					v4 = RLa;
				}
			}
		}
        if ( !v4 )	// reached end of the linked list
			break;

		tuptor(WorkArea, pos, &v4->DataStart, TDptr->TableDefs);
        v4->field_1C = _ifetch(&v4->DataStart, &v4->Dpos, TDptr);
        ++v9;
        WorkArea += v8;
    }
    return v9;
}

#endif
