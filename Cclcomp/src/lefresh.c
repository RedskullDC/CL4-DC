#ifndef LEFRESH_C
#define LEFRESH_C

int lefresh(PTAB *ptb)
{
    symbol = getsym();						// refresh *without* a block name is OK
    if ( symbol != 930 && symbol )
    {
		ptb->TABno = getstrmem(syml + 1);
		cdbcpystr(STRARR(ptb->TABno), sym, 0);
        symbol = getsym();
	}
    return 1;								// always exit_success
}

int lescreen(PTAB *ptb)
{
    symbol = getsym();
    if ( symbol != 930 && symbol )			// screen without a block name is an ERROR!
    {
		ptb->TABno = getstrmem(syml + 1);	// sym = screen name, anything technically valid
		cdbcpystr(STRARR(ptb->TABno), sym, 0);
        symbol = getsym();
        return 1;			// exit_success
	}
    else if ( symbol == 930 )
		loaderr(7, sym);					// "unexpected end of line"
	else
		loaderr(6, sym);					// "unexpected end of file"
	
	return 0;	// error_exit
}

#endif
