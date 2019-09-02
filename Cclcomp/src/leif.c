#ifndef LEIF_C
#define LEIF_C

#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

bool leif(int *a1)
{
	PTAB	*ptb;
	short	PTno;
	short	expno;
	
	if ( max_it <= it )			// "it" is some nice global goodness
    {
		newitab();
		assert (it < max_it);
	}
    PTno = getptabp((*a1)++);
    it->itType	= 1;					// 1 = 'if', 2 = 'while'
    it->PTfalse	= PTno;					// Jump here when "IF" expression evaluates to false
    it++;
    if ( max_it <= it )
    {
		newitab();
		assert (it < max_it);
	}
    
	do
		symbol = getsym();				// skip past any blank lines
	while ( symbol == 930 );			// <CR>

	allptpt(PTno, &ptb);
    ptb->OpCode		= 300;				// 300 == "if"
    ptb->SrcLineNo	= lla;
	expno			= loadexp(1, 1);	// in conditional, expect int answer
    ptb->TABno		= expno;			// ptb can move! keep expno step!
    dallptpt(&ptb);
    
	if ( !expno || symbol != 930 )
    {
		while ( symbol )
        {
			if ( expno )				// loaded expression ok, but <CR> didn't follow!
				loaderr(33, sym);		// unexpected symbol
			symbol = getsym();
            if ( comp_abort )
				break;
			if ( symbol == 930 )
				goto LABEL_15;
		}
        return false;	// error_exit
	}

LABEL_15:
    while ( symbol == 930 )
		symbol = getsym();
    
	// perfectly ok to have a blank block in between if/elsif, elsif/else, else/endif pairs
	if ( symbol == 370 )                       // elsif
	{
		while ( 1 )
        {
DO_ELSIF:
            --it;
            if ( it->HasElse )					// We've already encountered an 'else' case. ERROR 
            {
				loaderr(9, sym);				// "unexpected elsif"
	            return false;
			}
            it->HasElsif = 1;
            
			PTno = getptabp((*a1)++);
			PTARR(PTno)->OpCode = 700;	// goto
			it->PTABno = PTno;
            
			if ( it->PTfalse )
            {
				(&ptarr.TableAddr[it->PTfalse - 1])->Operand = *(short *)a1;
                it->PTfalse = 0;
			}
            ++it;
			//printf("do_elsif_90: it = x%08X\n", it);
			if ( max_it <= it )
            {
				newitab();
				assert (it < max_it);
			}
            PTno = getptabp((*a1)++);
            it->itType	= 1;				// 1 = 'if', 2 = 'while'
            it->PTfalse = PTno;
            ++it;
			//printf("do_elsif_104: it = x%08X\n", it);
			if ( max_it <= it )
            {
				newitab();
				assert (it < max_it);
			}
               
			do
				symbol = getsym();
			while ( symbol == 930 );		// <CR>

            allptpt(PTno, &ptb);
            ptb->OpCode		= 300;			// if OpCode
            ptb->SrcLineNo	= lla;
			expno			= loadexp(1, 1);
            ptb->TABno		= expno;
            dallptpt(&ptb);

            if ( !expno )
				break;
			if ( symbol != 930 )
				break;
LABEL_48:
			while ( symbol == 930 )
				symbol = getsym();

			if ( symbol != 370 )             // elsif
            {
				if ( symbol != 360 && symbol != 390 && symbol )// 360 = else, 390 = endif
                {
					do
                    {
						loadstate(a1);
                        if ( comp_abort )
							return false;
							  
						while ( symbol == 930 )
							symbol = getsym();
					}		// 370 = elsif, 360 = else,  390 = "endif"
					while ( symbol != 370 && symbol != 360 && symbol != 390 && symbol );
				}
                if ( symbol != 370 )        // 370 = elsif
					goto CHECK_ELSE;
			}
		}
        while ( symbol )
        {
			if ( ptb->TABno )
				loaderr(33, sym);			// unexpected symbol
			symbol = getsym();
            if ( comp_abort )
				break;
			if ( symbol == 930 )
				goto LABEL_48;
		}
        return false;	// error_exit
	}
    if ( symbol != 360 && symbol != 390 && symbol )// 360 = "else", 390 = "endif"
    {
		do
        {
			loadstate(a1);			// *** many levels of recursion possible here !! ***
            if ( comp_abort )
				return false;
			while ( symbol == 930 )
				symbol = getsym();
		}	// 370 = elsif, 360 = else,  390 = "endif"
        while ( symbol != 370 && symbol != 360 && symbol != 390 && symbol );
	}

    if ( symbol == 370 )			// 370 = "elsif"
		goto DO_ELSIF;

CHECK_ELSE:
	if ( symbol == 360 )			// 360 = "else"
    {
		--it;
        it->HasElse = 1;			// This if/end block has an 'ELSE' case!
        
		PTno = getptabp((*a1)++);
		PTARR(PTno)->OpCode = 700;	// goto OpCode.
		it->PTABno = PTno;			// This will be the next instruction after the if/endif block.
        if ( it->PTfalse )
        {
			//(&ptarr.TableAddr[it->PTfalse - 1])->Operand = *(short *)a1;
			PTARR(it->PTfalse)->Operand = *(short *)a1;
            it->PTfalse = 0;
		}
        it++;

		if ( max_it <= it )
        {
			newitab();
			assert (it < max_it);
		}
          
		do
			symbol = getsym();
		while ( symbol == 930 );

        while ( symbol != 390 && symbol )		// 390 = "endif"
        {
			loadstate(a1);				// many levels of recursion possible here
            if ( comp_abort )
				return false;
			while ( symbol == 930 )		// skip blank lines
				symbol = getsym();
		}
	}
// CHECK_ENDIF
    if ( symbol == 390 )				// 390 = "endif"
    {
		symbol = getsym();
        --it;							// back up the ITAB array, and set all linkages
        if ( it->itType != 1 )			// 1 = 'if', 2 = 'while' ?
        {
			loaderr(5, sym);			// "endif without if"
            return false;
		}

		if ( itab < it && it[-1].HasElsif )	// This was an "if/elsif/endif" *OR* "if/elsif/else/endif"
        {
			if ( it->HasElse )			// This block has an 'ELSE' case!
            {
				it->HasElse = 0;
                /*if ( it->PTABno )	// appears duplicated below...
                {
					(&ptarr.TableAddr[it->PTABno - 1])->Operand = *(short *)a1;
                    it->PTABno = 0;
				}*/
			}
            if ( it->PTfalse )
            {
				(&ptarr.TableAddr[it->PTfalse - 1])->Operand = *(short *)a1;
                it->PTfalse = 0;
			}
            if ( it->PTABno ) // duplicated here ???
            {
				(&ptarr.TableAddr[it->PTABno - 1])->Operand = *(short *)a1;
                it->PTABno = 0;
			}
            for ( --it; it->HasElsif; --it )	// Possibly many levels of elsif's!
            {
                it->HasElse		= 0;
                it->HasElsif	= 0;
                if ( it->PTABno )
                {
					(&ptarr.TableAddr[it->PTABno - 1])->Operand = *(short *)a1;
                    it->PTABno = 0;
				}
                if ( itab >= it )		// reached outermost level
					break;
				if ( !it[-1].HasElsif)	// Don't really need this check. For loop should suffice?
					break;
			}
		}
        else
        {
			if ( it->HasElse )		// This was an if/else/endif block
            {
				it->HasElse = 0;
                if ( it->PTABno )
                {
					(&ptarr.TableAddr[it->PTABno - 1])->Operand = *(short *)a1;
                    it->PTABno = 0;
				}
			}
            else					// This was a simple if/endif block
            {
				if ( it->PTfalse )
                {
					(&ptarr.TableAddr[it->PTfalse - 1])->Operand = *(short *)a1;
                    it->PTfalse = 0;
				}
			}
		}
	}
    return true;	// exit_success
}

#endif
