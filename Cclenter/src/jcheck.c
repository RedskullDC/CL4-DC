#ifndef JOINCHECK_C
#define JOINCHECK_C

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void dispjoin(PTAB *ptb, FLDdesc *fld, int TDno, bool show)
{
    XTAB	*xtb;
    RTAB	*rtb;
	int		fno;
	int		TTno;
    char	FLDtype;

    dispfld(XTARR(ptb->TABno), fld, show, 0, 1, 0);
    ptb++;

    while ( ptb->OpCode != 950 )							// look for 'break' at end of current block
    {
		if ( (unsigned short)(ptb->OpCode - 500) <= 98u )	// file operation.
        {
			rtb = RTARR(ptb->TABno);
            if ( rtb->TTno == TDno )						// if fileop is on same TD, no need to go any further!
				break;
		}
        if ( (unsigned short)(ptb->OpCode - 1) <= 48u )
        {
			xtb = XTARR(ptb->TABno);

			if ( xtb->Flags & xtb_JOIN);	// 0x0010 JOIN command follows this accept field
				return;

			if ( xtb->Flags & xtb_DISPLAY)	// 0x0004 DISPLAY ONLY field
            {
                fno = gettdfno(ENARR(xtb->VarExpNo), &TTno, &FLDtype, 1);
				assert(fno >= 0);
				
				if ( TTno == TDno )
                {
					xtb = XTARR(ptb->TABno);
					dispfld(xtb, &ttab[TDno].TTfields[fno], show, 0, 1, 0);
				}
			}
		}
        ptb++;
    }
}

// join check?

bool jcheck(PTAB *ptb, FLDdesc *fld, int wchar, int a4)
{
    RTAB	*rt;
	//ENTAB	*v10;
	//ENTAB	*v11;
	ENTAB	*v12;
	XTAB	*xt;
	TDesc	*TTptr;
	FLDdesc *v37;

	int		fno;
	bool	NOJOIN;
	bool	Forward;
	bool	WildChar;
	int		TTno;
	
	short	retcode;
	
    char	prmt[112];			// prompt string expression
	char	v46[64];
	char	format[64];
	ENTAB	ent;
	char	v36;
	char	FLDtype;
	
    xt = XTARR(ptb->TABno);
    if ( !(xt->Flags & (xtb_JOIN|xtb_NOJOIN)))	// 0x0110			// this field has no "join" constraint, return success!
        return true;
    
	NOJOIN = ((xt->Flags & xtb_NOJOIN) != 0);	// 0x0100			// 'nojoin' no match should exist
    
	WildChar	= ( wchar == '*' || wchar == '+' || wchar == '-' );	// Is last character a wildcard?  ( '*' == '+' )
	Forward		= WildChar && wchar != '-';							// Search direction if WildChar found

    rt  = RTARR(xt->RangeID);
	fno	= gettdfno(ENARR(rt->enleft), &TTno, &FLDtype, 1);
	assert(fno >= 0);
    
	v37 = &ttab[TTno].TTfields[fno];
    
	if ( !WildChar )
    {
        if ( fld->FLDstat & fld_ZERO )
        {
            rfree(rt->TTno, fno);
            dispjoin(ptb, fld, rt->TTno, false);
            return true;
        }
        fldtobuf(v46, fld, 0);			// are 64 chars really enough for some string fields?
    }
    
	memset((char*)&ent, 0, sizeof(ENTAB));		// Fudge our own local ENTAB record. :o)
    ent.enleft	= rt->enleft;
    ent.entype	= 2;
    ent.Enun.Enop.Enoper	= 0xA800u;			// insert '=' OpCode
    ent.enright	= rt->enright;
    calculate(&ent);
    
	TTptr = &ttab[TTno];
    TTptr->TTselect = (EXPR *)fretre(TTptr->TTselect, 0);
     
	short WhereEXP = rt->WhereEXP;
    if ( WhereEXP )	// Has a "where" selection been specified in join command?
    {
		short enleft = ENARR(WhereEXP)->enleft;
		if ( enleft )
			v12 = ENARR(enleft);
        else
            v12 = 0;									// error, will throw nil pointer exception
		
		v12->entype = 2;

		if ( fld->FLDtype != 'C' )						// override expression constraints
        {
            if ( WildChar )
				v12->Enun.Enop.Enoper = Forward ? 0x30u : 0x18u;	// 0x30 == greater than or equal '>=', 0x18 == less than or equal '<='
            else
                v12->Enun.Enop.Enoper = 0x10u;						// 0x10 == equal to	'='
        }
        TTptr->TTselect = getexpr(ENARR(rt->WhereEXP), TTno);	// "Where" expression specified in the join command. Use it!
    }
    else
        TTptr->TTselect = bldexpr(TTno, fno, wchar);			// Build an expression including all key fields up to fno
    
    if ( !WildChar || (v36 = 'p', Forward) )
        v36 = 'n';
    
	rt->OpType &= 0xFFF9;			// 1111 1111 1111 1001		
	if ( !WildChar || Forward )
        rt->OpType |= 0x0002;
    else
        rt->OpType |= 0x0004;

    retcode = cdbrestrict(rtdget(TTptr), TTptr->TTselect);
    if ( retcode >= 0 )
    {
        ent.enleft	= rt->enright;
        ent.enright = rt->enleft;
        exptobuf(prmt, ENARR(xt->Prompt_exp), 0);
        
		if ( prompt_0 )					// blow away old prompt string
            mfree_0(prompt_0);
        
        if ( Forward )
			prompt_0 = mstrcpy(prmt, " <y>es <n>ext <p>rev or <q>uit ", 0);
		else
			prompt_0 = mstrcpy(prmt, " <y>es <p>rev <n>ext or <q>uit ", 0);
        
		while ( v36 == 'n' || v36 == 'p' )
        {
            retcode = reffile(rt);
            if ( retcode )
            {
                if ( NOJOIN )
                    retcode = 0;
                else
                {
                    rfree(TTno, fno);
                    v36 = 120;	// lower case 'x' ?
                }
            }
            else if ( NOJOIN )
            {
				prerr("record exists", 0,1);
                return false;
            }

            v37->FLDstat &= 0xFFEFu;	// ~0x0010  == fld_DATA_AVAIL
            if ( !NOJOIN )
            {
                calculate(&ent);
                dispjoin(ptb, fld, TTno, retcode == 0);
            }
            if ( !retcode )
            {
                if ( WildChar )
                {
                    qat(1, _li - 1);					// Move to column 1 , last screen line

                    if ( v36 == 'n' )					// show prompt, get response
						v36 = reply(prompt_0, "npyq");
                    else
						v36 = reply(prompt_0, "pnyq");
                    erase_line();						// wipe prompt line
                    
					if ( v36 == 'q' )
                    {
                        rfree(TTno, fno);
                        v37->FLDstat &= 0xFFEFu;	// ~0x0010  == fld_DATA_AVAIL
                        calculate(&ent);
                        dispjoin(ptb, fld, TTno, false);
                    }
                    else if ( v36 == 'n' || v36 == 'p' )	// set direction flag
                    {
						rt->OpType &= 0xFFF9;				//  1111 1111 1111 1001		// clear bits 0x02 and 0x04
                        if ( v36 == 'p' )
							rt->OpType |= 0x0004;	// backwards
						else
							rt->OpType |= 0x0002;	// forwards
                    }
                }
                else
                {
                    v36 = 'y';						// indicate we want this record being shown
                }
            }
        } // while
    }
    else
    {
        dberror(retcode, 0, rtdget(TTptr));			// dead end error 
    }

    if ( v36 == 'y' && xt->Flags & xtb_LOCK && rdrec(TTno, 1) == -10 )
    {
        prerr("record locked", 0, 1);
        return false;
    }
    if ( retcode && v36 != 'q' && a4 )
    {
        if ( WildChar )
			prerr("no more in %s",TTptr->TableName,1);
        else
        {
            sprintf(format, "'%s' not found in %%s", v46);	// %%s converted to %s in format buffer
			prerr(format,TTptr->TableName, 1);				// prerr replaces it with 'Tablename'
        }
    }
    return v36 == 'y';
}


#endif

