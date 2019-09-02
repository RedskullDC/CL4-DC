#ifndef GETDIRNAME_C
#define GETDIRNAME_C

#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <pwd.h>
#include "DBdefs.h"
#include "cl4.h"

char	*myuser_0;		// not public in libcl4

char	*getdirname(char *path)				// path = path/filename
{
	struct passwd *pswd;
	char *v1;
	char *v2;
	char *ptr;
	char *v8;
	
	char sep;

	//eprint("getdirname - path = %s\n",path);
	if ( *path == '~' )						// relative to user's home dir eg: ~/Cclcomp/TESTDB
	{
		v1 = path + 1;
		v2 = v1;
		while ( *v1 && *v1 != '/' )
			++v1;

		sep = *v1;							// sep = '/' or \0 at this point
		*v1 = 0;
		if ( !*v2 )							// if non-zero, another user is specified: ~leslie/Cclcomp/TESTDB
		{
			if ( !myuser_0 )
				myuser_0 = gelogin();
			v2 = myuser_0;
		}
		pswd = getpwnam(v2);				// possible that v2 will point to an invalid username
		if ( pswd )
		{
			*v1 = sep;
			return mstrcpy(pswd->pw_dir, v1, 0);
		}
		else
			return mstrcpy("", 0);			// invalid user name specified, return blank
	}
	else if ( *path == '$' )				// start of an environment variable eg. $HOME
	{
		ptr = mstrcpy(path, 0);
		while ( *ptr == '$' )				// multiple depth in system variables allowed.
		{
			v1 = ptr + 1;
			while (*v1 && *v1 != '/')
				v1++;
			
			sep = *v1;
			*v1 = 0;
			v2 = getevar(ptr + 1);			// extract system variable
			*v1 = sep;
			if ( !v2 )						// if *any* of the system variables can't be found, return path unchanged
				return path;
			
			v8 = mstrcpy(v2, v1, 0);		// inject system var contents into string, and go around again
			mfree_0(ptr);
			ptr = v8;
		}
		return ptr;							// exit success
	}
	else
		return path;						// just return to caller
}

#endif
