#ifndef REPLY_C
#define REPLY_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <termcap.h>
#include <stdlib.h>		// for srand48()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

char reply(char *ReplyMsg, char *ValidChars)
{
    FLDdesc *fldREPLY;
	char	*v3;
	char	*v9;
	int		v7;
	int		v8;
	short	v4;
	short	save_align;
	
	short	Column = 0;
	short	Line = 0;

	bool	EXIT_PROG;
	bool	HideMsg;
	bool	ValidFound;
	bool	v18;
	char	EscChar;

	char	Buffer[2];
	char	reply_buff[256];
	
	//printf("reply: (%s,%s)\n",ReplyMsg,ValidChars);

    EXIT_PROG = false;	// fast exit
    zap(Buffer, 2u);
    
	fldREPLY = &ttab->TTfields[v_REPLY];	// "REPLY" fld

    if ( *(char *)fldREPLY->FLDdata == 'E' )
    {
        v3 = ccbuf;					// screen attribute buffer
        v4 = _li * _co;
        while ( v4 > 0 && !*v3 )	// see if program has created any screen output
        {
	        --v4;
            ++v3;
        }
        if ( v4 )
            prerr(0, 0, 0);			// clear message line
        goto LABEL_52;				// crash out via a longjmp to main()!!!
    }

	if ( fldREPLY->FLDstat & fld_ZERO )			// REPLY not set, use the default valid characters
        *Buffer = *ValidChars;
    else
    {
        *Buffer = *(char *)fldREPLY->FLDdata;	// REPLY *was* set, use it as the prompt, and ZERO REPLY variable
        *(char *)fldREPLY->FLDdata = 0;
        fldREPLY->FLDstat |= fld_ZERO;
    }
    
	if ( !(isupper(*Buffer)) && (_mreply || ValidChars[1] != 'r' || repeat) )
        HideMsg = false;
    else
    {
        *Buffer = tolower(*Buffer);				// REPLY was set to uppercase char. Skip showing next message
        HideMsg = true;
    }

	ValidFound = (HideMsg && *Buffer == 'e') ? true : false ;
    
	while ( !ValidFound )
    {
        if ( !HideMsg )
        {
            EXIT_PROG	= false;
            v18			= false;
            prerr("%s", ReplyMsg, 0);
            if ( !Column )
            {
                Column = _cx + 2;
                Line = _cy;
            }
            
			do
            {
                save_align = align;
                align = 0;
                toscreen(Column, Line, 0, 1, Buffer, 0, 1, 1);
                align = save_align;

                whereat(Column, Line, 0, 0);
                at();
				
				v7 = linput(&Buffer[0], (_YR ? -1 : 1), 0);		// _YR == true, auto reply fields allowed
                if ( v7 <= 0 )
                {
                    v8 = chkesc(0, -v7, &EscChar);
                    prerr("%s", ReplyMsg, 0);
                    if ( v8 == -5 )
                    {
                        exit_type = 0;
                        v18 = true;
                        *Buffer = 'E';
                    }
                }
                else
                    v18 = true;
            }
            while ( !v18 );
            
			fillbuf(reply_buff, Column + 1, ' ');
            reply_buff[Column + 1] = 0;
            toscreen(1, Line, 0, Column + 1, reply_buff, 0, 0, 1);
            whereat(1, Line, 0, 0);
            at();
            
			if ( *Buffer == '.' )			// some fudgey aliases for <a> and <c>hange
                *Buffer = 'a';
			else if ( *Buffer == ',' )
				*Buffer = 'c';
			else if (isupper(*Buffer))
            {
				EXIT_PROG = (*Buffer == 'E' && bige == 1) ? true : false ;	// bige controlled by "set E,on|off" default == on
				
				*Buffer = tolower(*Buffer);
                if ( EXIT_PROG )
					break;
            }
        } // end if ( !HideMsg )
        
        for  ( v9 = ValidChars ; *v9; v9++ )
        {
			if ( *Buffer == *v9 || *Buffer == tolower(*v9) )
				ValidFound = true;			// Found a valid match on the char entered
        }
        if ( !ValidFound && !HideMsg )
            fputc(7, stderr);				// bell char
		
		HideMsg = false;					// If we go around again, force message to be shown
    }
    prerr(0, 0, 0);							// clear message line

    if ( EXIT_PROG )
LABEL_52:
        longjmp(sjbuf, 1);					// crash back out to main()	****************
    
	setcvar(v_LREPLY, &Buffer[0]);			// "LREPLY"
    return Buffer[0];
}

void mreply(char *a1, char *a2)
{
    _mreply = 1;
    reply(a1, a2);
    _mreply = 0;
}

#endif
