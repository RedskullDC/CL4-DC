#ifndef CLVERS_C
#define CLVERS_C

#include <stdio.h>
#include <sys/utsname.h>
#include "DBdefs.h"
#include "cl4.h"

// removed the isClDemo test.

char	clv[256];
char	kernal[80];
struct	utsname utsbuf;
char*	getclver(void)
{

	// get info about the running system
	if (!uname(&utsbuf))
		cdbcpystr(kernal," ",utsbuf.sysname," ", utsbuf.release,0);
	else
		cdbcpystr(kernal," ",0);

	//sprintf(clv, "[Linux2.6-DC] V%d.%d.%d Build Date: %s",4, 4, 4, __DATE__);
	#ifdef __x86_64__
		sprintf(clv, "[%s] V%d.%d.%d-DC 64-BIT, Build Date: %s, GCC Version %s", kernal, 4, 4, 4, __DATE__,__VERSION__);
	#else
		sprintf(clv, "[%s] V%d.%d.%d-DC 32-BIT, Build Date: %s, GCC Version %s", kernal, 4, 4, 4, __DATE__,__VERSION__);
	#endif
	return clv;
}

#endif
