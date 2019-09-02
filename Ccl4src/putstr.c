#ifndef PUTSTRING_C
#define PUTSTRING_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

void putstr(int fd, char *a2, ...)
{
	char *v3;
	va_list va;

	va_start(va, a2);
	v3 = a2;
	while ( v3 )
	{
		write(fd, v3,lenstr(v3));
		v3 = va_arg(va,char*);
	}
	va_end(va);
}

#endif
