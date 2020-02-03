#ifndef LEESCAPE_C
#define LEESCAPE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

int leescape(int PTno, int symbol2)
{
	PTAB	*ptb;
	XTAB	*xtab;
	ONESC	**v6;
	ONESC	*v8;
	
    short	KeyNumber;
	bool	AcceptField;
	char	v16[32];
	
	//eprint("leescape(PTno = %d, symbol2 = x%04X)\n", PTno,symbol2);
	AcceptField = 0;
	if ( PTno )
    {
		ptb = PTARR(getptabp(PTno - 1));	// look at previous program instruction
        AcceptField = ptb->OpCode == 1;		// Is this an accept field?
    }

	if ( AcceptField )
    {
		xtab = XTARR(ptb->TABno);
		v6 = &xtab->onesc;           // Look at individual field escapes
    }
    else
    {
		v6 = &oelist;                // Not an accept field, look at global block escapes
    }

	while (*v6)
		v6 = &(*v6)->NextESC;

    v8 = (ONESC *)getmem(sizeof(ONESC));		// sizeof(ONESC)
    *v6 = v8;
    
	v8->OESCFlags = symbol2 == 1020;		// 1020 == on_key command

// work out the key number equivalent, or single char value

	symbol = getsym();
    if ( syml == 1 && isalpha(sym[0]) )		// single char keyname specifier (isalpha())
    {
		v8->KeyNumber = sym[0];
    }
	else									// Text key name. See "KEYS.c"
    {
		KeyNumber = KeyToNumber(sym);
		if ( !KeyNumber )
		{
            loaderr(73, sym);				// "invalid 'escape' character"
            return 0;
		}
		v8->KeyNumber = KeyNumber;
	}

// Now load block name
	symbol = getsym(); 
    if ( symbol != 2230 )					// not a string literal
    {
        if ( symbol == 930 )
			loaderr(2, sym);				// "missing block name"
		else
			loaderr(33, sym);				// "unexpected symbol"
        return 0;
	}
    v8->BlockName = getmem(fixbname(v16, sym));	// truncate the blockname to 20chars + 1 for \0
    cdbcpystr(v8->BlockName, v16, 0);

	symbol = getsym();
    return 1;			// exit_success
}

#endif
