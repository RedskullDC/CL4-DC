#ifndef DSCREEN_C
#define DSCREEN_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void gtd(ENTAB *entab, int *TTno)
{
    if ( !entab || entab->entype == 2 && entab->Enun.Enop.Enoper & 0x0100 )	// reserved value
        *TTno = 0;
    else if ( entab->entype == 1 )
		*TTno = entab->Enun.Enref.TTno;
	else
		*TTno = 0;
}

void dhdtxt(XTAB *xtb, bool head, bool allow_glitch)
{
    char	*v5;

    int		FieldWidth;
	int		i;
	int		MaxLen;
	int		v10;
	short	GlitchChars;
	
	short	Column;
	short	Line;
	char	head_txt[1032];
	
    head_txt[0] = 0;
    clgetyx(xtb, &Column, &Line, yx_prompt);		// prompt position on screen.	yx_prompt = fixed value of 2
    exptobuf(head_txt, ENARR(xtb->Prompt_exp), 0);
    
	FieldWidth = getwid(xtb);
    if ( FieldWidth > 0 )
    {
        v5 = head_txt;
        for ( i = FieldWidth - 1; i != -1; --i )
            *v5++ = head_txt[0];
        *v5 = 0;
    }
    MaxLen = lenstr(head_txt);
    
	if ( Column == 0x1000 )							// Flag bits? 
		Column = (_co - MaxLen) / 2;
    else if ( Column == 0x2000 )					// centre, left and right attributes?
		Column = (_co - MaxLen);

    if ( MaxLen )									// true if we have text to display
    {
        v10 = xtb->ScrAttrib;
        if ( allow_glitch )							// only used by _disprec()
        {
            if ( Line == _li - 1 )
                return;
            GlitchChars = getnats(v10, 0x20u);		// get number of 'glitch' chars for TEXT fields
            
			v10 = 0x3Fu;
            Column -= GlitchChars;
            if ( Column <= 0 )
                Column = 1;
            
			MaxLen += 2 * GlitchChars;				// make room for glitch on/off chars
            memset(head_txt, ' ', MaxLen);			// ensure screen area for text field is clear before displaying
            head_txt[MaxLen] = 0;
        }
		// head true == head command, false == text
        toscreen(Column, Line, 0, MaxLen, head_txt, v10, (head ? 0x04u : 0x20u), 0);
    }
    else			// nothing to display, position cursor at correct place on screeen anyway
    {
        whereat(Column, Line, false, 0);			// don't allow mask bits
        at();
    }
}

void _dscreen(BTAB *bt, short PT_Start, short PT_End, bool allowDelayForm)
{
    PTAB	*ptb;
	PTAB	*ptab_end;
	
    BTAB	*btb;
	RDTAB	*rdtb;
	XTAB	*xtb;
	
    int		OpCode;
	int		FieldNo;
	int		TTno;
    short	Column;
	short	Line;
	
	bool	DelayForm = false; 
    
	char	fnum_txt[20];		// used for field numbering when active '>> xx' (xx == 1-99) 
	char	Prompt[1008];		// Prompt text for a head or accept field
	char	Dest[1008];			// temp area used to copy previous values prior to screen display
	
	char FLDtype;

    ptb		 = PTARR(getptabp(PT_Start));
	ptab_end = PTARR(getptabp(PT_End));

	while ( ptb <= ptab_end )					// look at all instructions in this block range which affect the screen.
    {
        srclineno = ptb->SrcLineNo;
        OpCode	  = ptb->OpCode;
		switch (OpCode)
		{
			case 265:							// delay_form - should appear before anything else!!
				if ( allowDelayForm )
				{
					DelayForm = true;
					ptb->Operand = nflds;
				}
				else
					nflds = ptb->Operand;
				break;

			case 720:							// do {block_name}
                btb = &btab[ptb->Operand];
                _dscreen(bt, btb->Screens->PT_start, btb->Screens->PT_end, allowDelayForm);		// recursion. descend into block for screen defs
				break;

			case 730:							// redisplay {variable/Table element}
				rdtb = RDARR(ptb->TABno);
                rdtb->RDTFlags = save_sf;
				break;
			
			case 674:							// set fnum , on|off	( 1 = on, 2 = off)
				fld_nbrs = ptb->Operand;
				break;
			
			case 1 :	// accept field
			case 3 :	// head 
				xtb = XTARR(ptb->TABno);
				clgetyx(xtb, &Column, &Line, yx_prompt);			// Get "Prompt" position on screen. yx_prompt = fixed value of 2
				exptobuf(Prompt, ENARR(xtb->Prompt_exp), 0);

				if ( first_fld && Prompt[0] )
                {
					if ( !DelayForm )
                    {
						whereat(1, Line, false, 0);			// don't allow mask bits
                        at();
                        erase_page(true);	// wipe displayed screen, *AND* internal buffers :o)
					}
                    first_fld = false;
				}
                if ( ptb->OpCode == 3 && !DelayForm)		// head
                {
					dhdtxt(xtb, true, 0);					// true == head command
				}
                else
                {
					//seems pointless. gettdfno() will clobber TTno anyway?
					gtd(ENARR(xtb->VarExpNo), &TTno);

					FieldNo = gettdfno(ENARR(xtb->VarExpNo), &TTno, &FLDtype, 0);
					assert(FieldNo >= 0);

                    // Add the >> Field Number nonsense to start of field
					if ( TTno == bt->TTno && ttab[TTno].TTfields[FieldNo].FLDstat & fld_KEY) //0x8000 Key field
                    {
						Column -= 3;
                        cdbcpystr(fnum_txt, ">> ", 0);
					}
					else if ( xtb->Flags & xtb_DISPLAY)		// 0x0004 display only field, zero the buffer
						fnum_txt[0] = 0;
                    else
                    {
						Column -= 3;
                        ++nflds;
                        sprintf(fnum_txt, "%2d ", nflds);	// insert Field Selection number (1-99)
                        xtb->FLDnum = nflds;				// Number displayed on screen when Field Numbering is *on*
					}
					//--------------------------------
                    if ( fld_nbrs == 2 && fnum_txt[0] )		// fld_nbrs == 2 means fnum set to "off"
                    {
						Column += 3;
						memset(fnum_txt, 0, 10);
					}
					//eprint("_dscreen - fnum_txt = %s, Prompt = %s\n",fnum_txt,Prompt);
                    if ( !DelayForm )
                    {
						if ( Column <= 0 )
							Column = 1;
						if ( Prompt && Prompt[0] )			// Prompt should always be non-zero, stack variable
                        {
							cdbcpystr(Dest, fnum_txt, Prompt, 0);
                            toscreen(Column, Line, 1, lenstr(Dest), Dest, xtb->ScrAttrib, ft_PROMPT, 0);	// 0x10 == Prompt field type
						}
					}
				}
				break;
		}
	ptb++;
	}
}

short dscreen(BTAB *btb, int ScreenNo, short CurrPTno)
{
    SCR		*scr;
	short	PT_start;

    nflds = 0;										// ** global var **
    if ( !isCGI )	// no screen fields when running as a cgi script. DOH!
    {
        first_fld = true;							// ** global var **
        
		scr = (SCR*)btb->Screens;
        for ( --ScreenNo; ScreenNo != -1; --ScreenNo )
            scr = scr->NextSCR;
        
		PT_start = CurrPTno ? CurrPTno : scr->PT_start;	// pass CurrPTno == 0 to layout whole screen

		save_sf = PT_start;							// ** global var **

        _dscreen(btb, PT_start, scr->PT_end, 1);
    }
    return nflds;	// ** global var **
}

#endif

