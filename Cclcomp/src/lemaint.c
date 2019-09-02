#ifndef LEMAINT_C
#define LEMAINT_C


bool lemaint(BTAB *btb)
{
	TDesc	*TTptr;
	char	*v7;
	char	*v8;
	ONESC	*escp;
	FLDdesc *fld;
	
	int		TDno;
	short	cmd;
	char	blockname[32];						// only need 21 bytes here
	
	symbol = getsym();
    if ( symbol != 2230 )						// expecting a Table/Alias name. error if not
    {
		if ( symbol == 930 )
			loaderr(40, sym);					// missing table/alias
		else
			loaderr(39, sym);					// invalid table/alias
		return false;
	}

	TDno = findtd(sym, -1);
    if ( TDno < 0 )
    {
		loaderr(14, sym);						// "table not open"
		return false;
    }
	
	ttab[TDno].TDlocked |= (ttl_LOCK | ttl_PUT| ttl_GET);	// 0x0007u;
    btb->TTno = TDno;	   	
						   
	symbol = getsym();
    if ( (symbol == 180 || symbol == 330) && (symbol = getsym(), symbol == 2230) )	// 180 = ",", 330="/", 2230 = string literal
	{
		TTptr = &ttab[TDno];
		fld = getvarname(sym);					// maintain code can be in a pre-defined variable????  *undocumented*
		if ( fld )
		{
			v7 = (char *)fld->FLDdata;
			v8 = (char *)fld->FLDdata;
		}
		else
		{
			v8 = sym;
			v7 = sym;
		}
		while (*v7)
		{
			switch (*v7)
			{
				case 'a':
				case 'c':
					TTptr->TDlocked |= (ttl_PUT| ttl_GET);	// 0x0006;
					break;
				case 'd':
					TTptr->TDlocked |= (ttl_DEL | ttl_GET);	// 0x8004;
					break;
				case 'v':
					TTptr->TDlocked |= ttl_GET;				// 0x0004;
					break;
				default:
					loaderr(42, sym);			// error!!!  invalid maintain mode
					return false;
					break;
			}
			v7++;
		}
		cdbcpystr(btb->Maintain, v8, 0);		// copy maintain codes to BTAB
		symbol = getsym();
	}
    if ( !btb->Maintain[0] )
		cdbcpystr(btb->Maintain, "vac", 0);		// default = view, add, change if no codes selected
//--------------------------------------------     
	while ( symbol == 970 || symbol == 1000 )	// 970="on_delete", 1000="on_exception"
    {
		cmd = symbol;					// save for check below
		symbol = getsym();
        if ( symbol != 2230 )           // string literal {block name}
		{
			if ( symbol == 930 )		// newline
				loaderr( 2, sym);		// missing block name
			else
				loaderr(33, sym);		// unexpected symbol
			return false;
		}
		escp = (ONESC *)getmem(sizeof(ONESC));	// 16
        escp->BlockName = getmem(fixbname(blockname, sym));
        cdbcpystr(escp->BlockName, blockname, 0);
        if ( cmd == 970 )               // "on_delete"
			btb->On_delete = escp;
		else
			btb->On_excep = escp;        // "on_exception
        symbol = getsym();
	}
    return true;							// exit success
}

#endif
