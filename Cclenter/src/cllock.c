#ifndef CL_LOCK_C
#define CL_LOCK_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <termio.h>		// for ioctl etc.
#include <unistd.h>		// for isatty(fileno(stdout))
#include "DBdefs.h"
#include "cl4.h"

bool cllock(int PID)
{
    bool	isMyPID;
	int		v4;
	
    isMyPID = PID == getpid();		// make sure we are running in original process, not a fork()
    
	if ( isMyPID )
        sprintf(lpath, "/tmp/clfl%d", PID);			// global goodness :o)
    
	while ( 1 )
    {
        //v4 = open64(lpath, 0xC1);			// 0000 0000 1100 0001 ==    O_WRONLY | ?? | ??
        v4 = open64(lpath, 0301,0666);		// 0000 0000 1100 0001 ==    O_WRONLY | ?? | ??
		if ( v4 >= 0 || *__errno_location() != 17 )
            break;
        
		if ( isMyPID )
            return true;
        
		sleep(1u);
    }
    return (unsigned int)~v4 >> 31;
}

int clunlock()
{
    if ( lpath[0] )
    {
        unlink(lpath);		// release file. Will disappear when program exits
        lpath[0] = 0;
    }
    return 1;
}

#endif

