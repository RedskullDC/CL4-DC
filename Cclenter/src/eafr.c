#ifndef EAFR_C
#define EAFR_C

#include <math.h>	// for fmod in time functions
#include <ctype.h>	// for isdigit() in gettime()
#include <stdlib.h>			// for strtol()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// eafr() is used to find a specific field number pointer when fnum is set to "on"

void _getfnum(char *Buffer, int TDno, short PT_Start, short PT_End)
{
    PTAB	*ptab;
	PTAB	*ptab_end;
	BTAB	*btb;
	XTAB	*xtb;
	
	int		TDno2;
    short	FieldNo;
	FLDdesc fld;
	char	FLDtype;

    ptab	 = PTARR(getptabp(PT_Start));
    ptab_end = PTARR(getptabp(PT_End));

    while (!found_name && ptab <= ptab_end )	// careful!  global found_name variable
    {
        srclineno = ptab->SrcLineNo;
        if ( ptab->OpCode == 720 )				// do {block_name}
        {
            btb = &btab[ptab->Operand];
            _getfnum(Buffer, TDno, btb->Screens->PT_start, btb->Screens->PT_end);	// nice recursion here
        }
        else if ( ptab->OpCode == 1 )           // accept field
		{
			xtb = XTARR(ptab->TABno);
            FieldNo = gettdfno(ENARR(xtb->VarExpNo), &TDno2, &FLDtype, 1);
			assert(FieldNo >= 0);

			//if ( !(xtb->Flags & xtb_DISPLAY) && (TDno2 != TDno || ttab[TDno2].TTfields[FieldNo].FLDstat >= 0))
			if ( !(xtb->Flags & xtb_DISPLAY) && (TDno2 != TDno || !(ttab[TDno2].TTfields[FieldNo].FLDstat & fld_KEY)))	// not display only, not Key field
			{
				++fnum;
				if (compare((char *)getftf(ENARR(xtb->Prompt_exp), 0, &fld)->FLDdata, Buffer) == 0)
					found_name = true;
			}
        }
		ptab++;
    }
}

void _ugetfnum(int TDno, short PT_Start, short PT_End)
{
    PTAB	*ptab;
	BTAB	*btb;
	XTAB	*xtb;
	PTAB	*ptab_end;
	
	signed int FieldNo;
	int		TTno;
	short	Column;
	short	Line;
	char	FLDtype;
	
    ptab	 = PTARR(getptabp(PT_Start));
	ptab_end = PTARR(getptabp(PT_End));

	while ( fnum && ptab <= ptab_end )		// careful!! global fnum variable
    {
        if ( ptab->OpCode == 720 )                // 720 = do {block_name}
        {
            btb = &btab[ptab->Operand];
            _ugetfnum(TDno, btb->Screens->PT_start, btb->Screens->PT_end);	// recursion!
        }
        else if ( ptab->OpCode == 1 )              // accept field
        {
			xtb = XTARR(ptab->TABno);
            FieldNo = gettdfno(ENARR(xtb->VarExpNo), &TTno, &FLDtype, 0);
			assert(FieldNo >= 0);
			
            //if ( !(xtb->Flags & xtb_DISPLAY) && (TTno != TDno || ttab[TTno].TTfields[FieldNo].FLDstat >= 0) )
            if ( !(xtb->Flags & xtb_DISPLAY) && (TTno != TDno || !(ttab[TTno].TTfields[FieldNo].FLDstat & fld_KEY)))	// not display only and (field in another table or not Key field)
            {
				--fnum;
                if ( fnum )
                {
					clgetyx(XTARR(ptab->TABno), &Column, &Line, yx_answer);	// position of answer field
                    whereat(Column, Line, true, 0);							// true == allow mask bits
				}
                pt_occ = ptab - PTARR(getptabp(0));   // ptab arithmetic!! ugghh!!!
			}
        }
		ptab++;
    }
}

int eafr(int TDno, short PT_Start, short PT_End, char *a4)
{
    char	*v9;
    signed int v6;
    short	v4;
    bool	Numeric;
    char	v7;
    char	Buffer[22];

    found_name	= false;	// all globals
    fnum		= 0;
    pt_occ		= 0;

    zap(Buffer, sizeof(Buffer));	// 22u);
    if ( fld_nbrs == 1 )
    {
        prerr("Enter field ", 0, 0);
        linput(Buffer, 21, 0);
        prerr(0, 0, 0);               // This is a quick way of clearing the Message line area
    }
    v4 = lenstr(Buffer);
    v6 = 0;
    if ( v4 )	// Can only be true if fnum is set to "on"
    {
		v4--;
        if ( Buffer[v4] == '.' )					// points at last char in Buffer[]
            Buffer[v4] = '+';
        v7 = Buffer[v4];
        *a4 = v7;									// pass value back to caller
			
		if ( v7 == '*' || v7 == '+' )
            Buffer[v4] = 0;
		
		// look for a 3 digit number!
        Numeric = false;
        v9 = Buffer;
        if ( v4 <= 3 )
        {
            while ( *v9 && isdigit(*v9))
                ++v9;
            Numeric = *v9 == 0;						// true if all chars in the string were numbers
        }
//------------------------------        
		if ( Numeric )								// User entered a valid field number 
            fnum = strtol(Buffer, 0, 10);
        else
        {
            cdbcpystr(Buffer, Buffer, "*", 0);		// User didn't enter a valid field number, do a wild card search
            _getfnum(Buffer, TDno, PT_Start, PT_End);
            if ( !found_name )						// global var
                fnum = -1;							// global var
        }
//------------------------------        
		if ( fnum > 0 )
        {
            _ugetfnum(TDno, PT_Start, PT_End);
            if ( fnum )
                v6 = -1;
            else
                v6 = pt_occ;
        }
        else
            v6 = fnum;
    }
    return v6;
}


#endif
