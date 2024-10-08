#ifndef PRINT_SCREEN_C
#define PRINT_SCREEN_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

bool prscreen(void)
{
    char	*v1;
    FILE	*stream;
	int		j;
	int		i;
	char	filename[128];		// ** should add PID to differentiate, in case running in shared dir?
	
    cdbcpystr(filename, pname, ".scr", 0);
    if ( (npr_scr || pr_scr) && (stream = fopen64(filename, "a")) != 0 )
    {
        tbline(stream);
        v1 = abuf;
        for ( i = 0; i < _li; ++i )
        {
            if ( npr_scr )							// show screen line number at left side
                fprintf(stream, "%2d*", i + 1);
            else
                fputc('*', stream);
            for ( j = 0; j < _co; v1++ )
            {
                if ( *v1 )
					fputc(*v1, stream);
                else
					fputc(' ', stream);				// space for NULLs
                j++;
            }
            fprintf(stream, "*\n");
        }
        tbline(stream);
        fputc('\f', stream);						// formfeed
        fclose(stream);
        return true;								// exit_success printed file OK
    }
	return false;
}

void tbline(FILE *stream)
{
    int i;
    int v2;

    if ( npr_scr )								// show screen line number at left side
        fwrite("  ", 1u, 2u, stream);			// leave room for 2 chars of line number
    
	for ( i = 0; i < _co + 2; i++ )
    {
        if ( npr_scr && i && !(i%10))			// i MOD 10
        {
            v2 = i / 10;
            if ( v2 >= 10 )						// > 100 column screen
                v2 -= 10;						// will work for up to 200col screens
            fprintf(stream, "%d", v2);
        }
        else
            fputc('*', stream);

	}
    fputc('\n', stream);
}


#endif
