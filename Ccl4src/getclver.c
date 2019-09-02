#ifndef CLVERS_C
#define CLVERS_C

#include <stdio.h>
#include "DBdefs.h"
#include "cl4.h"

// removed the isClDemo test.

char	clv[40];
char*	getclver(void)
{
	sprintf(clv, "[Linux2.6-DC] V%d.%d.%d Build Date: %s",4, 4, 4, __DATE__);
	return clv;
}

#endif
