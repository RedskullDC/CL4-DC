#ifndef PRINTERR_C
#define PRINTERR_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <termcap.h>
#include <stdlib.h>		// for srand48()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

/*void prmsg(char *format, ...)
{
    char Message[256];
	va_list	va;

    va_start(va, format);
    vsprintf(Message, format, va);	// original does some hokey asm call to sprintf()!
    _cx = _co - 30;
    _cy = _li;
    if ( toscreen(_cx, _cy, 0, strlen(Message), Message, (unsigned short)_Ym, 8, 0) )
        erase_line();
	va_end(va);
}
*/

void prmsg(char *format, char *a2, char *a3)
{
    char Message[1024];
	
    sprintf(Message, format, a2, a3);
    _cx = _co - 30;						// last line, right hand side
    _cy = _li;
    if ( toscreen(_cx, _cy, 0, strlen(Message), Message, (unsigned short)_Ym, 8, 0) )
        erase_line();
}

void prerr(const char *format, void* a2, int Bell)
{
    int MaxLen;
	char ErrBuffer[1024];
	
    if ( Bell )
    {
        fputc(7, stderr);
        fflush(stderr);
    }
    if ( format )
    {
        _cx = incvar(v_MCOL, 0);				// "MCOL"
        _cy = incvar(v_MLINE, 0);				// "MLINE"
        
		sprintf(ErrBuffer, format, a2);
        erase_line();
        MaxLen = lenstr(ErrBuffer);
        if ( Bell )
	        toscreen(_cx, _cy, 0, MaxLen, ErrBuffer, (unsigned short)_Ye, 2, 0);
        else
			toscreen(_cx, _cy, 0, MaxLen, ErrBuffer, (unsigned short)_Ym, 8, 0);
    }
    else	// pass \0 is a quick way to blank the last line
    {
        _cx = 1;
        _cy = _li - 1;
        at();
        erase_line();
    }
}

#endif
