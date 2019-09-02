#ifndef PROG_BLOCK_C
#define PROG_BLOCK_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <termcap.h>
#include <stdlib.h>		// for srand48()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int pblock(BTAB *bt, ENTAB *entab)
{
    PTAB	*ptb;
    BTAB	*btb2;
    FLDdesc *fldMode;

    char	*v8;
	char	*v12;
	char	*v17;
	char	*v27;
	char	*v28;
	
    int		v9;
	int		v30 = 0;
    int		v24 = 0;
    int		v11;
	int		v16;
	int		result;
	int		v23;
	short	curr_nflds = 0;

	bool	screenLayoutDone;
    char	MODE;
	char	v26;
	char	v13;
	char	v25 = 0;
	FLDdesc fld;
    
	char	Dest[32];
	char	Maint[32];
	char	Src[64];
	char	s[64];
	
    if ( bt == btab )		// are we at the start of the btab array? Yes if 1st run through!
    {
        ptb = PTARR(getptabp(bt->StartLine));
        while ( ptb->OpCode != 999 )		// while ! OpCode == "end"
        {
			srclineno = ptb->SrcLineNo;
            if ( ptb->OpCode == 1 )			// found first accept field, jump out
				break;
			switch (ptb->OpCode)			// these are all set statements, affect entire block
			{
				case 650:					// message
                    _Ym = ptb->Operand;
					break;
				case 658:					// prompt
					_Yp = ptb->Operand;
					break;
				case 662:					// head
                    _Yh = ptb->Operand;
					break;
				case 679:					// set repeat
                    repeat = ptb->Operand == 1;
					break;
			}
            ptb++;
        }
		//printf("pblock: No accept field found! \n");
	}
//-------------------------

	zap(s, 20u);
    if ( entab )
        s[0] = *(char *)(getftf(entab, true, &fld)->FLDdata);

    Dest[0] = 0;
    cdbcpystr(Src, "e", 0);				// default exit mode = 'e'
    if ( s[0] == 'E' )
        Src[0] = 'E';
    
	cdbcpystr(Maint, bt->Maintain, 0);	// copy the maintain codes that were read at compile time
    
	if ( !Maint[0] )					// Shouldn't be reachable. clcomp selects "vac" as default if no modes specified
    {
        cdbcpystr(Maint, "er", 0);		// default values: <e>nd and <r>epeat ?
        v25 = 'r';
        v30 = 1;
	}
	else
	{
		v8 = Maint;
		v9 = -1;
		do
		{
		    if ( !v9 )	// unreachable
		        break;
		    v13 = *v8++ == 0;
		    --v9;
		}
		while ( !v13 );

		if ( v9 == -3 )
		{
		    v25 = Maint[0];
		    v30 = 1;
		}
	}

//---------------------
    v28 = Maint;
    while ( *v28 )
    {
        v11 = *v28;
        if (isupper(v11))
			*v28 = tolower(v11);
		switch ( *v28 )
        {
			case 'a':
				v27 = "<a>dd ";
                v12 = "a";
                break;
			case 'r':
				v27 = "<r>epeat ";
                v12 = "r";
				break;
			case 'c':
				v27 = "<c>hange ";
                v12 = "c";
                break;
			case 'd':
				v27 = "<d>elete ";
                v12 = "d";
                break;
			case 'v':
				v27 = "<v>iew ";
                v12 = "v";
                break;
			default:
				v12 = 0;
                v27 = 0;
                break;
		}
        if ( v12 && ((v23 = *v12, !( isupper(s[0]))) ? (v13 = v23 == s[0]) : (v13 = v23 == tolower(s[0])), v13) )
        {
			cdbcpystr(s, s, Src, 0);
            cdbcpystr(Src, s, 0);
		}
        else
			cdbcpystr(Src, Src, v12, 0);

		cdbcpystr(Dest, Dest, v27, 0);
        v28++;
    }
    cdbcpystr(Dest, Dest, "<e>nd", 0);		// <e>nd always valid option

//--------------------------------------
    if ( !v30 )
    {
        v30 = isupper(Src[0]);
        if (v30)
            v25 = tolower(Src[0]);
    }
    
	v26 = 0;
    screenLayoutDone = false;
    while ( v26 != 'e' && Src[0] != 'E' )	// while not exit
    {
		if ( !screenLayoutDone )
        {	
			curr_nflds = dscreen(bt, 0, 0);	// layout the current screen, returns global variable 'nflds'
            screenLayoutDone = true;
		}
            
		if ( v30 )
			v26 = v25;
		else if ( fld_nbrs == 1 && Maint[1] != 'r' )
			prmsg("SELECT %s", ttab[bt->TTno].TableName, "");

		if ( !v30 )
        {
			v26 = reply(Dest, Src);
            if ( v26 == 'e' )
				break;
		}
        if ( Src[0] != 'e' )
        {
			cdbcpystr(s, "e", Src, 0);
            cdbcpystr(Src, s, 0);
		}
        
		fldMode = &ttab->TTfields[v_MODE];	// "MODE"
        *(char *)fldMode->FLDdata = v26;	// current block mode
        bt->Curr_Mode = v26;
        setcvar(v_REPLY, 0);				// "REPLY"

        while (abcd(bt, curr_nflds))		// execute abcd() until ZERO value returned
			;

		v16 = 0;	// some retarded coding here
        if ( !v30 )
			v16 = 0;
		v30 = v16;
            
		v26 = 0;
        v17 = (char *)fldMode->FLDdata;
        if ( bt->Curr_Mode != *v17 )
        {
			v26 = *v17;
            if ( v26 && v26 != 'e' )
				setcvar(v_REPLY, v17);		// "REPLY"
		}
        *(char *)fldMode->FLDdata = 0;

// --- Check for on_exit routine ---

        if ( bt->On_exit )
        {
			btb2 = &btab[bt->On_exit->BlockNo];
            v24  = ploop(btb2, 0, btb2->StartLine, btb2->EndLine, btb2->StartLine, btb2->EndLine,0);
            if ( v24 == -5 )
				return -5;
                
			MODE = *(char *)fldMode->FLDdata;	// "MODE"
			if ( isupper(MODE))
				MODE = tolower(MODE);
            
			switch (MODE)						// if !MODE, do nothing
			{
			case 'e':
				v26 = 'e';						// will quit out of main loop also
                prerr(0, 0, 0);					// clear message line
				break;
			case 'a':
			case 'c':
			case 'd':
			case 'v':
			case 'r':
				v25 = MODE;
                v30 = 1;
				break;
			}
		} // end on_exit

	}// end while ( v26 != 'e' && Src[0] != 'E' )  // while not exit
    return v24;
}


#endif
