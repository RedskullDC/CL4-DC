#ifndef LECOPY_C
#define LECOPY_C

short lecopy(void)
{
    XFTAB	*xft;
	int		TTno;
	short	XFno;
	
	symbol = getsym();
	if ( symbol == 2230 )					// Destination table alias
	{
		XFno = getxfmem();
		allxfpt(XFno, &xft);
		TTno = findtd(sym, -1);				// look for the destination table/alias "-1" means search all DB's 
		xft->TTno_to = TTno;
		if ( TTno >= 0 )
		{
			symbol = getsym();
			if ( symbol == 550 )			// from
			{
				symbol = getsym();
				if ( symbol == 2230 )		// expecting a string literal to follow (source table/alias)
				{
					TTno = findtd(sym, -1);	// all ok, look for source table
					xft->TTno_from = TTno;
					if ( TTno >= 0 )
					{
						dallxfpt(&xft);
						symbol = getsym();
						return XFno;		// *** exit_success ***
					}
					else
						loaderr(14, sym);	// "table not open"
				}
				else if ( symbol == 930 )
					loaderr(40, sym);		// "missing table/alias"
				else
					loaderr(39, sym);		// "invalid table/alias"
			}
			else
				loaderr(38, sym);			// "expected 'from'"
		}
		else
			loaderr(14, sym);				// "table not open"

		dallxfpt(&xft);
	}
	else
    {
		if ( symbol == 930 )
			loaderr(40, sym);		// "missing table/alias"
		else
			loaderr(39, sym);		// "invalid table/alias"
    }

	return 0;		// error exit
}


#endif
