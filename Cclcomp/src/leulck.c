#ifndef LEULCK_C
#define LEULCK_C

bool leulck(int *a1)
{
    PTAB	*ptab;
    int		TDno;
    int		Error;

    ptab = PTARR(getptabp((*a1)++));
    ptab->OpCode	= 400;				// 400 = default to NO-OP
    ptab->SrcLineNo = lla;
    symbol = getsym();
    if ( symbol == 70 )                 // all
    {
		ptab->Operand	= 0;			// 0 indicates unlock all tables?
		ptab->OpCode	= 910;			// Unlock OpCode
		symbol = getsym();
		return true;                    // success exit
	}
	else if ( symbol == 2230 )          // table/alias name
    {
		TDno = findtd(sym, -1);			// -1 == look in all open DBases
        if ( TDno >= 0 )
		{
			ptab->Operand	= TDno;
			ptab->OpCode	= 910;		// Unlock OpCode
			symbol = getsym();
		    return true;                // success exit
		}
        Error = 14;						// "table not open"
	}
    else if ( symbol == 930 )
		Error = 40;						// "missing table/alias"
	else	
		Error = 39;						// "invalid table/alias"
	
	loaderr(Error, sym);
    return false;                       // error exit
}

#endif
