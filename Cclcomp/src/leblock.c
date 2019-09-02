#ifndef LEBLOCK_C
#define LEBLOCK_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <assert.h>
#include "DBdefs.h"

void leblock(int *LineNo)
{
    BTAB	*bt;
	PTAB	*ptab;
	ONESC	*oep;
	
	size_t	BlkNameLen;
	int		ErrorCode;
	short	blkno;

	char BlockName[32];
	
    symbol = getsym();							// prior to this we are looking at a '{' block start character
    if ( symbol == 2230 )						// Table/Variable name	== blockname
	{
		BlkNameLen = fixbname(BlockName, sym);	// Fix maxLen as 21 (20 chars + \0)
		for (bt = btab, blkno = 0; bt->BlockName[0] && no_btabs > blkno ; bt++, blkno++)
		{
			if ( cmpbuf(bt->BlockName, BlockName, BlkNameLen) )	// look for an already defined block with same name
				break;
		}
		cur_block = blkno;
		if ( blkno == no_btabs )	// true if this is a block we don't know about, so create a new one!
		{
			newbtab();				// should bump no_btabs by 1
			bt = &btab[blkno];
			assert(blkno < no_btabs);
		}
		if ( !bt->BlockName[0] )
			cdbcpystr(bt->BlockName, BlockName, 0);
		if ( bt->StartLine || bt->EndLine )
			loaderr(81, sym);					// "duplicate block name"
		else
			bt->StartLine = *(short *)LineNo;
		symbol = getsym();

		if ( symbol == 1010 )                   // look for on_exit handler
		{
			symbol = getsym();
			if ( symbol == 2230 )               // string literal == {on_exit : block_name}
			{
				oep = (ONESC *)getmem(sizeof(ONESC));		// 16
			    oep->BlockName = getmem(fixbname(BlockName, sym));
			    cdbcpystr(oep->BlockName, BlockName, 0);
			    bt->On_exit = oep;
			    symbol = getsym();
			}
			else
			{
				if ( symbol == 930 )			// newline = ERROR. was expecting a blockname to follow on_exit:
					loaderr(2, sym);            // "missing block name"
				else
					loaderr(33, sym);           // "unexpected symbol"
				return;
			}
		}
	}
	else	// symbol != 2230 )		// table/var name literal
	{
		loaderr(2, sym);			// "missing block name"
	}

//-------------------

	while ( 1 )
    {
		while ( symbol == 930 )				// "\n"  end of line
			symbol = getsym();

		ErrorCode = 0;
		switch (symbol)
		{
		case 570:									// getfile
			if ( !legetf())
				ErrorCode = 1;
			break;

		case 750:									// ldefine = 750, define == 290
		case 290:
			if ( !ledefine(symbol == 750) )			// symbol == 750 if local define
				ErrorCode = 1;
			break;

		case 830:									// maintain
			if ( !lemaint(&btab[cur_block]))
				ErrorCode = 1;
			break;

		case 630:									// 630 = "head"
			symbol			= getsym();
		    allptpt(getptabp((*LineNo)++), &ptab);
		    ptab->OpCode	= 3;					// 3 = head OpCode
			ptab->SrcLineNo = lla;
			ErrorCode		= !(ptab->TABno = lehdtxt());
			
			dallptpt(&ptab);
			break;
			
		case 510:									// format
			if ( !leformat(LineNo, false))			// NoBreak = false
				ErrorCode = 1;
			break;

		case 120:									// "break"    -   force a screen break. 
			allptpt(getptabp((*LineNo)++), &ptab);
	        ptab->OpCode	= 950;					// 950 = break OpCode
	        ptab->SrcLineNo = lla;
	        dallptpt(&ptab);
	        symbol			= getsym();
	        if ( symbol != 930 )
			{
				loaderr(33, sym);		// "unexpected symbol" - should be no arguments after break on the line
				ErrorCode = 1;
			}
			break;

		case 1360:									// "}"  end of block terminator
			btab[cur_block++].EndLine = *(short*)LineNo;

			allptpt(getptabp((*LineNo)++), &ptab);
            ptab->OpCode	= 950;		// "break"
            dallptpt(&ptab);

			allptpt(getptabp((*LineNo)++), &ptab);
            ptab->OpCode	= 999;		// "end"
            ptab->SrcLineNo = lla;
            dallptpt(&ptab);
            return;						// exit_success  **************
			break;

		case 0:							// no symbol!!  Error condition
			loaderr(6, sym);			// "unexpected end of file"
			return;
			break;

		default:						// normally call this for all other lines in a block
            if ( !loadstate(LineNo) )
				ErrorCode = 1;
			break;
		}
		if ( comp_abort )
			return;			// error_exit
		
		if ( ErrorCode )	// We found an error condition. Keep reading till we hit NewLine or EOF
        {
			while ( symbol && symbol != 930 )
				symbol = getsym();
		}
	}
}

#endif
