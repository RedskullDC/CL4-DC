#ifndef LEREDISP_C
#define LEREDISP_C

int leredisp()
{
	RDTAB	*v6;
	RDTAB	*rd;
	int		TDno;
	short	redno;
	short	expno;
	register short RDno;

	redno = getrdmem();
    allrdpt(redno, &rd);
	while ( symbol != 930 )				// no arguments is OK??
    {
		if ( !symbol )
        {
			dallrdpt(&rd);
            return 0;
		}
        if ( symbol != 2230 )           // variable/table name
		{
			loaderr(33, sym);			// unexpected symbol
		    return 0;
		}
		if ( nxtch() == '.' || (TDno = findtd(sym, -1), TDno <= 0) )
        {
			expno = loadexp(0, 1);		// This was a single field in a tables, or variable 
            if ( !expno )
			{
				dallrdpt(&rd);
                return 0;
			}
            rd->ENTno = expno;			// single variable/field re-display
		}
        else
        {
			rd->TTno = TDno;			// This was a table name. Re-display all currently visible fields
            symbol = getsym();
		}
		//------
        if ( symbol != 930 )            // Can have multiple redisplayed records/fields all chained
        {	
			RDno =  getrdmem();			//***RDno can alter rd pointer if realloc occurs!
            rd->NextRD = RDno;
			v6 = RDARR(rd->NextRD);
            rd = v6;
		}
	}
	//( symbol == 930 )
	dallrdpt(&rd);
	return redno;	// exit_success			// result == first variable in chain
}

#endif
