#ifndef GETEXPRESSION_C
#define GETEXPRESSION_C

#include <stdio.h>
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

short getlit(FLDdesc *fld, ENTAB *entab, char *Dest, short *OpCode)
{
    char	*v5;
    char	*v13;
    char	*v16;
    
	double v21;
    
	int		v6;
    int		v22;
    
	short	FLDlen;
    short	Src;
    
	char	s[1002];
    char	LastChar;

	//printf("getlit(fld = x%08X, entab = x%08X,  *OpCode = x%04X)\n",fld,entab,*OpCode);
    FLDlen = 0;
    if ( fld->FLDtype == 'C' )
    {
        zap(s, 1001u);
        evalstr(ENARR(entab->enright), s);

		if ( *OpCode & 0x0004 )		// 0004 is the 'like' bit. do wildcard search
        {
            for (v5 = s; *v5 ; v5++)
            {
				v6 = (unsigned char)*v5;
                //if ( v6 == '*' || v6 == '?' || v6 == '[')
                if ( v6 == 0x2A || v6 == 0x3F || v6 == 0x5B)		// set high bit on wildcard chars
					*v5 |= 0x80u;

				//else if ( v6 == 170 || v6 == '+' || v6 == '¦' )
				else if ( v6 == 0xAA || v6 == 0xBF || v6 == 0xDB )	// chars above with high bit set
					*v5 &= 0x7Fu;
            }
        }
        else
            s[fld->FLDlen] = 0;
		//printf("getlit(s = %s\n",s);
    }
    else
    {
        if ( entab->entype == 0x02 && entab->TTno & 0x0200 )		// Integer calc flag 0x200
            v21 = (double)evalint(ENARR(entab->enright));
        else
            v21 = clround(evalnum(ENARR(entab->enright), fld->FLDtype), fld->FLDtype);
        FLDlen = fld->FLDlen;
    }
//-----------------------------
    switch ( fld->FLDtype & 0xFFFFBFFF )		// 1111 1111 1011 1111
    {
        case 'B':
        case 'I':
            Src = (short)v21;
            v13 = (char *)&Src;
			break;
        case 'N':
            v22 = (signed int)v21;
            v13 = (char *)&v22;
			break;
        case 'D':
            v22 = 0;
            if ( v21 >= 0.0 )
                v22 = (int)v21;
            v13 = (char *)&v22;
			break;
        case 'T':
            if ( v21 < 0.0 )
                v21 = 0.0;
            v13 = (char *)&v21;
			break;
        case 'C':
            FLDlen = lenstr(s);
            if ( FLDlen )
            {
                v16 = &s[FLDlen - 1];							// look at last char of the string
                LastChar = *v16;	
                if ( LastChar == '*' || LastChar == '+' || LastChar == '-' )	// Is it a wildcard? We might change the operator type!!
                {
                    *v16 = 0;									// get rid of wildcard char
                    --FLDlen;
                    
					if (*OpCode == 0x10)						// *OpCode are CL4 Operators. 0x10 == equal to	'='
					{
						if ( LastChar == '+' )		
							*OpCode = 0x30u;					// 0x30 == greater than or equal	'>='
						else if (LastChar == '-')
							*OpCode = 0x18u;					// 0x18 == less than or equal		'<='
					}											// if wildcard was '*', we just test the truncated length
                }
				else
					++FLDlen;									// ensure we copy the \0 byte
            }
			else
			{
			    if ( *OpCode == 0x10 || *OpCode == 0x28 )		// 0x10 == equal to					'='
			        ++FLDlen;									// 0x28 == not equal to				'<>'
			}
            v13 = s;
			break;
        default:
            v13 = (char *)&v21;
			break;
    }
	xtodom(Dest, FLDlen, fld->TDFtype, v13);
    return FLDlen;
}

EXPR *getexpr(ENTAB *entab, int TDno)
{
	FLDdesc	*fld;
	
	int		TTno;
	int		Operator;
	
	short	FieldNo;
	short	v9;
	short	OpCode;

    char	a3[1002];
	char	FLDtype;


    if ( entab && entab->entype == 2 )
    {
        Operator = entab->TTno & 0x1FF;
        switch ( Operator )
        {

            case 0x01:	// OR
            case 0x02:	// AND
                return newjunc(getexpr(ENARR(entab->enleft), TDno), Operator, getexpr(ENARR(entab->enright), TDno));	// recursive
                break;
						// 3 Operators bits: 
            case 0x08:	// 0x08 == less than				'<'
            case 0x10:	// 0x10 == equal to					'='
            case 0x20:	// 0x20 == greater than				'>'

	// Mixed bit tests:

            case 0x14:	// 0x14 == equal to	and like		'=*'
            case 0x18:	// 0x18 == less than or equal		'<='
            case 0x28:	// 0x28 == not equal to				'<>'
            case 0x30:	// 0x30 == greater than or equal	'>='
                FieldNo = gettdfno(ENARR(entab->enleft), &TTno, &FLDtype, 0);
				assert(FieldNo >= 0);
                
				if ( TDno && TTno != TDno )
                    return 0;
                
				entab->TTno &= 0xFDFFu;		// 1111 1101 1111 1111     clear the integer calc bit 0x0200

                fld = &ttab[TTno].TTfields[FieldNo];
                
				zap(a3, 1000u);
                OpCode = entab->TTno & 0x003C;	// mask all but relative operator bits
                v9 = getlit(fld, entab, a3, &OpCode);

                return v9 ? newexpr(fld->FLDelemID, OpCode, a3, v9) : 0;
                break;
            //default:			all others fall through
                //return 0;
        }
    }
    return 0;
}

#endif
