#ifndef CHKPATH_C
#define CHKPATH_C

//#define _LARGEFILE64_SOURCE
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>	// for strncmp()
#include "DBdefs.h"
#include "cl4.h"

char	file_0[8192];

// a1 must be in global mem, not on caller's stack since we are modding directly!!
// segment violation will occur if a1 on caller's stack!!

void rmdots(char *a1)
{
	char *v6;
	char *v17;		
	char *v18;		

	char *j;		
	signed int v20; 
	char *s;		
	char *sa;		
	char *v24;		

	//printf("rmdots(%s)\n",a1);
	do
	{
		v20 = 0;
		v24 = 0;
		// look for all instances of "/./" and replace with "/" 
		for ( s = a1; ; s = sa + 1 )
		{
			sa = strchr(s, '/');
			if ( !sa )
				break;
			v18 = sa;
			while (!(strncmp(sa,"/./",3)))
			{
				v6 = v18 + 2;	// point at trailing '/'
				while (*v6)
				{
					*++v18 = *++v6;	
				}
				v18 = sa;
			}
		}
		//printf("passed 1st section a1 = %s\n", a1);
		
		// now look for all instances of "/../" and replace with "/"
		for ( sa = a1; ; v24 = sa )
		{
			sa = strchr(sa + 1, '/');
			if ( !sa )
				break;
			if ( v24 )	// first run through, v24 always == 0
			{
				if (!(strncmp(sa,"/../",4)))
				{
					v17 = sa + 3;
					for ( j = v24; *v17; )
					{
						*++j = *++v17;
					}
					v20 = 1;
				}
			}
		}
	}
	while ( v20 );
	//printf("rmdots returned %s\n",a1);
}

/*
char* permbits(char* Buf, unsigned short st_mode)
{
    Buf[0] =  (S_ISDIR(st_mode)) ? 'd' : '-';	// check to see if directory entry
    Buf[1] =  (st_mode & S_IRUSR) ? 'r' : '-';	// user
    Buf[2] =  (st_mode & S_IWUSR) ? 'w' : '-';
    Buf[3] =  (st_mode & S_IXUSR) ? 'x' : '-';
    Buf[4] =  (st_mode & S_IRGRP) ? 'r' : '-';	// group
    Buf[5] =  (st_mode & S_IWGRP) ? 'w' : '-';
    Buf[6] =  (st_mode & S_IXGRP) ? 'x' : '-';
    Buf[7] =  (st_mode & S_IROTH) ? 'r' : '-';	// others
    Buf[8] =  (st_mode & S_IWOTH) ? 'w' : '-';
    Buf[9] =  (st_mode & S_IXOTH) ? 'x' : '-';
	Buf[10] = 0;
	return Buf;
}
*/

bool canopen(char *FileName, unsigned short perms)
{
	unsigned short v3;
	struct stat64 stat_buf;
	//char PermBuff[11];
	//char Permsreq[11];


	//eprint("canopen(%s, perms = x%02X)\n",FileName,perms);
	if ( strlen(FileName) <= 127 )
	{
		if ( __xstat64(3, FileName, &stat_buf))		// returns 0 to indicate no error?
			return false;

		if (!geteuid()||!getuid())	// effectively UMASK 000?
			return true;

		//eprint("canopen 92 (%s, permsreq = x%02X (%s), fileperms = '%s' )\n",FileName,perms,permbits(Permsreq, perms), permbits(PermBuff, stat_buf.st_mode));

		if ( stat_buf.st_uid == getuid() )				// st_uid;     /* user ID of owner */	true if current user owns the file
			v3 = perms;
		else if (stat_buf.st_gid == getgid())			// st_gid;     /* group ID of owner */	true if current user is in same group
			v3 = perms >> 3;
		else
			v3 = perms >> 6;							// all others

		//if (stat_buf.st_mode & v3)	// ** not correct ** should test all bits match. if ((stat_buf.st_mode & v3) == v3)
		//	return true;			

		if ((stat_buf.st_mode & v3) == v3)				// st_mode;    /* protection */
			return true;
	}
	return false;
}
/*

		DumpBlock(&stat_buf, sizeof(stat_buf));
		printf("getuid() = %d [x%08X]\n", getuid(),getuid());
		printf("getgid() = %d [x%08X]\n", getgid(),getgid());

		printf("st_uid  = %d [x%08X]\n", stat_buf.st_uid, stat_buf.st_uid);
		printf("st_gid  = %d [x%08X]\n", stat_buf.st_gid, stat_buf.st_gid);
		printf("st_mode = %d [x%08X]\n", stat_buf.st_mode, stat_buf.st_mode);

BFFFEEE0 = 01 08 00 00 00 00 00 00 00 00 00 00 F8 EB 9D 00  ................
BFFFEEF0 = F6 81 00 00 01 00 00 00 8E 1E 00 00 64 00 00 00  ............d...
BFFFEF00 = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 05 00  ................
BFFFEF10 = 00 00 00 00 00 10 00 00 80 02 00 00 00 00 00 00  ................
BFFFEF20 = 8A 58 07 51 00 00 00 00 56 C4 88 4A 00 00 00 00  .X.Q....V..J....
BFFFEF30 = 9A F6 01 51 00 00 00 00 F8 EB 9D 00 00 00 00 00  ...Q............
getuid() = 7822 [x00001E8E]
getgid() = 100 [x00000064]
st_uid  = 7822 [x00001E8E]
st_gid  = 100 [x00000064]
st_mode = 33270 [x000081F6]
		
*/		

bool chkperm(char *a1, int a2, unsigned short perms)				// returns true if we can open, false otherwise
{
	char FileName[128];

	//eprint("chkperm a1 = %s, a2 = %d, perms = x%02X\n",a1,a2,perms);
	if ( a2 )	// not used in clcomp.
	{
		cdbcpystr(FileName, a1, ".enc", 0);
		if ( canopen(FileName, perms) )
			return true;

		cdbcpystr(FileName, a1, ".ent", 0);
		return canopen(FileName, perms);			
	}
	else
		return canopen(a1,perms);
}

char *chkpath(char *a1, char *include, char *Name, int a4, unsigned short perms)
{
	char *v6;
	char *v9;
	char *v10;
	char *v17;

	bool v18;
	char Buffer[256];

	//eprint("chkpath(a1 = \"%s\", include %s, Name %s, a4 %d, perms %d)\n",a1,include,Name,a4,perms);
	Buffer[0] = 0;
	
	if ( a1 )
	{
		v6 = getdirname(a1);	// look for '~' or $HOME type re-directions
		v17 = v6;
		if ( *v6 == '/' )								// getdirname resolved to absolute, or user specified absolute in ent file	
		{												// eg: getfile "/home/leslie/TESTDB" Tranfl1,t1
			if (chkperm(v6, a4, perms))
				return v17;
		}
		else
		{
			if ( *v6 == '.' )
			{
				if ( v6[1] == '/' )							//	"./"
				{
					cdbcpystr(Buffer, ":", 0);
					v17 += 2;
				}
				else if ( v6[1] == '.' && v6[2] == '/' )	// "../"
				{
					cdbcpystr(Buffer, "..:", 0);
					v17 += 3;
				}
			}
			if ( Buffer[0] == 0 )	// 
			{
				if ( include )								// additional include dir specified on command line
					cdbcpystr(Buffer, include, "/:", 0);
				
				cdbcpystr(Buffer, Buffer, getevar(Name), 0);

				if ( Buffer[0] != ':' )
					cdbcpystr(Buffer, Buffer, "::", 0);
			}
			//eprint("chkpath 210 Buffer = \"%s\"\n",Buffer);
			v9 = Buffer;
			v10 = Buffer;
			while ( 1 )
			{
				while ( *v9 && *v9 != ':' )				// ':' to seperate different search directories
					++v9;
				v18 = *v9 == 0;
				*v9 = 0;
				
				if ( v10 == v9 )
					cdbcpystr(file_0, cwd, "/", v17, 0);
				else if ( *v10 != '.' )
					cdbcpystr(file_0, v10, "/", v17, 0);
				else
					cdbcpystr(file_0, cwd, "/", v10, "/", v17, 0);

				rmdots(file_0);
				if (chkperm(file_0, a4, perms))
					return file_0;						// found it, and we can open it. exit_success 
				
				++v9;
				v10 = v9;
				if ( v18 || !*v9 )
					return 0;
			}
		}
	}
	return 0;	// not found err_exit
}


#endif
