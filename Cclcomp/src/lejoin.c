#ifndef LEJOIN_C
#define LEJOIN_C

#include <assert.h>
#include "lvarnames.h"			// for flag bits

int lejoin(short PTno, int join)
{
    PTAB	*ptab;
	ENTAB	*v12;
	ENTAB	*entb;
    XTAB	*xtb;
	RTAB	*rtb;
	
    int		TDno;
	short	v9;
	short	fno;
	short	RTno;
	char	FLDtype;

    ptab = PTARR(getptabp(PTno - 1));	// examine the *PREVIOUS* instruction
    allxtpt(ptab->TABno, &xtb);
    if ( ptab->OpCode != 1 )			// accept field
    {
		loaderr(20, sym);				// "previous field not accept field"
		dallxtpt(&xtb);
		return 0;
	}
    symbol = getsym();
    if ( symbol != 2230 )
    {
        if ( symbol == 930 )
			loaderr( 7, sym);			// "unexpected end of line"
		else
			loaderr(33, sym);			// "unexpected symbol"
        dallxtpt(&xtb);
        return 0;
	}
    v9 = getenmem();
    if ( !gettfexp(v9) )				// get table/field expression
	{
		dallxtpt(&xtb);
        return 0;
	}
	if ( xtb->RangeID )					// Not sure how to trigger this condition
    {
		loaderr(18, sym);				// "cannot join and range check"
        dallxtpt(&xtb);
        return 0;
	}
    
	RTno = getrtmem();
    allrtpt(RTno, &rtb);
    rtb->enleft = v9;
    rtb->enright = xtb->VarExpNo;
    if ( gettf(v9, &TDno, &FLDtype) < 0 )
    {
        __assert_fail("fno >= 0", "lejoin.c", 62, "lejoin");
	}
    rtb->TTno = TDno;
    xtb->RangeID = RTno;

	if (join)		// Join == 1 join, 0 nojoin
	{
		rtb->OpType = rtb_JOIN;			// 0x0040u;
		xtb->Flags |= xtb_JOIN;			// 0x0010;	// join
	}
	else
	{
		rtb->OpType = 0x0001u;			// same as GET
		xtb->Flags |= xtb_NOJOIN;		// 0x0100;	// nojoin
	}
    
	dallxtpt(&xtb);
    if ( symbol == 800 )				// "lock"
    {
		symbol = getsym();
        xtb->Flags		|= xtb_LOCK;	// 0x0020u;
        rtb->LockType	|= 0x0400u;
    }
    
	if ( symbol == 1720 )				// where
    {
		symbol = getsym();
        v9 = loadexp(1, 1);
        if ( !v9 )
        {
			dallrtpt(&rtb);
            return 0;
        }
        rtb->WhereEXP = v9;
        
		v9 = getenmem();
        allenpt(v9, &entb);
        entb->Enun.Enop.Enoper	= 2;		// AND?
        entb->entype			= 2;
        entb->enright			= rtb->WhereEXP;
        entb->enleft			= getenmem();
        rtb->WhereEXP			= v9;
        dallenpt(&entb);

		entb = ENARR(rtb->WhereEXP);
		
		allenpt(entb->enleft, &entb);
        entb->Enun.Enop.Enoper	= 16;				// relational operator
        entb->entype = 2;
//----------------------------
		entb->enleft = getenmem();
		fno = gettf(rtb->enleft, &TDno, &FLDtype);
		assert(fno >= 0);

		v12						= ENARR(entb->enleft);
		v12->Enun.Enref.TTno	= TDno;
        v12->Enun.Enref.VarNum	= fno;
		v12->entype				= 1;
//-----------------------------
		entb->enright = getenmem();
		fno = gettf(rtb->enright, &TDno, &FLDtype);
		assert(fno >= 0);

		v12						= ENARR(entb->enright);
		v12->Enun.Enref.TTno	= TDno;
		v12->Enun.Enref.VarNum	= fno;
		v12->entype				= 1;

		dallenpt(&entb);
	}
    dallrtpt(&rtb);
    if ( symbol != 930 )
    {
		loaderr(33, sym);
		return 0;
	}
    return 1;	// exit_success
}

#endif
