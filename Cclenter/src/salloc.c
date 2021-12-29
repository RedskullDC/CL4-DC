#ifndef SALLOC_C
#define SALLOC_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

SALLOCBUF *_salloc(SALLOCBUF *SAp, int LDsize, int avgRecSize)
{
    REFR	**BufferEnd;
    short	MaxRecs;
    short	v5;

    //MaxRecs = (LDsize - 52) / (unsigned int)(avgRecSize + 12);			// guesstimate how many recs fit in buffer
    MaxRecs = (LDsize - offsetof(struct SALLOCBUF, DataStart)) / (unsigned int)(avgRecSize + 12);			// guesstimate how many recs fit in buffer

    SAp->DataPtr	 = (REFR *)&SAp->DataStart;
    SAp->MaxRecs1	 = MaxRecs;
    SAp->field_4	 = 0;
    SAp->MaxRecs2	 = MaxRecs;
    SAp->refr.Offset = &SAp->DataStart + 8 * SAp->MaxRecs1;				// Allow room for [8 bytes * MaxRecs1] at Datastart
    BufferEnd		 = (REFR **)((char *)SAp + (LDsize & 0xFFFFFFFC));	// ensure end pointer is long word aligned
    SAp->BufferEnd	 = BufferEnd;										// Buffer End
    SAp->refr.Data   = BufferEnd - 1;									// following field point back at each other
    *(BufferEnd - 1) = &SAp->refr;
	
	for (v5 = 0 ; v5 <= 5 ; v5++ )
		SAp->NodeArr[v5] = 0;											// Related to Depth is some way?

    SAp->RL_Head	 = 0;												// RLIST*
    return SAp;
}

/*
00000000 SALLOCBUF       struc ; (sizeof=0x38)
00000000 DataPtr         dd ?                    ; offset
00000004 field_4         dw ?
00000006 MaxRecs2        dw ?
00000008 MaxRecs1        dw ?
0000000A field_A         dw ?
0000000C field_C         dd ?                   ; offset
00000010 WorkArea        dd ?                   points to mem following DataStart structures
00000014 BufferEnd       dd ?                   end of buffer, long-word aligned
00000018 NodeArr         dd 6 dup(?)            NODE*[6]
00000030 RL_Head         dd ?                   RLIST*
00000034 DataStart       db ?					8 byte structures[MaxRecs1]	*not RLISTS*
00000035 field_35        db ?
00000036 field_36        db ?
00000037 field_37        db ?
00000038 SALLOCBUF       ends

*/
#endif
