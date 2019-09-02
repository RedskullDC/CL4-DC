#ifndef ESCAPE_C
#define ESCAPE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

bool	found_0 = false;		// static

int disp_esc(ONESC *esc, bool local)
{
    BTAB	*btb;
	char	*key_name;
	char	*v11;
	
	char	macro_name[32];
	char	Buff[128];
	char	Buffer;

    if ( local )				// local = true means look for a field specific escape
		found_0 = false;
    
	if ( esc )
    {
        while ( !esc->OESCFlags )
        {
            //if ((*__ctype_b_loc())[esc->KeyNumber] & 0x0400 )
            if (isalpha(esc->KeyNumber))
            {
                found_0 = true;
                btb = &btab[esc->BlockNo];
                v11 = Key_GetEscMacroName(esc->KeyNumber);
                if ( v11 )
                    sprintf(macro_name, "(%s) ", v11);
                else
                    macro_name[0] = 0;
                sprintf(Buff, "%s field: escape '%c' %s%s <CR>", (local ? "this" : "any"), esc->KeyNumber, macro_name, btb->BlockName);
LABEL_24:
                prerr(Buff, 0, 0);
                at();
                Buffer = ' ';
                if ( linput(&Buffer, -1, 0) == -27 )
                    return 0;
            }
            esc = esc->NextESC;
            if ( !esc )
                goto LABEL_26;
        }

        btb = &btab[esc->BlockNo];
        found_0 = true;
        key_name = KeyToName(esc->KeyNumber);
		
		if (key_name && strncmp(key_name,"No such",7))	// no match on "No such...", so key_name is valid
	        sprintf(Buff, "%s field: on_key %s %s <CR>", (local ? "this" : "any"), key_name, btb->BlockName);
        else
			sprintf(Buff, "%s field: on_key %c %s <CR>", (local ? "this" : "any"), (char)esc->KeyNumber, btb->BlockName);
        goto LABEL_24;
    }

LABEL_26:
    if ( local || found_0 || (cdbcpystr(Buff, "no escapes available <CR>", 0), prerr(Buff, 0, 0), at(), Buffer = ' ', linput(&Buffer, -1, 0) != -27) )
		return 1;
    return 0;
}

int _chkesc(ONESC *esc, short KeyNumber)
{
	BTAB	*btb;
	int		v5;
	int		result;
	short	v4;
	
    if ( esc )
    {
        while ( esc->KeyNumber != KeyNumber )
        {
            esc = esc->NextESC;
            if ( !esc )
                return 0;
        }
        btb = &btab[esc->BlockNo];
        btb->Curr_Mode = 's';
        dscreen(btb, 0, 0);
        do
        {
            v4 = ploop(btb, 0, btb->StartLine, btb->EndLine, btb->StartLine, btb->EndLine, 0);
            v5 = v4;
        }
        while ( v4 == -22 );
        result = 1;
        if ( v5 )
            result = v5;
    }
    else
    {
        result = 0;
    }
    return result;
}

int chkesc(ONESC *esc, int key_char, char *a3)
{
    int result;
	char v4;
	
    v4 = *a3;
    if ( key_char == 32 )	// if the user types <ESC><SPC> , display a list of possible escape sequences
    {
        if (disp_esc(esc, true))		// true == try field specific escape first
            disp_esc(oelist, false);
        prerr(0, 0, 0);
        result = 1;
    }
    else
    {
        if ( !esc || (result = _chkesc(esc, key_char)) == 0 )
            result = _chkesc(oelist, key_char);
    }
    *a3 = v4;
    return result;
}

#endif
