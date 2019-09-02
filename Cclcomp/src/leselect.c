#ifndef LESELECT_C
#define LESELECT_C

short leselect(void)
{
	RTAB	*rt;
    int		TDno;
	short	RTno;
    short	EXPno;

	symbol = getsym();
    if ( symbol == 2230 )					// variable/table name
    {
		RTno	 = getrtmem();				// don't simplify!! rt can move
		rt		 = RTARR(RTno);

		TDno	 = findtd(sym, -1);
		rt->TTno = TDno;
		if ( TDno >= 0 )
		{
			rt->OpType = 0x0080u;			// file op
			symbol = getsym();
			rt->WhereEXP = 0;				// select TABLEname [all]/[where  where_expression]
			
			if ( symbol == 70 )				// 70 = all (also the default case when all not specified)
				symbol = getsym();			// fall through to looking for a <CR> below

			else if ( symbol == 1720 )		// 1720 = where
			{
				symbol = getsym();
				EXPno = loadexp(1, 1);
				if ( EXPno )
					rt->WhereEXP = EXPno;	// loaded where expression, fall through to look for <CR>
				else
					return 0;				// error_exit, couldn't load 'where' expression
			}

			if ( symbol == 930 )			// nothing following table name is also ok. : "select Tranfl1 \n"
				return RTno;				// exit_success
			loaderr(33, sym);				// "unexpected symbol"
		}
		else
			loaderr(14, sym);				// "table not open"
	}
	else
	{
		if ( symbol == 930 )
			loaderr(40, sym);				// "missing table/alias"
		else
			loaderr(39, sym);				// "invalid table/alias"
	}
    return 0;	// error_exit
}

#endif
