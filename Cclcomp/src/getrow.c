#ifndef GETROW_C
#define GETROW_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>         // for strtol()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

XTAB *getrow(XTAB *xtab, int type)
{
	short	TestSym;
	short	v8;
	short	run_rel;
	
	if (symbol == 2230)			// variable/Field def
	{
	    v8 = loadexp(0, 1);
	    if ( !v8 )
			return 0;

		if ( type == row_ANSWER )
			xtab->ALine_exp = v8;
		else
			xtab->PLine_exp = v8;
	}
	else if ( symbol == 1800 || symbol == 930)	// '=' or <CR>
	{
        if ( type == row_ANSWER )
			v8 = prow_0;				// don't bump for the answer column
		else
			v8 = prow_0 + 1;			// bump prow for PROMPT and HEAD 
		
        if ( v8 <= 0 || v8 > _li )		// range check on row calculated
			v8 = 1;
		
		if ( type != row_HEAD )			// don't update line for head statements
			prow_0 = v8;
          
		if ( type == row_ANSWER )		// a2 ==1 to get Answer Row, 2 to get Prompt Row
			xtab->ALine = v8;
		else
			xtab->PLine = v8;

		if ( symbol != 930 )			// if symbol was a <CR> don't advance
			symbol = getsym();
	}
    
	else if ( symbol == 1820 || symbol == 1840 || symbol == 2200 || symbol == 50 || symbol == 1580)
	{		// 1820 = ">", 1840 = "<" (runtime relative) or 2200 = Integer literal, 50 = '+', 1580 = '-' 
		TestSym = 0;
		if ( symbol != 2200 )
		{
			TestSym = symbol;
			symbol = getsym();
			if ( symbol != 2200 )
			{
				loaderr(35, sym);	// "invalid row specification"
				return 0;
			}
		}
		   
		v8 = strtol(sym, 0, 10);
		if ( v8 < 0 )
			TestSym = 1580;	// '-'

		if (type == row_HEAD)	// Ignore runtime relative specifiers in HEAD/TEXT
		{
			if ( TestSym == 50 || TestSym == 1580 )	// if '-' or '+'
				v8 += prow_0;

			if ( v8 <= 0 || v8 > _li)		// range check on row value parsed
				v8 = 1;
		}
		else
		{
			switch (TestSym)
			{
			case 1840:
			case 1820:
				if (TestSym == 1840)	// '<'	runtime relative to last address
					run_rel = 0x0400;
				else
					run_rel = 0x0200;		// TestSym = '>' (token 1820)
				if ( v8 < 0 )				// shouldn't be reachable, due to negattive test above : TestSym = 1580;
				{
					if ( run_rel == 0x0400 )	// switch flag bits for negative values
						run_rel = 0x0200;
					else
						run_rel = 0x0400;
					v8 = -v8;
				}
				v8 |= run_rel;
				break;

			case 50 :
			case 1580:
				v8 += prow_0;				// fall through to default case, range check
			default :
				if ( v8 <= 0 || v8 > _li)
					v8 = 1;
				break;

			}
			if ( type == row_PROMPT )		// only update static variable for PROMPT ROW
				prow_0 = v8;
		}
		if ( type == row_ANSWER )
			xtab->ALine = v8;
		else
			xtab->PLine = v8;
		
		symbol = getsym();
		return xtab;
	}
	else
	{
		loaderr(35, sym);	// "invalid row specification"
        return 0;
	}
    return xtab;
}

#endif
