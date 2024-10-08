#ifndef GETFLAGS_C
#define GETFLAGS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for strtol, strtod etc.
//#include "DBdefs.h"
//#include "cl4.h"

typedef struct 
{
	short	FlagIndex;		//0 
	short	FlagType;		//2
} FLAG;

char *arg(int *argc, char ***argv, short FlagType)
{
	char **v4;
	
	if ( (**argv)[1] || FlagType == 4 )		// No gap between flag and value, or expecting a single char value
	{
		return **argv + 1;					// pointer to the single character
	}
	else
	{
		--*argc;
		v4 = *argv;
		++*argv;
		
		//printf("arg()#3 - *argv = %08X, v4 = %08X, v4[1] = %08X (%s)\n", *argv, v4, v4[1], v4[1]);
		if ( *argc )
			return v4[1];
	}
	return 0;
}

bool flagindex(char *v4, char Val, FLAG *flag)
{
	short	Index = 0;

//	printf ("flagindex - v4 = %s, Val = %C, \n",v4,Val);
	while ( *v4 )					// example of index string passed in "D,I#,L*,C?"
	{
		if ( *v4 == Val )
		{
			flag->FlagIndex = Index;			// update caller mem directly
			switch (*++v4)
			{
			case '*':
				flag->FlagType = 3;				// string argument
				return true;
				break;

			case '?':
				flag->FlagType = 4;				// Single char value
				return true;
				break;

			case '#':
				if ( v4[1] == '#' )
					flag->FlagType = 2;			// ## = long integer
				else
					flag->FlagType = 1;			// #  = short integer
				return true;
				break;

			case ':':							// : = boolean
			case ',':							// no specifier also == boolean
			case 0:								// no specifier, and we are last parameter!
				flag->FlagType = 0;
				return true;
				break;
			}
		}
		if ( *v4++ == ',' )		// separator
			Index++;
	}
	return false;	// error_exit
}

bool getflags(int *argc, char ***argv, char *flags, ...)
{
	char	**v12;
	char	*v3;
	char	*v9;
	char	*strarg;
	int		v5;
	FLAG	Flag;
	void	*v16[38];
	va_list va;
	
	va_start(va, flags);		// grab any arguments which follow flags
	v3 = va_arg(va,char*);
	v5 = 0;
	while (v3)
	{
		v16[v5++] = v3;
		v3 = va_arg(va,char*);
	}
	v16[v5] = 0;	// NULL terminated list
	va_end(va);     // keep va macros happy

	v12 = *argv;
	++*argv;
	while ( --*argc && *v12[1] == '-' )
	{
		v9 = **argv + 1;
		**argv = v9;										// Update caller memory directly
		if ( *v9 )
		{
			while (flagindex(flags, ***argv, &Flag))		// Flag updated by subroutine
			{
				//printf("Type = %d, Index = %d\n",Flag.FlagType,Flag.FlagIndex);
				if ( Flag.FlagType )
				{
					strarg = arg(argc, argv, Flag.FlagType);
					if ( strarg )
					{
						switch (Flag.FlagType)
						{
						case 1:
							*(short *)v16[Flag.FlagIndex]	= strtol(strarg, 0, 10);	// string arg to short
							break;
						case 2:
							*(int *)v16[Flag.FlagIndex]		= strtol(strarg, 0, 10);	// string arg to long
							break;
						case 3:
							*(char**)v16[Flag.FlagIndex]	= strarg;					// string argument
							break;
						case 4:
							*(char*)v16[Flag.FlagIndex]		= *strarg;					// single char value
							break;
						}
						goto LABEL_19;
					}
					return false;			// error exit	- couldn't read parameter when expected			
				}
				*(int *)v16[Flag.FlagIndex] = 1;			// default = bool on
		
				v9 = **argv + 1;
				**argv = v9;
				if ( !*v9 )
					goto LABEL_19;
			}
			return false;					// error return : unknown flag type found
		}

LABEL_19:		
		v12 = *argv;
		++*argv;
	}
	return true;	// exit_success
}

#endif
