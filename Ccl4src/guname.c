#ifndef GUNAME_C
#define GUNAME_C

#include <sys/types.h>	// for user calls
#include <pwd.h>
#include "DBdefs.h"
#include "cl4.h"


// ****  Different functionality to libcl4 ***

char* gelogin()
{
	struct passwd *pwd;	// not typedef'ed in pwd.h
	char *UserName;

	pwd = getpwuid(geteuid());
	if ( pwd )
		UserName = ftrim(mstrcpy(pwd->pw_name, 0));
	else
		UserName = "";

	return UserName;
}

// same functionality as gelogin(), but uses caller buffer area, doesn't exist in libcl4 
void glogin(char *Buffer)
{
    struct passwd *pwd;
	
    pwd = getpwuid(getuid());
    if ( pwd )
	{
        cdbcpystr(Buffer, pwd->pw_name, 0);
		ftrim(Buffer);
	}
    else
        *Buffer = 0;
}

// doesn't exist in libcl4 
void guname(char *Dest)
{
    struct passwd *pwd;

    pwd = getpwuid(getuid());
    if ( pwd )
        cdbcpystr(Dest, pwd->pw_gecos, 0);
    else
        *Dest = 0;
    ftrim(Dest);
}

char *guserdir(char *name)
{
	struct passwd *pwd;

	pwd = getpwnam(name);
	if ( pwd )
		return pwd->pw_dir;
	else
		return "";
}

// guserdir() not called in clenter

#endif
