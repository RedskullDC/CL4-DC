#ifndef OPENSHARE_C
#define OPENSHARE_C

#include <fcntl.h>
#include "DBdefs.h"
#include "cl4.h"

short _oshare(char *a1,char *a2)
{
	*a2 = 0;
	return open64(a1, O_RDWR);
}

#endif
