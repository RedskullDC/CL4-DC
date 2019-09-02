#ifndef MAKE_TEMP_C
#define MAKE_TEMP_C

#include <stdlib.h>		// for mkstemp
#include "DBdefs.h"
#include "cl4.h"

int _mktmp(int TDno)
{
	short	v2;
	char	Dest[128];
	
	//print("_mktmp(%d)\n",TDno);
    if ( !cdbtmpdir[0] )
    {
        cdbcpystr(cdbtmpdir, getevar("CLTMPDIR"), 0);
        if ( !cdbtmpdir[0] )
            cdbcpystr(cdbtmpdir, defcdbtdir, 0);	// default == "/tmp"
    }
    cdbcpystr(Dest, cdbtmpdir, "/tXXXXXX", 0);
	//print("_mktmp(%d) - File = %s\n",TDno,Dest);
    v2 = mkstemp(Dest);
    unlink(Dest);				// comment out to leave tmp DBases in /tmp/
    return v2;
}

#endif

