#ifndef PPAUSE_C
#define PPAUSE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

void ppause(ENTAB *entab)
{
    char	*Prompt;
	size_t	v4;
	char	Buffer[2];
	char	s[1008];		// serious overkill on the parameter buffers
	char	Dest[1008];
	
// ppause assumes entab enright/enleft fields are valid!!

    if ( isatty(0) )	// Don't pause if we are not talking to a terminal!!
    {
        s[0] = 0;
        Prompt = (char *)ttab->TTfields[v_PAUSE_PROMPT].FLDdata;	// 58 = "PAUSE_PROMPT"
        
		if ( !Prompt || !*Prompt )
            Prompt = _YP;			// Prompt not set, use default: "<CR> to continue"	see CLINFO file
        
		if ( !entab )
            cdbcpystr(s, Prompt, 0);
		else if ( entab->enleft )	// Does entab point to multiple params?
        {
            exptobuf(s, &enarr.TableAddr[entab->enleft - 1], 0);
            if ( entab->enright )
            {
                exptobuf(Dest, &enarr.TableAddr[entab->enright - 1], 0);
			    cdbcpystr(s, s, " ", Dest, 0);
            }
        }
        else		// only a single parameter
        {
            exptobuf(s, entab, 0);
			cdbcpystr(Dest, Prompt, 0);
			cdbcpystr(s, s, " ", Dest, 0);
        }

		prerr(s, 0, 0);					// print message on MLINE
        while ( linput(Buffer, 0, 0) )	// wait for a single keypress
			;
		v4 = (short)lenstr(s);
        memset(s, ' ', v4);
        s[v4] = 0;
        prerr(s, 0, 0);					// clear the message line
        return;
    }
}


#endif

