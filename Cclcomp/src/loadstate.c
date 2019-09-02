#ifndef LOADSTATE_C
#define LOADSTATE_C

#include <stdio.h>
#include <stdarg.h>				// for var args stuff
#include <stdlib.h>				// for strtod()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

// called from leblock() initially, can be recursive
bool loadstate(int *LineNo)
{
	ENTAB	*enta;
	ENTAB	*entb;
	PTAB	*ptab1;
	FLDdesc *fld;

	int		FieldNo;
	int		Join;
	int		chkINT;
	int		TTno;

	short	TABno;
	short	ENTno;
	short	expright;
	short	v112;
	short	FLDstat;

	bool	AuxError;

	char	Blockname[64];
	char	FieldType;

//eprint("loadstate called - *LineNo = %d\n",*LineNo);
	if ( comp_abort )
		return false;

	while ( symbol == 930 )					// <CR>  skim through blank lines
		symbol = getsym();
        
	AuxError = false;						// default to exit_success
	switch (symbol)
	{
	case 0:									// no symbol found, usually means EOF
		return false;						// error_exit
		break;

	case 660:								// include
		if ( leincl())
			return true;
		break;

	case 510:
		if ( leformat(LineNo, true))		// NoBreak = true
			return true;
		break;

	case 55:								// replace
	case 160:								// clear
	case 300:								// delete
	case 580:								// getnext
	case 590:								// getprev
	case 600:								// get
	case 1220:								// put
		if ( lerfile(LineNo, symbol))		// This is a table type operation
			return true;
		break;

	case 1430:								// set
		if ( leset(LineNo) )
			return true;
		break;

	case 1260:								// range
		if ( lerange(*LineNo))
			return true;
		break;

	case 1680:								// unlock
		if ( leulck(LineNo))
			return true;
		break;

	case 990:								// on_eop
		if ( leeop(*LineNo))
			return true;
		break;

	case 420:								// escape
	case 1020:								// on_key
		if ( leescape(*LineNo, symbol))
			return true;
		break;

	case 620:								// goto
		symbol = getsym();
        allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 700;
        ptab1->SrcLineNo = lla;
        if ( symbol == 720 )				// LABEL type.   string followed by colon.  "HELLO:"
		{
			ptab1->TABno = getstrmem(syml + 1);	// allow room for \0
			cdbcpystr(STRARR(ptab1->TABno), sym, 0);
			symbol = getsym();					// advance to next symbol
		}
		else
		{
			loaderr(44, sym);				// "invalid label"
            AuxError = true;
		}
		dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 650:									// if
		if ( !leif(LineNo))
        {
			AuxError = true;
            while ( symbol != 390 && symbol )	// 390 == endif
				symbol = getsym();
    		if ( symbol )
				symbol = getsym();
		}
		if ( !AuxError )
			return true;
		break;

	case 1730:									// while
		++while_count;
		if ( !lewhile(LineNo) )
		{
			AuxError = true;
            while ( symbol != 1710 && symbol )	// wend
				symbol = getsym();
			if ( symbol )
				symbol = getsym();
		}
		while_count--;
		if ( !AuxError )
			return true;
		break;

	case 1610:									// text
		symbol = getsym();
		allptpt(getptabp((*LineNo)++), &ptab1);
		ptab1->OpCode	 = 4;
		ptab1->SrcLineNo = lla;
		AuxError = (!(ptab1->TABno = lehdtxt()));
	    dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 950:									// no_join
	case 690:				                    // join
		if (symbol == 950)
			Join = 0;		// 950 no_join
		else
			Join = 1;
		if ( lejoin(*LineNo, Join))
			return true;
		break;

	case 400:									// endloop
	case 210:									// continue
		if ( while_count )			// ensure we are inside a while loop!
        {
			allptpt(getptabp((*LineNo)++), &ptab1);
			
			if ( symbol == 210 )
				ptab1->OpCode = 320;	// continue
			else
				ptab1->OpCode = 330;	// endloop

			ptab1->SrcLineNo	= lla;
			ptab1->Operand		= 0;
			symbol = getsym();
			if ( symbol != 930 )
			{
				loaderr(33, sym);		// unexpected symbol
				AuxError = true;
			}
			dallptpt(&ptab1);
			if ( !AuxError )
				return true;
		}
		else
			loaderr(78, sym);	// "'continue/endloop' must be inside 'while' block

		break;

	case 350:								// do {blockname}
		symbol = getsym();		// get blockname
        allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 720;
        ptab1->SrcLineNo = lla;
        if ( symbol == 2230 )	// string literal  blockname
        {
			ptab1->TABno = getstrmem(fixbname(Blockname, sym));		// ensure max length = 21 [including \0]
			cdbcpystr(STRARR(ptab1->TABno), Blockname, 0);
			symbol = getsym();
		}
		else
		{
			if ( symbol == 930 )
				loaderr(33, sym);		// unexpected symbol
			else
				loaderr(2 , sym);		// "missing block name"
			AuxError = true;
		}
		dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 402:							// endtran
		allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 340;
        ptab1->SrcLineNo = lla;
        dallptpt(&ptab1);
        symbol = getsym();
        if ( symbol == 930 )
			return true;
		else
			loaderr(33, sym);			// unexpected symbol
		break;

	case 870:							// message
		allptpt(getptabp((*LineNo)++), &ptab1);
		ptab1->OpCode	 = 270;
		ptab1->SrcLineNo = lla;
		AuxError = !lemsg(ptab1);
        dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 90:							// exit_block
	case 310:							// exit_do
	case 1090:							// exit_process
	case 1240:							// restart_block
        allptpt(getptabp((*LineNo)++), &ptab1);

		if ( symbol == 90 )				// exit_block
			ptab1->OpCode = 882;
        else if ( symbol == 310 )		// exit_do
			ptab1->OpCode = 884;
        else if ( symbol == 1090 )		// exit_process
			ptab1->OpCode = 886;	
        else if ( symbol == 1240 )		// restart_block
			ptab1->OpCode = 888;

		ptab1->SrcLineNo = lla;
		dallptpt(&ptab1);
		symbol = getsym();
		if ( symbol == 930 )				// expect <CR> to follow
			return true;
		else
			loaderr(33, sym);				// unexpected symbol
		break;

	case 1060:								// pause
		allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 870;
		ptab1->SrcLineNo = lla;
		AuxError = !lepause(ptab1);
        dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 500:								// fork
		allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 750;
        ptab1->SrcLineNo = lla;
		AuxError = !lefork(ptab1);
        dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 430:								// openapp
	case 460:								// openin
	case 530:								// openout
		allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 860;
        ptab1->SrcLineNo = lla;
		AuxError = (!(ptab1->TABno = leopen(false)));	// true passed to leopen() indicates a Pipe, false == file
	    dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 1460:									// select
		allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 560;
        ptab1->SrcLineNo = lla;
        TABno = leselect();
        if ( !TABno )
        {
			AuxError = true;
			ptab1->OpCode = 400;			// no-op 
		}
        ptab1->TABno = TABno;
        dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 1320:									// redisplay
		allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 730;
        ptab1->SrcLineNo = lla;
        symbol = getsym();
		AuxError = (!(ptab1->TABno = leredisp()));
	    dallptpt(&ptab1);
		if ( !AuxError )
			return true;	// exit_success
		break;

	case 230:								// copy
	case 240:								// create
		allptpt(getptabp((*LineNo)++), &ptab1);
		ptab1->SrcLineNo = lla;
		switch (symbol)
		{
		case 230:
            ptab1->OpCode	 = 800;			// copy
			AuxError = (!(ptab1->TABno = lecopy()));
			break;
		case 240:
            ptab1->OpCode	 = 450;			// create
			AuxError = (!(ptab1->TABno = lecreate()));
			break;
		}
	    dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 1350:								// refresh
	case 1390:								// screen
	case 1470:								// sleep
	case 1510:								// srandom
		allptpt(getptabp((*LineNo)++), &ptab1);
		ptab1->SrcLineNo = lla;
		switch (symbol)
		{
		case 1350:
            ptab1->OpCode	 = 250;
			AuxError = !lefresh(ptab1);
			break;
		case 1390:
            ptab1->OpCode	 = 260;
			AuxError = !lescreen(ptab1);
			break;
		case 1470:
			ptab1->OpCode	 = 880;
			AuxError = !lesleep(ptab1);
			break;
		case 1510:
            ptab1->OpCode	 = 760;
			AuxError = !lesrand(ptab1);
			break;
		}
        dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 440:								// 440 = close
	case 540:								// 540 = read
	case 560:								// 560 = write
		allptpt(getptabp((*LineNo)++), &ptab1);
		ptab1->OpCode	 = 860;
		ptab1->SrcLineNo = lla;
		AuxError = (!(ptab1->TABno = leread(false)));	// isPipe = false
		dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 940:								// 940 = delay_form
		allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 265;
		ptab1->SrcLineNo = lla;
		dallptpt(&ptab1);
		symbol = getsym();
		if ( symbol == 930 )
			return true;
		else
			loaderr(33, sym);			// unexpected symbol
		break;

	case 1110:							// 1110 = popenin
	case 1120:							// 1120 = popenio
	case 1150:							// 1150 = popenout
		allptpt(getptabp((*LineNo)++), &ptab1);
		ptab1->OpCode	 = 850;
		ptab1->SrcLineNo = lla;
		AuxError = (!(ptab1->TABno = leopen(true)));	// true passed to leopen() indicates a Pipe, false == file
	    dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;

	case 1190:							// 1190 = print
		allptpt(getptabp((*LineNo)++), &ptab1);
        ptab1->OpCode	 = 900;			// print
        ptab1->SrcLineNo = lla;
        symbol = getsym();
		AuxError = (!(ptab1->TABno = leprint()));
	    dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;
/*
	case 1180:							// ??		Appear to be unreachable. Old versions of popen.. cmds ?
	case 1230:							// ??		1230 like 540 write in leread()
		allptpt(getptabp((*LineNo)++), &ptab1);
		ptab1->OpCode = 850;			// Same opcode as popenio, popenin, popenout
		ptab1->SrcLineNo = lla;
		AuxError = (!(ptab1->TABno = leread(1)));
	    dallptpt(&ptab1);
		if ( !AuxError )
			return true;
		break;
*/
	case 130:							// 130 = clear_eol
	case 140:							// 140 = clear_eos
		allptpt(getptabp((*LineNo)++), &ptab1);
		if (symbol == 140)
            ptab1->OpCode = 922;		// clear_eos
		else
            ptab1->OpCode = 920;		// clear_eol
        ptab1->SrcLineNo = lla;
        dallptpt(&ptab1);
		symbol = getsym();
		return true;					// always succeeds
		break;

	case 720:		// we hit a LABEL: as an lvalue
		//printf("720 Label = %s, lti = %d\n", sym, lti);
		for ( v112 = 0 ; v112 < lti ; v112++)
		{
			if ( cmpbuf(sym, ltab[v112]->LabelName, ltab[v112]->NameLen) )	// returns true if match
			{
				loaderr(16, sym);		//"duplicate label"
				AuxError = true;
				break;
			}
		}
		if ( !AuxError )
		{
			if ( lti < no_ltabs || (newltab(), lti < no_ltabs) )
			{
				++lti;
				*lt = (LTAB *)getmem(syml + sizeof(LTAB));	// allocate enough to hold labelname , sizeof(LTAB) = 6
				(*lt)->NameLen = syml;
				(*lt)->PTno = *(short*)LineNo;
				cdbcpystr((*lt)->LabelName, sym, 0);
				++lt;
				symbol = getsym();
				return true;			// exit_success
			}
			__assert_fail("lti < no_ltabs", "loadstate.c", 221, "loadstate");
		}
		break;

	case 4000:								// openlib		*** DC extension ***
		if ( leopenlib(LineNo))
			return true;
		break;

	case 4010:								// libcall()	*** DC extension ***
		if ( lelibcall(LineNo))
			return true;
		break;

	case 2230:								// we hit a table.field/variable name as an lvalue
		allptpt(getptabp((*LineNo)++), &ptab1);
		ptab1->SrcLineNo = lla;
		ENTno = getenmem();
		allenpt(ENTno, &entb);
		if ( !gettfexp(ENTno) )				// get Table/Field expression
		{
			ptab1->OpCode = 400;			// no-op?
			AuxError = true;
            dallenpt(&entb);
            dallptpt(&ptab1);
			break;
		}
		switch (symbol)
		{
		// we found an accept field
		case 50:							// "+"
		case 930:							// <CR> same as using '=' defaults for all values
		case 2200:							// Integer literal
		case 2230:							// TableField/normal variable
			TABno = leaccept(ENTno);
            if ( TABno )
			{
				ptab1->OpCode = 1;			// Accept field
                ptab1->TABno  = TABno;
				if ( yflag )				// debug output. Show width of DateFields
					prdatefield(ENTno, TABno, 0);
			}
			else
				AuxError = true;

			dallenpt(&entb);
			dallptpt(&ptab1);
			if ( !AuxError )
				return true;			// exit sucess!
			break;
		
		// variable assignment follows
		case 40:							//  +=
		case 320:							//  /=
		case 850:							//  %=
		case 880:							//  *=
		case 1160:				            //  **=
		case 1560:							//  -=
		case 1800:							//  "=" or "eq" or "=="
			FieldNo = gettfs(ENTno, &TTno, &FieldType, &FLDstat);
			assert(FieldNo >= 0);

			chkINT = ( FieldType == 'B' || FieldType == 'I' || FieldType == 'N' ) ? 1 : 0 ;	// check in loadexp for integer targets
            
			v112 = getenmem();
            allenpt(v112, &enta);
            enta->enleft = ENTno;
			switch (symbol)
			{
			case 40:					// "+="
				enta->TTno = f_ADDEQ;
				break;
			case 320:					// "/="
				enta->TTno = f_DIVEQ;
				break;
			case 850:					// "%="
				enta->TTno = f_MODEQ;
				break;
			case 880:					// "*="
				enta->TTno = f_MULEQ;
				break;
			case 1160:					// "**="
				enta->TTno = f_POWEQ;
				break;
			case 1560:					// "-="
				enta->TTno = f_SUBEQ;
				break;
			case 1800:					// "=" or "==" or "eq"
				enta->TTno = f_ASSIGN;
				break;
			default:
				assert(0);// should be unreachable
				break;
			}
            if ( FLDstat & fld_READ_ONLY ) 	// Only possible for SYSTEM variables. FLDstat not accesible for user vars
			{
				loaderr(83, sym);			// "can't assign to read-only variable"
                AuxError = true;
			}
			else
			{
				enta->entype = 2;
				//if ( FieldType == 'C' && isnumop((unsigned short)enta->TTno) )	// *** real CL4 doesn't pass variable correctly, msg not displayed ***
				//	warncexp();				// "+-*%%/ will be unsupported on strings in future releases\n"
				   
				symbol = getsym();
				if ( symbol == 930 )
				{
					loaderr(7, sym);		//  "unexpected end of line"
					AuxError = true;
				}
				else
				{
					ptab1->OpCode = 200;		// assignment operator
					expright = loadexp(0, chkINT);
					if ( expright )
					{
						enta->enright = expright;						// rvalue
						if ( chkINT && intexp(enta->enright) )
							enta->TTno |= 0x0200;						// integer calcs flag for clenter
						
						else if ( FieldType != 'C' && ( dpexp(enta->enleft) < dpexp(enta->enright) )) // LVALUE has less decimal places than RVALUE. Possible rounding issues occur.
							enta->TTno |= 0x0040u;						// ??? generates 'RND' in prdebug output.  Never read in clenter?

						if ( symbol == 930 )							// <CR> should be next
						{
							ptab1->TABno = v112;						// All ok, point program instruction at assign expression
							if ( TTno )
								fld = &ttab[TTno].TTfields[FieldNo];	// Table Field
							else
								fld = getvars(FieldNo);					// Normal variable
							fld->FLDstat |= 0x0008u;					// Indicate that is variable/field is written to. For prdebug() Not used in clenter(?)
						}
						else
						{
							loaderr(33, sym);			// " unexpected symbol"
							AuxError = true;
						}
					}
					else
						AuxError = true;
				}
			}
			dallenpt(&enta);
			dallenpt(&entb);
			dallptpt(&ptab1);
			if ( !AuxError )
				return true;		// exit success!
			break;

		default:
			if ( symbol == 930 )
				loaderr(7 , sym);	// "unexpected end of line"
			else
				loaderr(33, sym);	// " unexpected symbol"
			AuxError = true;
            dallenpt(&entb);
            dallptpt(&ptab1);
			break;
		}
		break;

	default:
		loaderr(33, sym);	// "unexpected symbol" error exit
        symbol = getsym();
		break;
	}

// general exit
	if ( comp_abort )		// We have had more than 20 soft errors, or reached end of file unexpectedly
		return false;	// error_exit
	
	while ( symbol != 930 && symbol )	// symbol == 0 may mean EOF
		symbol = getsym();

	return true;	// exit_success
}


#endif
