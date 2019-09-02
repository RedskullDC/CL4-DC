#ifndef GETNODENAME_C
#define GETNODENAME_C

#include <sys/types.h>	// for user calls
#include <pwd.h>
#include <sys/utsname.h>	// for getnodename()
#include "DBdefs.h"
#include "cl4.h"

char *getnodename(void)
{
	if ( isCGI )
		return getenv("HTTP_HOST");

	if ( !name )		// global
    {
		name = (struct utsname *)mmalloc(sizeof(struct utsname));	// 390u
        uname(name);
	}
    if ( name->nodename[0] )
		return name->nodename;
	else
		return "????";
}

#endif
