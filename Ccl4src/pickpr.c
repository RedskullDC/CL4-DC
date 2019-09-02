#ifndef PICKPREV_C
#define PICKPREV_C

short _pickprev(TDinfo *TDptr, char *WorkArea, short SkipRecs, POS *pos)
{
    RLARR *v4;
	RLARR *RLa;
	short v6;
	int v8;
	short v9;
	
    if ( TDptr->TDFlags & 0x1000 )		// 0x1000 indicates TDptr->TDSallocBuf holds an RLARR structure
    {
        relnode(TDptr->TDNodePtr);
        TDptr->TDNodePtr = 0;
        TDptr->TDFlags &= 0xEFFFu;
        _dmerge((RLARR *)TDptr->TDSallocBuf, TDptr);
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
				if ( v4 && (v6 = _cmpkey(&RLa->DataStart, &v4->DataStart, TDptr->TableDefs), v6 <= 0) )
                {
					if ( !v6 && !(((unsigned short)TDptr->TDFlags >> 8) & 4) )
                    {
						if ( !(((unsigned short)TDptr->TDFlags >> 8) & 8) )
							_addtuple(&v4->DataStart, &RLa->DataStart, TDptr->TableDefs);
						RLa->field_1C = _dfetch(&RLa->DataStart, &RLa->Dpos, TDptr);
					}
				}
                else
                {
					v4 = RLa;
				}
			}
		}
        if ( !v4 )
			break;
		
		tuptor(WorkArea, pos, &v4->DataStart, TDptr->TableDefs);
        v4->field_1C = _dfetch(&v4->DataStart, &v4->Dpos, TDptr);
        ++v9;
        WorkArea += v8;
    }
    return v9;
}

#endif
