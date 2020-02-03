#ifndef LEEOP_C
#define LEEOP_C

int leeop(int a1)								// on end of page 'on_eop'
{
	ONESC **v2;
	ONESC *escp;
	char BlockName[32];
	
	if ( a1 )
		PTARR(getptabp(a1 - 1));				// seems pointless, but may force new PTAB

	v2 = &oelist;
	while (*v2)
		v2 = &(*v2)->NextESC;

	escp = (ONESC *)getmem(sizeof(ONESC));		// create new escape, Add it to the end of the list
	*v2 = escp;									// Links the new escape into the chain

	escp->KeyNumber = 49;			// ????
	symbol = getsym();

	if ( symbol == 2230 )						// String literal - Blockname
	{
		escp->BlockName = getmem(fixbname(BlockName, sym));
		cdbcpystr(escp->BlockName, BlockName, 0);
		symbol = getsym();
		return 1;								// exit success
	}
	else
	{
		if ( symbol == 930 )					// <CR>
			loaderr(2, sym);					// missing block name
		else
			loaderr(33, sym);					// unexpected symbol
		return 0;								// error exit
	}
}

#endif
