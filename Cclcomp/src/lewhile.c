#ifndef LEWHILE_C
#define LEWHILE_C

#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

bool lewhile(int *a1)
{
    PTAB *ptab;
	PTAB *v12;
	PTAB *v13;
	
	short wend_start;
	short wend_after;

    short PTno;
	short expno;
	
	if ( max_it <= it )				// "it" is some nice global goodness
    {
		newitab();
		assert (it < max_it);
	}
    it->itType = 2;					// 1 = 'if', 2 = 'while'

    PTno = getptabp(*(short *)a1);
	ptab = PTARR(PTno);
	ptab->Operand = *(short *)a1;
    it->PTfalse = PTno;				// Jump here when "WHILE" expression evaluates to *false*
									// *wrong value*, save PTno for later. link will be fixed below.
    it++;

    if ( max_it <= it )				// Allocate another ITAB where necessary
    {
		newitab();
		assert (it < max_it);
	}

    do						
		symbol = getsym();			// skip past any blank lines
	while ( symbol == 930 );		// <CR>

	ptab->OpCode	= 310;			// 310 = "while"
    ptab->SrcLineNo	= lla;

    expno = loadexp(1, 1);			// load conditional expression
    ptab->TABno		= expno;
    if ( !expno || symbol != 930 )
    {
		//printf("lewhile 51 - expno = %d, symbol = %d, sym = %s\n",expno,symbol,sym);
		while ( symbol )
        {
			if ( expno )				// We loaded expression ok, but was something else before the <CR>
				loaderr(33, sym);		// unexpected symbol error
			
			symbol = getsym();
            if ( comp_abort )
				return false;				// error_exit
			if ( symbol == 930 )
				goto LABEL_18;
		}
	}

LABEL_18:
	while (symbol == 930)
		symbol = getsym();
    
	++*a1;									// Bump currPTAB ptr
		
	while ( symbol != 1710 )				// "wend"
    {
		loadstate(a1);						// **many** levels of recursion possible here
        if ( comp_abort )
			return false;						// error_exit
		while (symbol == 930)				// <CR> skip blank lines
			symbol = getsym();
	}

DO_WEND:									// wend symbol found, back up ITAB, and fix links
    symbol = getsym();
    --it;
    if ( it->itType != 2 )					// 1 = 'if', 2 = 'while' ?
    {
		loaderr(23, sym);					// "wend without while"
        return false;
	}
	
	v12 = PTARR(getptabp((*a1)++));			// Insert a goto instruction, back to the initial "while" instruction
    v12->OpCode = 700;						// 700 = "goto"  Branch always
    
	v13 = PTARR(it->PTfalse);
	wend_start = v13->Operand;				// Recover first instruction from ITAB record.
    v12->Operand = wend_start;				// branch back to wend instruction

	wend_after = *(short *)a1;
    v13->Operand = wend_after;				// fix false link in original wend instruction!
    
	// Look for any continue/end_loop instructions inside while/wend block, and fix links
	for (PTno = wend_start + 1; PTno <= wend_after; PTno++)
	{
		ptab = PTARR(PTno);
		switch (ptab->OpCode)
		{
		case 320:								// 320 = continue
			if ( !ptab->Operand )				// don't clobber any existing pointers!!
				ptab->Operand = wend_start;		// first instruction of the loop
			break;
		case 330:								// 330 = end_loop
			if ( !ptab->Operand )
				ptab->Operand = wend_after;		// instruction *after* the loop
			break;		
		}
	}
    return true;	// exit_success
}


#endif
