#ifndef CHECK_LIT_C
#define CHECK_LIT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

char *chklt(char *a1)			// only called from pfork.c - return code is ignored
{
    char	*j;
	char	*v18;
	char	*Dest;
	
    int		v9;
	unsigned int v12;
	
	char	Quote;				// stores ' or "
	
	char	TranIDstr[32];		// log file transaction number as string. 32 is overkill
	char	LogIDstr[32];		// log file process ID as string. 32 is overkill
	
	Dest = a1;
	while((v18 = strchr(Dest, 'c')) != 0)			// why not just do v18 = strstr(a1,"clenter");  ???
	{
		if (!strncmp(v18,"clenter",7))
			break;
		Dest = v18 + 1;
	}

	// if v18 is set here, it is pointing at 'clenter....blah blah'
    if ( v18 )
    {
        for ( Dest = v18 + 7; *Dest; Dest++ )				// +7 == skip past 'clenter' on cmdline
        {
			if ( *Dest == '-' )		// found a command line switch option
			{
				v9 = *++Dest;
				if ( v9 == 'L' )							// -L {pid} = log file process ID
				{
					Dest++;
					sprintf(LogIDstr, "%d", lpid);			// ** global var ** - initialised in main()
					
					v12 = strlen(LogIDstr) + 1;
					for ( j = (char *)&a1[strlen(a1)]; Dest <= j; --j )
						j[v12 - 1] = *j;

					cpybuf(Dest, LogIDstr, (v12 - 1));
				}
				else if ( v9 == 'T' )						// -T {tran} = log file transaction number
				{
					Dest++;
					sprintf(TranIDstr, "%ld", cl_tx);		// ** global var ** - initialised in main()
					
					v12 = strlen(TranIDstr) + 1;
					for ( j = (char *)&a1[strlen(a1)]; Dest <= j; --j )
						j[v12 - 1] = *j;
					
					cpybuf(Dest, TranIDstr, (v12 - 1));
				}
				else if (!v9)
					--Dest;									// end of string, will force end of for loop next iteration
			}
            else if ( *Dest == '\'' || *Dest == '"' )
            {
                //Quote = *Dest;		// Save the Quote char type to find the matching end char
                //v8 = Dest + 1;
                //Dest = v8;
                Quote = *++Dest;		// Save the Quote char type to find the matching end char
				if ( !*Dest )
                    return a1;

                if ( *Dest != Quote )
                {
                    do
                    {
                        ++Dest;
                        if ( !*Dest )
                            return a1;
                    }
                    while ( *Dest != Quote );
                }
                if ( !*Dest )
                    return a1;
            }
        }
    }
    return a1;		// return unchanged
}

#endif

