#ifndef REDISPLAY_C
#define REDISPLAY_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

void redisp(RDTAB *rdtab, short Start, short End)
{
    ENTAB	*entb;
	PTAB	*ptabStart;
	PTAB	*ptabEnd;
	XTAB	*xtb;
	ENTAB	*v9;
	BTAB	*btb;
	RDTAB	*rdtb;
	
    int		OpCode;
	int		v14;
	short	RecNo;
	
    if ( rdtab )
    {
        do
        {
            v14 = rdtab->TTno;		// if > 0, redisplay all visible fields of this table
            if ( v14 <= 0 )
            {
                entb = ENARR(rdtab->field_2);	// Only re-displaying a single field/variable
                v14 = entb->Enun.Enref.TTno;
            }
            ptabStart	= PTARR(getptabp(Start));			// Look for redisplay records between program Start and End instructions
			ptabEnd		= PTARR(getptabp(End));

            for ( ; ptabStart != ptabEnd; ptabStart++ )
            {
                OpCode = ptabStart->OpCode;
                if ( OpCode == 1 )						// accept field
                {
					xtb = XTARR(ptabStart->TABno);
					//if ( *(int *)&xtb->C_X )			// check for C_X and C_Y together. Means field has been shown on screen already
					if ( xtb->C_X || xtb->C_Y )			// ** int* = 8 bytes in X64 == ERROR! **
					{
                        v9 = ENARR(xtb->VarExpNo);
                        if ( v9->Enun.Enref.TTno == v14 )
                        {
							RecNo = v9->Enun.Enref.VarNum;
                            //if ( rdtab->TTno > 0 || (v11 = ENARR(rdtab->field_2), v11->RecNo == RecNo) )

                            if ( rdtab->TTno > 0 || ( ENARR(rdtab->field_2)->Enun.Enref.VarNum == RecNo) )
                                dispfld(xtb, &ttab[v14].TTfields[RecNo], 1, 0, 1, 1);		// Last 1 means use C_X, C_Y values
                        }
                    }
                }
                else if ( OpCode == 720 )										// do {blockname}
                {
					btb = &btab[ptabStart->Operand];
                    redisp(rdtab, btb->Screens->PT_start, btb->Screens->PT_end);	// nice recursion possible here
                }
            }
            rdtb = RDARR(rdtab->NextRD);
            rdtab = rdtb;
        }
        while ( rdtb );
    }
}


#endif

