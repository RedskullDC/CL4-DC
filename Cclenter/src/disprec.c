#ifndef DISPLAY_REC_C
#define DISPLAY_REC_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

void _disprec(BTAB *bt, int TDNo, short PT_start, short PT_end, bool show)
{
    PTAB	*ptb_start;
	PTAB	*ptb_end;
	BTAB	*bt2;
	XTAB	*xt;
	RTAB	*rt;
	
	int		fno;
	int		MaxLen;
	short	Column;
    short	Line;
    int		TTno;
	char	FLDtype;
    char	a3[1032];

    ptb_start	= PTARR(getptabp(PT_start));
	ptb_end		= PTARR(getptabp(PT_end));

	while( ptb_start <= ptb_end )
    {
        srclineno	= ptb_start->SrcLineNo;			// **global var ** Set in various places, but never read? Debug Code?
        //OpCode		= ptb_start->OpCode;
		//switch (OpCode)
		switch (ptb_start->OpCode)
		{
		
		case 684:									// "ALIGN"
            pset(ptb_start);
			break;
		
		case 720:									// do {blockname}
			bt2 = &btab[ptb_start->Operand];
            _disprec(bt, TDNo, bt2->Screens->PT_start, bt2->Screens->PT_end, show);	// Some nice recursion possible here
			break;
		
		case 884:									// exit_do
			return;
		
		case 1:										// accept field
			xt = XTARR(ptb_start->TABno);
			
            if ((xt->Flags & xtb_FDF))				// FDF First Display Field for a block
				fdf_found = true;					// **global var **
            
            if ((xt->Flags & xtb_LKF))				// LKF Last Key Field
				lkf_found = true;					// **global var **
                    
            fno = gettdfno(ENARR(xt->VarExpNo), &TTno, &FLDtype, 1);
			assert(fno >= 0);
            
			if ( TTno )	// Field in a record
            {
				dispfld(xt, &ttab[TTno].TTfields[fno], show, 0, 1, 0);
                if ( xt->Flags & xtb_JOIN )						// JOIN command follows this accept field
                {
					rt = RTARR(xt->RangeID);
                    rfree(rt->TTno, 0);
                }
			}
            else	// This is a variable (system or user)
            {
				MaxLen = getwid(xt);
                memset(a3, ' ', MaxLen);								// clear answer area 
                a3[MaxLen] = 0;
                clgetyx(xt, &Column, &Line, yx_answer);					// position to answer field
                toscreen(Column, Line, 1, MaxLen, a3, xt->Attr, 1, 0);
			}
			break;
		case 4:										// 4 = "text"
			if (!show && fdf_found )				// **global var ** First Display Field
				dhdtxt(XTARR(ptb_start->TABno), false, true);			// text field, and allow glitch calcs
			break;
		}
		ptb_start++;
    }
}

void disprec(BTAB *btb, int TDno, int ScreenNum, bool show, short PT_st)
{
    SCR		*scr;
	short	PT_start;
	
    scr = (SCR*)btb->Screens;
	while (ScreenNum--)
		scr = scr->NextSCR;		// no sanity check on validity of scr pointer!

    lkf_found	= false;
    fdf_found	= false;

	PT_start	= PT_st ? PT_st : scr->PT_start ;

	_disprec(btb, TDno, PT_start, scr->PT_end, show);
}

#endif

