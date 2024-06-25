#ifndef PROG_LOOP_C
#define PROG_LOOP_C

#include <stdio.h>
#include <stdarg.h>				// for var args stuff
#include <stdlib.h>				// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

bool tojoin(BTAB *btb, XTAB *xtb)
{
    //RTAB *rtb; // eax@9
    //RTAB *rtb2; // eax@13
    //ENTAB *v5; // eax@15
    int TDno; // [sp+10h] [bp-18h]@17
    int TTno; // [sp+14h] [bp-14h]@4
    char FLDtype; // [sp+1Bh] [bp-Dh]@4

    gettdfno(ENARR(xtb->VarExpNo), &TTno, &FLDtype, 0);

    //return btb->Curr_Mode != 'a' || xtb->Flags >= 0 || btb->TTno != TTno || xtb->Flags & 0x10 && (!xtb->RangeID ? (rtb = 0) : (rtb = &rtarr.TableAddr[xtb->RangeID - 1]), !rtb->enleft ? (v5 = 0) : (!xtb->RangeID ? (rtb2 = 0) : (rtb2 = &rtarr.TableAddr[xtb->RangeID - 1]), v5 = &enarr.TableAddr[rtb2->field_C - 1]), gettdfno(v5, &TDno, &FLDtype, 0), TTno != TDno);
    //return btb->Curr_Mode != 'a' || xtb->Flags >= 0 || btb->TTno != TTno || xtb->Flags & 0x10 && (rtb = RTARR(xtb->RangeID), v5 = ENARR(rtb->enleft) , gettdfno(v5, &TDno, &FLDtype, 0), TTno != TDno);
    //return btb->Curr_Mode != 'a' || xtb->Flags >= 0 || btb->TTno != TTno || xtb->Flags & 0x10 && (rtb = RTARR(xtb->RangeID), gettdfno(ENARR(rtb->enleft), &TDno, &FLDtype, 0), TTno != TDno);

    return btb->Curr_Mode != 'a' || xtb->Flags >= 0 || btb->TTno != TTno || xtb->Flags & 0x10 && (gettdfno(ENARR(RTARR(xtb->RangeID)->enleft), &TDno, &FLDtype, 0), TTno != TDno);

}

short ploop(BTAB *btb, int ScreenNum, short PT_Start2, short PT_End2, short PT_Start, short PT_End, int a7)
{

    if ( a7 && !ScreenNum )
    {
        key_found = 0;
        key_i	  = 0;
    }

	if ( go_back == -22 )		// ** global var **
        skip = 1;
    else
		skip = (ScreenNum > 0 && a7 && (btb->Curr_Mode == 'c' || btb->Curr_Mode == 'v')) ? 1 : 0;

	return _ploop(btb, ScreenNum, PT_Start2, PT_End2, PT_Start, PT_End, false, false);		// allow screenout, not indoblock
}

short sploop(BTAB *bt, int ScreenNum, short PT_Start2, short PT_End2, int PT_Start, int PT_End)
{
    int		save_key_found;
    int		save_skip;
    int		save_in_do;
    short	result;
    short	save_depth;

    if ( recur_0 == 1 )
        result = 128;
    else
    {
        save_key_found	= key_found;
        save_skip		= skip;
        save_in_do		= in_do;
        save_depth		= depth;
		key_found		= 1;
        skip			= 1;
        
		++recur_0;
        result = _ploop(bt, ScreenNum, PT_Start2, PT_End2, PT_Start, PT_End, true, false);	// no screenout, not indoblock
        --recur_0;
        
		if ( result != 1 )
            save_key_found = 0;
        
		key_found	= save_key_found;
        skip		= save_skip;
        in_do		= save_in_do;
        depth		= save_depth;
    }
    return result;
}

short _ploop(BTAB *bt, int ScreenNum, short PT_Start2, short PT_End2, short PT_Start, short PT_End, bool noScreenOut, bool inDoBlock)
{
	PTAB	*ptab;	// Program statement records
	XTAB	*xtab;	// screen display record
	XFTAB	*xftb;	// xfer table <- table record
	RDTAB	*rdtab;	// redisplay record
    
	BTAB	*btb;
	
    ENTAB	*v31;
	
    FLDdesc *fldREPLY;
	FLDdesc *fldMREPLY;
	FLDdesc *fldLREPLY;

	FLDdesc *fld;
	FLDdesc fld1;
	
	signed int OpCode;
	int		v44;
	int		v48;
	int		v68;
	int		save_skip;
	int		save_key_found;
	
	short	v15;
	short	v51;
	short	v62;
	short	v63;
	short	v64;
	short	v65;
	short	v66;
	short	CurrPTno;
    bool	v67;
	char	v74[32];
	char	Dest[32];
	char	v76[1008];
	char	v77[1024];
	
	char	v61;
	
	//printf("_ploop: entry PT_Start2 = %d, PT_End2 = %d\n", PT_Start2,PT_End2 );

    fld			= 0;
    save_skip	= 0;
    v67			= false;
    CurrPTno	= PT_Start2;
    while ( CurrPTno <= PT_End2 )
    {
        while ( 1 )		//---- inner while loop ----
        {
            ptab		= PTARR(getptabp(CurrPTno));
            srclineno	= ptab->SrcLineNo;

            if ( (unsigned short)(ptab->OpCode - 1) > 48u ) // Inner loop deals with OpCodes 1 - 50
                break;
            
			xtab = XTARR(ptab->TABno);
            if ( xtab->VarExpNo )
                fld = getftf(ENARR(xtab->VarExpNo), 1, &fld1);

			if ( ptab->OpCode == 4 )            // 4 == "text"
            {
                dhdtxt(xtab, false, 0);			// false == text command
                fflush(stderr);
                goto LABEL_263;		// next instruction
            }
//----------------
// join checks
			//if (xtab)
			//	printf("_ploop: 1777 - xtab->Flags = x%04X, key_found = %d\n",(unsigned short)xtab->Flags, key_found);

			if ( xtab->Flags & xtb_DISPLAY || key_found && xtab->Flags & xtb_KEY )
            {
                dispfld(xtab, fld, 1, 0, 1, 0);
                if ( !key_found )				// ** global var **
                {
                    if ( tojoin(bt, xtab) )
                    {
                        if ( !jcheck(ptab, fld, 0, 1) )
                            sleep(5u);
                        goto LABEL_263;
                    }
                    if ( !key_found )			// ** global var **. Volatile, may change from test above
					{
			            jcheck(ptab, fld, 0, 1);
		                goto LABEL_263;
					}
                }
				if ( !(xtab->Flags & xtb_KEY) )
					jcheck(ptab, fld, 0, 1);
				else if ( !rcheck(xtab, fld, 0) || !jcheck(ptab, fld, 0, 1) )
					return 256;
				goto LABEL_263;
			}
//--------------
            if ( ptab->OpCode != 1 )            // accept field
                goto LABEL_263;

//printf("_ploop: 1809 - CurrPTno = %d, PT_Start = %d, PT_End = %d\n",CurrPTno,PT_Start,PT_End);
            if ( PT_Start <= CurrPTno && CurrPTno <= PT_End )
                goto LABEL_33;
            
			if ( !xtab->RangeID )
            {
                //if ( skipping != 2 && (!(xtab->Flags & 0x0400) || !(fld->FLDstat & 0x0004)) )
                if ( skipping != 2 && (!(xtab->Flags & xtb_REQUIRED) || !(fld->FLDstat & fld_ZERO)) )
                {
                    if ( fld->FLDstat & 8 )	// ????
                        dispfld(xtab, fld, 1, 0, 1, 0);
                    goto LABEL_263;			// jump to next instruction
                }
LABEL_33:
                if ( !xtab->RangeID )
                    goto LABEL_37;
            }
            if ( CurrPTno < PT_Start || CurrPTno > PT_End )
            {
                save_skip = skip;		// Current PTno is outside the block range!
                skip = 1;
            }

//----------------
LABEL_37:
            if ( key_found && xtab->Flags & xtb_KEY )
                v15 = 1;
			else
            {
                if ( go_back )
                {
                    if ( xtab->FLDnum == back_to )
                        skip = 0;
                    else if ( xtab->FLDnum >= lback_to )
                    {
						skip = skip_gb;
                        setcvar(v_LREPLY, lreply_gb);	// "LREPLY"
                        lback_to = 0;
                        go_back = 0;
					}
                }
                v15 = (signed short)pedit(xtab, ptab, fld, bt, &skip, inDoBlock, CurrPTno, noScreenOut);
            }

            if ( xtab->RangeID && (CurrPTno < PT_Start || PT_End < CurrPTno) )
                skip = save_skip;

// -- deal with pedit() return code --
			switch (v15)
			{
			case 0 :					// User entered blank for the key field. return to caller
                return 0;
				break;

			case -5:					// <crtl>e
			case -17:					// <ctrl>a
                go_back = 0;
                setcvar(v_LREPLY, "q");	// "LREPLY"
                return v15;
				break;

			case -22:					// User typed up-arrow or equivalent
				if ( !go_back )
                {
					go_back		 = -22;
                    lback_to	 = xtab->FLDnum;
                    skip_gb		 = skip;
                    lreply_gb[0] = *(char*)ttab->TTfields[v_LREPLY].FLDdata;
                    setcvar(v_LREPLY, "m");				// "LREPLY"
				}
                back_to = last_fld;
                return v15;
				break;

			case 64:
	            if ( bt->Curr_Mode != 'a' )
	                skip = 1;

	            key_found = 1;
	            if ( bt->Curr_Mode == 'a' )
				{
	                if ( xtab->Flags & xtb_LKF)			// LKF Last Key Field
	                    v67 = true;
	                goto LABEL_263;
				}
	            CurrPTno = PT_Start2;
				break;

			case 128:
				key_found = 0;
				return 128;
				break;

			default:
				//printf("_ploop: default 298 - v15 = %d\n",v15); 
                last_fld = xtab->FLDnum;				// what case is this???		:	1 comes here
                if ( xtab->Flags & xtb_LKF)				// LKF Last Key Field
                    v67 = true;
                goto LABEL_263;		// next instruction
				break;
			}

        }// end of inner while loop ----

//=================================

        OpCode = ptab->OpCode;
		switch (OpCode)
		{
		case 200:										// 200 = assignment : X=Y
			calculate(ENARR(ptab->TABno));				// do the actual assignment
			if ( debug == 1 )
				prdebass(ptab->SrcLineNo, ENARR(ENARR(ptab->TABno)->enleft), CurrPTno);	// show Debug assignment
			break;
		case 250:										// refresh {screen_save_name}
			pfresh(ptab->TABno ? &strarr.StringTable[ptab->TABno - 1] : 0);
			break;
		case 260:										// screen {screen_save_name}
			pscreen(ptab->TABno ? &strarr.StringTable[ptab->TABno - 1] : 0);
			break;
		case 265:										// no_form
			if ( ptab->Operand )
            {
				_dscreen(bt, CurrPTno, PT_End2, 0);
				ptab->Operand = 0;
			}
			break;
		case 270 :										// message
            if ( noScreenOut )
				return 1;
			
			v31 = ENARR(ptab->TABno);
			exptobuf(v77, ENARR(v31->enleft), 0);		// expecting 2 string values
			exptobuf(v76, ENARR(v31->enright), 0);
            
			fldREPLY  = &ttab->TTfields[v_REPLY];		// "REPLY"
            fldLREPLY = &ttab->TTfields[v_LREPLY];		// "LREPLY"
            fldMREPLY = &ttab->TTfields[v_MREPLY];		// "MREPLY"

            cdbcpystr(Dest, (char *)fldLREPLY->FLDdata, 0);
            cdbcpystr(v74, (char *)fldREPLY->FLDdata, 0);
            *(char *)fldREPLY->FLDdata = v76[0];

            mreply(v77, v76);
            
			cdbcpystr((char *)fldMREPLY->FLDdata, (char *)fldLREPLY->FLDdata, 0);
            cdbcpystr((char *)fldLREPLY->FLDdata, Dest, 0);
            cdbcpystr((char *)fldREPLY->FLDdata, v74, 0);
			break;
		case 300:										// 300 = if
		case 310:										// 310 = while
			if ( evaliex(ENARR(ptab->TABno), skip))		// evaluate expression
			{	
			    if ( debug == 1 )
			        prdebcon(ptab->SrcLineNo, true, CurrPTno);	// show debug condition
			}
			else										// condition false. continue execution after end of loop
			{	
				if ( debug == 1 )
				    prdebcon(ptab->SrcLineNo, false, CurrPTno);	// show debug condition
				CurrPTno = ptab->Operand - 1;			// condition false, exit loop. CurrPTno is incremented at end of loop
			}
			break;
		case 410:										// openlib()  DC extension.
			if (!popenlib(ptab->TABno))
			{
				printf("openlib ERROR, can't open library!");
				longjmp(sjbuf, 1);	// jump back up to main() and exit if lib can't be opened
			}
			break;
		case 420:										// openlib()  DC extension.
			if (!plibcall(ptab->TABno))
			{
				printf("Library call error!");
				longjmp(sjbuf, 1);						// jump back up to main() and exit if lib can't be opened
			}
			break;
		case 450:										// 450 = create [temp table]
            pcreate(KXARR(ptab->TABno));
			break;

		case 510:		// 510 get
		case 515:		// 515 getprev
		case 520:		// 520 getnext
		case 530:		// 530 put/replace
		case 560:		// 560 select
		case 570:		// 570 delete
		case 580:		// 580 clear
			reffile(RTARR(ptab->TABno));
			break;
		
		case 730:										// redisplay()
            rdtab = RDARR(ptab->TABno);
            redisp(rdtab, rdtab->RDTFlags, CurrPTno);
			break;
		case 750:										// fork
			pfork(ptab);
			break;
		case 760:										// rand()
		    psrand(ENARR(ptab->TABno));
			break;
		case 800:										// copy td from td2
	        xftb = XFARR(ptab->TABno);
		    tdtotd(xftb->TTno_to, xftb->TTno_from, 0, 2);
			break;
		case 850:										// openin, read etc.
		case 860:
		    pfile(SFARR(ptab->TABno));	// SFTAB*
			break;
		case 870:										// pause
		    if ( noScreenOut )			// Do nothing if output is piped to a file or ....
		        return 1;
		    ppause(ENARR(ptab->TABno));
			break;
		case 880:										// sleep ()
			psleep(ENARR(ptab->TABno));
			break;
		case 882:										// exit_block
			exit_type = 882;
			return -5;
			break;
		case 884:										// exit_do
			if ( inDoBlock )
				return 0;
			break;
		case 886:										// exit_process
			exit_type = 886;
			return -5;
			break;
		case 888:										// restart_block
		    return -17;
			break;
		case 900:										// print
			prfield(PRARR(ptab->TABno));	// PRTAB*
			break;
		case 910:										// unlock
			v51 = ulckrec(ptab->Operand);
			setvar(v_ERROR, v51 == 0);					// "ERROR"
			break;
		case 920:
		    erase_line();								// clear to end of current line
			break;
		case 922:
			at();										// home cursor, and wipe screen
			erase_page(true);							// wipe displayed screen, *AND* internal buffers :o)
			break;
		case 720:										// do {blockname}
			btb = &btab[ptab->Operand];
			
			in_do = 1;
			++depth;			// increase sub-routine count
			
			if ( PT_Start == PT_End )
				v15 = _ploop(bt, ScreenNum, btb->StartLine, btb->EndLine, PT_Start, PT_Start, noScreenOut, true);	// true indicates in a "do" block
			else
				v15 = _ploop(bt, ScreenNum, btb->StartLine, btb->EndLine, btb->StartLine, btb->EndLine, noScreenOut, true);	
			
			depth--;
			if ( depth == 0)	// are we still in a sub-routine?, or back at top block
				in_do = 0;
			
			if ( v15 == -17 || v15 == -5 || v15 == -22 )
			    return v15;
			break;
		case 600:										// format {blockname}

			v63				= last_fld;					// save current state variables
			v68				= skip_gb;
			v66				= go_back;
			v65				= back_to;
			v64				= lback_to;
			v61				= lreply_gb[0];
			v62				= depth;
			save_key_found	= key_found;
			v44				= in_do;
			save_skip		= skip;
			skip_gb			= 0;
			lback_to		= 0;
			back_to			= 0;
			go_back			= 0;
			in_do			= 0;
			
			btb = &btab[ptab->Operand];
			if ( bt->Scrs & 0x4000 || bt->Curr_Mode == 'r' || ScreenNum != (bt->Scrs & 0xBFFF) - 1 )
			    btb->Scrs |= 0x4000;
			else
			    btb->Scrs &= 0xBFFF;
			
			v48 = pblock(btb, ENARR(ptab->TABno));		// call "format" style block with MODE variable specifier
			if ( v48 == -5 )
			    return -5;
			
			// restore state
			depth		 = v62;
			key_found	 = save_key_found;
			in_do		 = v44;
			skip		 = save_skip;
			last_fld	 = v63;
			skip_gb		 = v68;
			go_back		 = v66;
			back_to		 = v65;
			lback_to	 = v64;
			lreply_gb[0] = v61;
			break;

		case 320:		// 320 = continue
		case 330:		// 330 = end_loop
		case 700:		// 700 = goto
			if ( noScreenOut && v67 )
            {
                if ( ptab->Operand < CurrPTno )
					return 128;
			}
            if ( key_found && key_i < CurrPTno && ptab->Operand < key_i )
            {
				key_found = 0;
                if ( skipping == 2 )
					return 128;
			}
            CurrPTno = ptab->Operand - 1;	// update line counter for next pass
			break;
		case 340:
			endtran();
			break;
		
		default:							// default, fall through to pset(). Break 950 also comes here?
		    pset(ptab);
			break;
		}

LABEL_263:	
        CurrPTno++;		// real end of the block. Check if we are past the last line in the block ( auto-return)
    } //  end while ( CurrPTno <= PT_End2 )
	return 1;	// exit success
}

#endif

