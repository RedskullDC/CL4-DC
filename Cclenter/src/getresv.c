#ifndef GETRESERVE_C
#define GETRESERVE_C

#include <stdio.h>
#include <stdarg.h>			// for var args stuff
#include <string.h>			// strncmp()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"

char *getresv(int VarNum)			// return *string* reserved values
{
    char *v11;
	char Dest[32];

	switch (VarNum)                 // VarNum is actually a bitmask
	{
		case rsv_CWD:							// 'path'
            return cwd;

		case rsv_VERSION:							// 'version'
			return getclver();

		//case 0x0004:	Unallocated?

		case rsv_DATE:							// 'date'
			return sysdate(dstr);

		case rsv_TIME:							// 'time'
			return systime(tstr);

		case rsv_PNAME:							// 'pname'
            return pname;

		case rsv_UNAME:							// 'uname'
			if ( !uname_0[0] )
				guname(uname_0);
			return uname_0;
		
		case rsv_PID:							// 'pid'
			if ( !pid[0] )
				sprintf(pid, "%d", getpid());
            return pid;

		case rsv_UID:							// 'uid'
			if ( !uid[0] )
				sprintf(uid, "%d", getuid());
            return uid;

		case rsv_LOGIN:							// 'login'
            if ( !login[0] )
                glogin(login);
            return login;

		case rsv_TTY:							// 'tty'
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

		case rsv_NNAME:							// 'nname'
			return getnodename();

		//case 0x1000:	Unallocated?

		case rsv_PWD:							// 'pwd' current directory
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
		case rsv_DATE:					// 'date'
            sysdate(dstr);
            return clgetdate(dstr);

		case rsv_TIME:					// 'time'
			systime(tstr);
			return (int)gettime(tstr);

		case rsv_PID:					// 'pid'
            return getpid();

		case rsv_UID:					// 'uid'
            return getuid();

		default:						// rest can't be expessed numerically
			return 0;
	}
}


#endif
