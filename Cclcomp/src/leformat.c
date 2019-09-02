#ifndef LEFORMAT_C
#define LEFORMAT_C

#include <assert.h>

bool leformat(int *a3, bool no_brk)
{
	BTAB	*btb;
	PTAB	*ptab;
	
	short	blkno;
	short	PTno;
	short	ENTno;
	
	size_t	MaxLen;
	char	BlockName[256];		// overkill. Only needs 21
	
	//eprint("leformat *a3 = %d , no_brk = %s\n", *a3,no_brk ? "true":"false");
	symbol = getsym();
    if ( symbol != 2230 )                   // 2230 must be a string literal?
    {
		if ( symbol == 930 )
			loaderr(2, sym);				// "missing block name"
		else
			loaderr(33, sym);				// "unexpected symbol"
		return false;
    }
    
	MaxLen	= fixbname(BlockName, sym);
    btb		= btab;
    blkno	= 0;
    
	if ( !btb->BlockName[0] )
		goto LABEL_11;
    while ( !cmpbuf(btb->BlockName, BlockName, MaxLen) )
    {
        blkno++;
        btb++;
        if ( !btb->BlockName[0] )
			goto LABEL_11;
    }
    if ( !btb->BlockName[0] )		// BlockName not found, forward reference
	{
LABEL_11:
		assert(blkno >= no_btabs);	// check we are beyond num_blocks limit
		newbtab();					// create new block
		assert(blkno < no_btabs);	// check block now exists
		cdbcpystr(btab[blkno].BlockName, BlockName, 0);
	}
//----------------------
    if ( !no_brk )
    {
		PTno = (*a3)++;
        ptab = PTARR(getptabp(PTno));
        ptab->OpCode = 950;			// break OpCode
    }
    
	PTno = (*a3)++;
    allptpt(getptabp(PTno), &ptab);
    ptab->OpCode	= 600;			// format OpCode
    ptab->Operand	= blkno;
    ptab->SrcLineNo = lla;

	// look for an optional Variable specifying MODE of the block to pass in. Defaults to 'e'
    symbol = getsym();
    if ( symbol == 2230 )			// table/variable name
    {
		ENTno = getenmem();
        if ( !gettfexp(ENTno) )
        {
			loaderr(8, sym);		// "invalid field name or number"
            dallptpt(&ptab);
            return false;
		}
        ptab->TABno = ENTno;
        symbol = getsym();
	}
    dallptpt(&ptab);
    return true;					// exit_success
}

#endif
