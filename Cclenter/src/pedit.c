#ifndef PEDIT_C
#define PEDIT_C

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

void setclvars(int ErrVal, int LockedVal)
{
    setvar(v_ERROR  , ErrVal);                          // "ERROR"
    setvar(v_LOCKED , LockedVal);                       // "LOCKED"
}

void pexcept(BTAB *bt)
{
    BTAB *btb; // eax@1

    btb = &btab[bt->On_excep->BlockNo];
    ploop(btb, 0, btb->StartLine, btb->EndLine, btb->StartLine, btb->EndLine, 0);
}

char *getkerr(char *Src, BTAB *btb, FLDdesc *fld2)
{
    PTAB	*ptb;
	XTAB	*xtb;
	ENTAB	*ent;
	FLDdesc *fld1;
	
	FLDdesc fld;
	char Buff[256]; // [sp+50h] [bp-88h]@11

    *Src = 0;
	ptb = PTARR(getptabp(btb->Screens->PT_start));
    
	while ( ptb->OpCode != 950 )	// 950 = "break"
    {
        srclineno = ptb->SrcLineNo;				// *** global variable ***
        if ( ptb->OpCode == 1 )                 // accept field
        {
            xtb		= XTARR(ptb->TABno);
            ent		= ENARR(xtb->VarExpNo);
            fld1	= getftf(ent, 1, &fld);
            
			if ( xtb->Flags < 0 && fld1 <= fld2 )
            {
                fldtobuf(Buff, fld1, 0);
                cdbcpystr(Src, Src, "'", Buff, "' ", 0);
            }
            if ( fld1 == fld2 )
                break;
        }
		ptb++;
    }
    return Src;
}

int eaddkey(BTAB *bt, PTAB *pt, FLDdesc *fld, int TDno)
{
	int		result;
    short	Error;
	char	Src[112];
	char	format[128];
	
    Error = rdrec(TDno, 0x0400u);				// 1024 = "lockr"  (0x400)
    if ( Error <= 0 && Error != -10 )
    {
        prerr(0, 0, 0);							// clear message line
        result = 0x40u;
    }
    else	// no error and/or record was locked
    {
        if ( bt->On_excep )
        {
            if ( Error == -10 )     // record locked
				setclvars(0,1);		// quick fn to set both ERROR and LOCKED variables  ErrVal = 0, LockedVal = 1, 
            else
				setclvars(1,0);		// ErrVal = 1, LockedVal = 0;
			pexcept(bt);            // do on_exception block routine
        }
        else
        {
            sprintf(format, "%snot unique", getkerr(Src, bt, fld), pt);
            prerr(format, 0, 1);
        }
        ulckrec(TDno);
        rfree(TDno, 0);
        disprec(bt, TDno, 0, 0, 0);
        result = 0x80u;
    }
    return result;
}

int egetkey(BTAB *bt, PTAB *ptb, XTAB *xtb, FLDdesc *fld, int TDno, int fno, short CurrPTno)
{
    RTAB	*rtb; // ecx@4
    RTAB	*v26; // ecx@4
    ENTAB	*v10; // edx@8
    ENTAB	*v11; // eax@10
    ENTAB	*v12; // eax@12
    FLDdesc *j; // edx@40
    void	*i; // edx@37

    signed int v7; // edi@2
    int		v9; // eax@8
    int		v15; // eax@26
    int		ErrorCode; // [sp+2Ch] [bp-9Ch]@1
    signed int v31; // [sp+30h] [bp-98h]@1
    signed int v32; // [sp+34h] [bp-94h]@1
    signed int v33; // [sp+38h] [bp-90h]@1
	short	RdRtn;
    char	SrchMode; // [sp+3Fh] [bp-89h]@33
    char	Src[128]; // [sp+40h] [bp-88h]@29
    char	v18; // zf@34

	//eprint("egetkey( TDNo = %d, fno = %d, CurrPTno = %d)\n",TDno,fno,CurrPTno);

    v33 = 0;
    v32 = 0;
    v31 = 0;
    ErrorCode = 0;
    ++no_fields;
    if ( !bt->BTFlags )
    {
		//eprint("egetkey 127\n");
        bt->EXPR = (EXPR *)fretre(bt->EXPR, 0);
        v31 = 0;
        v7 = 0;
        if ( xtb->Flags & xtb_JOIN )	// 0x0010 Does this field have a range specifier?
        {
			//eprint("egetkey 133\n");
            rtb = RTARR(xtb->RangeID);
            if ( TDno == rtb->TTno )
            {
                v31 = 1;
                if ( rtb->WhereEXP )
                {
                    v9 = rtb->WhereEXP;
                    v10 = enarr.TableAddr;
					if ( *((short *)&v10[v9] - 2) )
                    {
                        v11 = rtb->WhereEXP ? &v10[v9 - 1] : 0;
                        v12 = &enarr.TableAddr[v11->enleft - 1];
                    }
                    else
                        v12 = 0;		// error.

					if ( bt->SrchMode )					
                    {											// 3 Operators bits: 
                        if ( bt->SrchMode == '-' )				// 0x08 == less than				'<'
                            v12->TTno = 0x18u;					// 0x10 == equal to					'='
                        else									// 0x20 == greater than				'>'
                            v12->TTno = 0x30u;					// Mixed bit tests:
                    }											// 0x30 == greater than or equal	'>='
                    else										// 0x18 == less than or equal		'<='
                    {											// 0x28 == not equal to				'<>'
                        v12->TTno = 0x10u;
                    }

                    v12->entype = 2;
                    bt->EXPR = getexpr(ENARR(rtb->WhereEXP), TDno);
                    v7 = 1;	// EXPR loaded ok
                }
            }
        }
        if ( !v7 )	// if not, create a default EXPR
            bt->EXPR = bldexpr(TDno, fno, bt->SrchMode);

		v15 = rtdget(&ttab[TDno]);
        ErrorCode = cdbrestrict(v15, bt->EXPR);
        if ( ErrorCode < 0 )
            dberror(ErrorCode, 0, v15);

		if ( !bt->SrchMode )
            getkerr(Src, bt, fld);
    }
//--------------------------------------
    if ( bt->SrchMode == '-' )	// don't check for '+'
        SrchMode = 'p';			// set searchmode to backwards
    else
        SrchMode = 'n';			// default searchmode == forward
//--------------------------------------
    if ( SrchMode != 'n' )
        goto LABEL_35;

LABEL_37:
    for ( RdRtn = rdnxt(TDno, bt->EXPR, 0, 1); RdRtn <= 0;  RdRtn = rdprev(TDno, bt->EXPR, 0, 1) )
    {
        if ( !bt->BTFlags )
        {
            if ( bt->SrchMode )
                prerr("no more", 0, 1);
            else
            {
                if ( bt->On_excep )
                {
                    setclvars(1, 0);            // "ERROR"
                    pexcept(bt);
                }
                else
                    prerr("%snot found", Src, 1);
                v33 = 1;
            }
        }
        SrchMode = 'q';
LABEL_74:
        if ( SrchMode == 'y' )
        {
			//eprint("egetkey 211 SrchMode == %C\n",SrchMode);
			if ( !rcheck(xtb, fld, 0) )
			{
				SrchMode = 'q';
				goto LABEL_84;	// get out of the routine
			}

			if ( bt->Curr_Mode == 'v' )
				ErrorCode = rdrec(TDno, 0);		// no lock required when in VIEW mode
			else
				ErrorCode = rdrec(TDno, 1024);	// lockr
			
			if ( ErrorCode <= 0 )       // rdrec filters -10 error code in View mode?
			{
				if ( bt->On_excep )
				{
					setclvars(0, 1);
					pexcept(bt);
				}
				else
					prerr("'%s' record locked", fldtobuf(Src, fld, 0), 1);
				SrchMode = 'q';
			}
		}

LABEL_84:
        if ( SrchMode == 'n' )
            goto LABEL_37;
LABEL_35:
        if ( !(SrchMode == 'p'))
            goto LABEL_86;				// if we go around here, we do a rdprev();
    }

// fall out of above loop when we found valid data
//eprint("egetkey 243 SrchMode == %C\n",SrchMode);

//-----------------------------
    rectott(TDno, fno);
    for ( j = fld; j->FLDstat < 0; j++ )
        j->FLDstat &= 0xFFEFu;				// clear 0x0010 bit on all Key fields

    if ( !bt->SrchMode )
    {
        dispfld(xtb, fld, 1, 0, 1, 0);
        goto LABEL_50;
    }
    if ( !v31 && !jcheck(ptb, fld, 0, 0) )
        goto LABEL_84;						// go back into main loop looking for more records

    dispfld(xtb, fld, 1, 0, 1, 0);

    v33 = 1;
    ErrorCode = sploop(bt, 0, CurrPTno, bt->Screens->PT_end, CurrPTno, bt->Screens->PT_end);
	//eprint("egetkey 262 ErrorCode = %d\n",ErrorCode);
    if ( ErrorCode == 1 || ErrorCode == 256 )
    {
        v33 = 1;
        setcvar(v_REPLY, "");	//	"REPLY"
LABEL_50:
        if ( bt->SrchMode )
        {
            if ( ErrorCode == 256 )
            {
                linput(&SrchMode, 0, 0);
                if ( SrchMode == 'n' )
					SrchMode = reply("<n>ext <p>rev or <q>uit", "npq");
                else
					SrchMode = reply("<p>rev <n>ext or <q>uit", "pnq");
				ErrorCode = 0;
            }
            else
            {
                if ( SrchMode == 'n' )
					SrchMode = reply("<y>es <n>ext <p>rev or <q>uit", "npyq");
                else
					SrchMode = reply("<y>es <p>rev <n>ext or <q>uit", "pnyq");
            }
        }
        else
            SrchMode = 'y';
        
		if ( SrchMode == 'n' || SrchMode == 'p' )	// next/previous
        {
            rfree(TDno, 0);
            if ( var_cr )
            {
                dscreen(bt, 0, CurrPTno);
                var_cr = false;
            }
            else
                disprec(bt, TDno, 0, 0, CurrPTno);
        }
        goto LABEL_74;	// go back into main loop looking for more records
    }
    bt->BTFlags = false;
    bt->SrchMode = 0;
    SrchMode = 'q';
    v32 = 1;

LABEL_86:
//eprint("egetkey 307 SrchMode == %C\n",SrchMode);

//------------------------------
    if ( SrchMode == 'q' )
    {
        bt->BTFlags = false;
        bt->SrchMode = 0;
    }
    if ( v33 )
    {
        if ( SrchMode != 'y' )
        {
            rfree(TDno, 0);
            if ( var_cr )
            {
                dscreen(bt, 0, 0);
                var_cr = false;
            }
            else
                disprec(bt, TDno, 0, 0, 0);

			if ( xtb->Flags & 0x10 )
            {
				v26 = RTARR(xtb->RangeID);
                rfree(v26->TTno, 0);
            }
            goto LABEL_98;
        }
LABEL_99:
        if ( bt->SrchMode )
            bt->BTFlags = true;
    }
    else
    {
LABEL_98:
        if ( SrchMode == 'y' )
            goto LABEL_99;
    }
//-----------------------------
    if ( !v32 )
    {
        if ( SrchMode == 'y' )
            ErrorCode = 0x40u;
        else
            ErrorCode = 0x80u;
    }
    return ErrorCode;
}

int pedit(XTAB *xtab, PTAB *ptab, FLDdesc *fld, BTAB *btab, int *skipPtr, bool inDoBlock, short CurrPTno, bool noScreenOut)
{
    char	*v13;
	int		fno;
	int		result;
	int		v11;
	int		TDno;
	char	WildChar;
	char	FLDtype;
	
    fno = gettdfno(ENARR(xtab->VarExpNo), &TDno, &FLDtype, 0);
	assert(fno >= 0);

	if ( xtab->Flags >= 0 || !btab->BTFlags )
    {
        result = modfld(btab, ptab, fld, *skipPtr, &v13, noScreenOut);
        if ( result == -17 || result == -5 || result == -22 )// error codes
            return result;
        
		if ( result == -27 )
            *skipPtr = 1;		// Update caller mem directly.  Sets "skip" global variable
        
		if ( xtab->Flags < 0 )	// reachable if !btab->BTFlags
        {
            key_i = CurrPTno;			// ** global var **
            v11 = v13 ? lenstr(v13) : 0;

            if ( !v11 || (WildChar = v13[v11 - 1], btab->SrchMode = WildChar, WildChar != '*') && WildChar != '+' && WildChar != '-' )
                btab->SrchMode = 0;
        }
        if ( !btab->SrchMode && (xtab->Flags & 0x08) && (fld->FLDstat & fld_ZERO) && (null_exit == 1) && !inDoBlock )
        {
            dispfld(xtab, fld, 0, 0, 1, 0);
            return 0;
        }
    }
    if ( xtab->Flags < 0 )	// Key Field   & 0x8000
    {
        if ( btab->Curr_Mode == 'a' )
        {
            if ( xtab->Flags & xtb_LKF)	//	0x0040u	LKF Last Key Field
                return eaddkey(btab, ptab, fld, TDno);
        }
        else
        {
            if ( btab->SrchMode || xtab->Flags & xtb_LKF)		// 0x0040u LKF Last Key Field
                return egetkey(btab, ptab, xtab, fld, TDno, fno, CurrPTno);
        }
    }
    return 1;
}

#endif

