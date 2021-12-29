#ifndef MAKE_TEMP_C
#define MAKE_TEMP_C

#include <stdlib.h>		// for mkstemp
#include "DBdefs.h"
#include "cl4.h"

short _mktmp(void)
{
	short	v2;
	char	name[128];
	
	//print("_mktmp(%d)\n",TDno);
    if ( !cdbtmpdir[0] )
    {
        cdbcpystr(cdbtmpdir, getevar("CLTMPDIR"), 0);
        if ( !cdbtmpdir[0] )
            cdbcpystr(cdbtmpdir, defcdbtdir, 0);	// default == "/tmp"
    }
    cdbcpystr(name, cdbtmpdir, "/tXXXXXX", 0);
	//print("_mktmp(%d) - File = %s\n",TDno,name);
    v2 = mkstemp(name);
    unlink(name);				// comment out to leave tmp DBases in /tmp/
    return v2;
}

#endif

