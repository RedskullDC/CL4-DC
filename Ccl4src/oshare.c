#ifndef OPENSHARE_C
#define OPENSHARE_C

#include <fcntl.h>
#include "DBdefs.h"
#include "cl4.h"

short _oshare(char *DBname, char *DBopenflg)
{
	*DBopenflg = 0;
	return open64(DBname, O_RDWR);
}

#endif
