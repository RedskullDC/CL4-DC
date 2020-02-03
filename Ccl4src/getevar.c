#ifndef GETEVAR_C
#define GETEVAR_C

#include <stdio.h>
#include <stdlib.h>
#include "DBdefs.h"
#include "cl4.h"

char* getevar(char *name)
{
	char	*v2;
	FILE	*fp;
	size_t	NumBytes;
	char	Key[128];		// Key buffer     'xyz='
	char	Buff[8192];

	memset(str_0, 0, 8192u);
	v2 = getenv(name);	// 1st, look for a system variable with that name
	if ( v2 && *v2 )
	{
		cdbcpystr(str_0, trim(v2), 0);
	}
	else	// if not found, look for an entry in CLSETUP file
	{
		v2 = getenv("CLSETUP");
		if ( !v2 || !*v2 )
			v2 = "/etc/CLSETUP";	// look for a global CLSETUP file
		fp = fopen64(v2, "r");
		if ( fp )
		{
			cdbcpystr(Key, name, "=", 0);			// no sanity check on key size
			NumBytes = strlen(Key);
			while ( fgets(Buff, 8192, fp) )
			{
				if ( cmpbuf(Key, Buff, NumBytes) )
				{
					cdbcpystr(str_0, trim(&Buff[NumBytes]), 0);
					break;
				}
			}
			fclose(fp);
		}
	}
	return str_0;
}

void setupEnv()
{
	FILE *fp;
	char *v2;
	char Buff[8192]; // [sp+10h] [bp-2018h]@5

	//printf("setupEnv() called\n");

	v2 = getenv("CLSETUP");
	if ( !v2 )
		v2 = "./CLSETUP";
	fp = fopen64(v2, "r");
	//printf("setupEnv(): fp = %d\n", fp);
	if ( fp )
	{
		while ( fgets(Buff, 8192, fp) )
		{
			trim(Buff);
			if ( Buff[0] != '#' )
			{
				v2 = strchr(Buff, '=');
				if ( v2 )
				{
					*v2 = '=';	// seems redundant. *v2 was already a '*' ?!?
					v2 = (char*)mstrcpy(Buff, 0);
					//printf("setupEnv() : found : %s\n", v2);
					putenv(v2);
				}
			}
		}
		fclose(fp);
	}
}

#endif
