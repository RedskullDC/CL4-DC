#ifndef GETRESERVE_C
#define GETRESERVE_C

#include <stdio.h>
#include <stdarg.h>			// for var args stuff
#include <string.h>			// strncmp()
#include "DBdefs.h"
#include "cl4.h"

char *getresv(int VarNum)			// return *string* reserved values
{
    char *v11;
	char Dest[32];
	
	switch (VarNum)                 // VarNum is actually a bitmask
	{
		case 0x0001:							// 'path'
            return cwd;

		case 0x0002:							// 'version'
			return getclver();

		//case 0x0004:	Unallocated?

		case 0x0008:							// 'date'
			return sysdate(dstr);

		case 0x0010:							// 'time'
			return systime(tstr);

		case 0x0020:							// 'pname'
            return pname;

		case 0x0040:							// 'uname'
			if ( !uname_0[0] )
				guname(uname_0);
			return uname_0;
		
		case 0x0080:							// 'pid'
			if ( !pid[0] )
				sprintf(pid, "%d", getpid());
            return pid;

		case 0x0100:							// 'uid'
			if ( !uid[0] )
				sprintf(uid, "%d", getuid());
            return uid;

		case 0x0200:							// 'login'
            if ( !login[0] )
                glogin(login);
            return login;

		case 0x0400:							// 'tty'
			if ( !tty[0] )
            {
                cdbcpystr(Dest, ttyname(2), 0);

				if (strncmp(Dest,"/dev/",5))	
				{	// string didn't start with '/dev/'
					// look for last '/'. If not found, whole string is returned
					for ( v11 = &Dest[lenstr(Dest)] ; Dest < v11 ; v11-- )
					{
						if ( *v11 == '/')
						{
							v11++;
							break;
						}
					}
				}
				else	
					v11 = &Dest[5];		// '/dev/' *was* found, skip past '/dev/'

                cdbcpystr(tty, v11, 0);
			}
            return tty;

		case 0x0800:							// 'nname'
			return getnodename();

		//case 0x1000:	Unallocated?

		case 0x2000:							// 'pwd' current directory
			v11 = strrchr(cwd, '/');			// find *last* occurence of '/'
			return v11 ? v11 + 1 : cwd;

		//case 0x4000:							// Clcomp sees 0x4000 as 'licensee'
		//case 0x8000:	Unallocated?
		default:
            return 0;
	}
    //return result;

}

int getnresv(int a1)		// return *numeric* reserved values
{
	switch (a1)
	{
		case 0x0008:					// 'date'
            sysdate(dstr);
            return clgetdate(dstr);

		case 0x0010:					// 'time'
			systime(tstr);
			return (int)gettime(tstr);

		case 0x0080:					// 'pid'
            return getpid();

		case 0x0100:					// 'uid'
            return getuid();

		default:						// rest can't be expessed numerically
			return 0;
	}
}


#endif
