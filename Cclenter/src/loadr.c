#ifndef LOADR_C
#define LOADR_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

RLIST *addrun(RLIST *rlst, RLIST *rl_src, REFR *refr, TDinfo *TDptr)
{
    RLIST *rl_dest;
    short Depth;

    rl_dest = rlst;
    if ( !rlst )
    {
        rl_dest = (RLIST *)alloc(56, 0);        // 7 x 8 = 56
        rl_dest->Depth = 0;
    }
    if ( rl_dest->Depth == 6 )
    {
        _merge(rl_dest + 1, rl_dest->Depth, refr, TDptr);

		// PageNo either points at another RLIST, or holds an actual PageNo
        rl_dest->PageNo = (int)addrun((RLIST *)rl_dest->PageNo, rl_dest + 1, refr, TDptr);		// recursive
        rl_dest->Depth = 0;
    }
    Depth = rl_dest->Depth;
    rl_dest->Depth = Depth + 1;

    cpybuf((char *)&rl_dest[Depth + 1], (char *)rl_src, sizeof(RLIST));		// sizeof(RLIST) = 8
    return rl_dest;
}

int _loadr(int TDno, char *WorkArea, POS *pos)
{
    REFR		**v12;
    SALLOCBUF	*SAp;
	TDinfo		*TDptr;
	int			v4;
	int			BytesCopied;
	//short		v8;
	short		idx;
	//short		v10;
	//short		v11;
	RLIST		rlst[6];
	
	//printf("_loadr(%d)\n",TDno);
    TDptr = _chktd(TDno);
    v4 = check_bup_sem(TDptr->TDDBinfo);
    if ( v4 )
        derror(v4, 0, TDptr);                   // 
                                                // 
    if ( TDptr->TDFlags & 4 )
        derror(37, 0, TDptr);                   // 
                                                // 
    SAp = TDptr->TDSallocBuf;                   // 
                                                // 
    if ( SAp )
    {
        if ( TDptr->TDFlags & 2 )               // This is a single run through table that has been read?
            return -1;
    }
    else
    {                                           // first time, set up buffers etc.
        SAp = _salloc((SALLOCBUF *)ealloc(_ldsize, 0), _ldsize, (unsigned int)(TDptr->TDRecMin + TDptr->TDRecSize) >> 1);		// ldsize defaults to 0x8000 (32768)
        TDptr->TDSallocBuf = SAp;
    }                                           // 
                                                // 
    while ( SAp->field_4 == SAp->MaxRecs2 || SAp->refr.Data - 1 < (REFR **)&SAp->refr.Offset[TDptr->TDRecSize] )// While room left in sallocbuf
    {
        if ( !SAp->field_4 )                    // initialized to ZERO by salloc()
            derror(17, 0, TDptr);               // 
                                                // 
        _append(&SAp->refr, SAp->NodeArr, SAp->DataPtr, TDptr);// 
                                                // 
        //v8 = SAp->field_4 - 1;
        //SAp->field_4 = v8;
        //if ( v8 )
        if ( --SAp->field_4 )
        {
            _mvref(SAp->DataPtr, &SAp->DataPtr[SAp->field_4]);
            _siftdown(SAp->DataPtr, SAp->field_4, 0, TDptr->TableDefs);
        }
        else
        {
            _pflush(SAp->NodeArr, &SAp->refr, rlst, TDptr);
            SAp->field_4 = _mvheap(SAp->DataPtr, SAp->MaxRecs2, SAp->MaxRecs1);
            SAp->MaxRecs2 = SAp->MaxRecs1;      // 
                                                // 
            for ( idx = SAp->field_4 / 2; idx >= 0; --idx )
                _siftdown(SAp->DataPtr, SAp->field_4, idx, TDptr->TableDefs);// 
                                                // 
            SAp->RL_Head = addrun(SAp->RL_Head, rlst, &SAp->refr, TDptr);
        }
    }                                           // 
                                                // 
                                                // 
    BytesCopied = rtotup(SAp->refr.Offset, WorkArea, pos, TDptr->TableDefs);// Copy data from WorkArea into SAp structure area
                                                // 
    if ( SAp->NodeArr[0] && _cmpkey(SAp->refr.Offset, SAp->DataPtr->Offset, TDptr->TableDefs) < 0 )
    {
        if ( SAp->RL_Head && SAp->RL_Head->Depth == 6 )
        {
            _hflush(SAp, rlst, TDptr);
            SAp->RL_Head = addrun(SAp->RL_Head, rlst, &SAp->refr, TDptr);
            rtotup(SAp->refr.Offset, WorkArea, pos, TDptr->TableDefs);
            _mvref(SAp->DataPtr, &SAp->refr);
            ++SAp->field_4;
        }
        else
        {
            //v10 = SAp->MaxRecs2 - 1;
            //SAp->MaxRecs2 = v10;
            //_mvref(&SAp->DataPtr[v10], &SAp->refr);
            _mvref(&SAp->DataPtr[--SAp->MaxRecs2], &SAp->refr);
        }
    }
    else
    {
        //v11 = SAp->field_4;
        //SAp->field_4 = v11 + 1;
        //_mvref(&SAp->DataPtr[v11], &SAp->refr);
        _mvref(&SAp->DataPtr[SAp->field_4++], &SAp->refr);
		_siftup(SAp->DataPtr, SAp->field_4, TDptr->TableDefs);
    }                                           // 
                                                // 
                                                // 
    SAp->refr.Offset += BytesCopied;
    v12 = SAp->refr.Data;
    SAp->refr.Data = v12 - 1;
    *(v12 - 1) = &SAp->refr;                    // Always the lowest REFR in the upper structure?
    return 1;                                   // exit_success
}

int loadr(int TDno, char *Buffer, POS *pos)
{
    //return (signed __int16)xenter((int)loadr, TDno, Buffer, pos);
    return _loadr(TDno, Buffer, pos);
}


#endif
