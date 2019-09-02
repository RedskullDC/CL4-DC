#ifndef ERROR_OUT_C
#define ERROR_OUT_C

#include <stdlib.h>		// for mkstemp()
#include <unistd.h>		// for unlink()
#include <stdarg.h>		// for var args stuff
#include <string.h>
#include <errno.h>
#include "DBdefs.h"
#include "cl4.h"

void tstamp(void)
{
	time_t timer;

	time(&timer);
	eprint("%.24s: ", ctime(&timer));
}

char* gsyserr(void)
{
	int *v0; // eax@1

	v0 = __errno_location();
	return strerror(*v0);
}


void syserror(const char *format, ...)
{
	char*	v1;
	va_list va;

	va_start(va, format);
	eprint("%s: ", _pname);
	if ( time_stamp )
		tstamp();
	vfprintf(stderr, format, va);
	v1 = gsyserr();
	eprint(": %s\n", v1);
	va_end(va);
}

void errexit(char *a1, int a2)
{
	putstr(2, _pname, ": ", a1, a2, 0);	// careful to distinguish _pname and pname!!!!
	putstr(2, "\n", 0);
	exit(0);
}

/*

error() and xwarn() are not called in libcl4

*/
#endif
