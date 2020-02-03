#ifndef TOSCREEN_C
#define TOSCREEN_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

int toscreen(short Column, short Line, bool allow_mask, int MaxLen, char *MessageString, short Attr, int FieldType, bool forceUpdate)
{
    char	*AttrBuff;
    char	*CharBuff;
	
    
	int		FieldStart;
	int		ScreenEnd;
	
	short	cy_sav;
	short	cx_sav;
	short	glt_cy;
	short	glt_cx;
	
	bool	a5;
	
	short	ScreenOffset;
	short	a4;
	short	v13;
	short	num_glitch;
	char	v25;
	
    if ( MaxLen )
    {
		whereat(Column, Line, allow_mask, (FieldType == 1 ? MaxLen: 0));	// allow_mask == true means col/line can have relative flags
		coord();	// update _cx, _cy

		ScreenOffset = getosbuf();
        
		CharBuff = &abuf[ScreenOffset];		// current offset in Character Buffer
        AttrBuff = &ccbuf[ScreenOffset];	// current offset in Attribute Buffer

        a5 = 0;
		// Don't update screen if string is already there, and forceUpdate not set, and no attribute change, 
        if ( forceUpdate || !cmpbuf(CharBuff, MessageString, MaxLen) || (a5 = chg_attr(AttrBuff, Attr, FieldType)) != 0 )
        {
            a4 = 0;
            if ( FieldType != 0x40 )
                a4 = _cx;
            
            num_glitch = getnats(Attr, FieldType);
            if ( !num_glitch && a5 )
                num_glitch = _sg;				// Number of "standout glitch" chars to display

			v13 = getattr(Attr, FieldType);
            v25 = v13 ? (v13 | 0x40) : 0;

			if ( num_glitch )
            {
                if ( num_glitch >= a4 )
                {
					memset(CharBuff, 0, num_glitch);
                    CharBuff += num_glitch;
                }
                else
                {
                    AttrBuff -= num_glitch;
                    _cx -= num_glitch;
					memset(&CharBuff[-num_glitch], 0, num_glitch);
                }
            }

            cx_sav = _cx;
            cy_sav = _cy;
            FieldStart = getosbuf();											// current offset (zero based) into screen char buffer

            ScreenEnd = _co * _li;												// check for Screen buffer overflow.
            if ((FieldStart + MaxLen + 2 * num_glitch + 1) >= ScreenEnd )		// if FieldEnd > ScreenEnd
            {
                MaxLen = ScreenEnd - FieldStart - (2 * num_glitch) - 1;			// Adjust Maxlen so field doesn't overrun ScreenEnd
                MessageString[MaxLen] = 0;										// *** modifies caller string buffer directly ***
            }
            
			memset(AttrBuff, v25, MaxLen + num_glitch);							// Fill attribute buffer with required info
            AttrBuff = &AttrBuff[MaxLen] + num_glitch;
            _cx += MaxLen + num_glitch;
            
			coord();			// bounds check on _cx, _cy
            glt_cx = _cx;		// save cx,cy in case of glitch char requirements
            glt_cy = _cy;
            
			cpybuf(CharBuff, MessageString, MaxLen);							// copy message into screen buffer.
            
			if ( num_glitch && (*AttrBuff = v25 | 0x80, memset(&CharBuff[MaxLen], 0, num_glitch), (_cx += num_glitch) != 0) && _YF == 1 )
            {
                qat(glt_cx, glt_cy);
                scat(Attr, false, FieldType, 0, a5);	// attr off
				qat(cx_sav, cy_sav);
                scat(Attr, true, FieldType, a4, a5);	// attr on
                fputs(MessageString, stderr);
                scat(Attr, false, FieldType, 0, a5);	// attr off
                qat(num_glitch + glt_cx, glt_cy);
            }
            else	// no screen glitch fudging was required
            {
                qat(cx_sav, cy_sav);
                scat(Attr, true, FieldType, a4, a5);	// attr on
                fputs(MessageString, stderr);
                scat(Attr, false, FieldType, 0, a5);	// attr off
            }
            
			whereat(num_glitch + glt_cx, glt_cy, 0, 0);
            if ( FieldType != 0x40 )
                fflush(stderr);

            return 1;		// exit_success
        }
        else
        {
            _cx += MaxLen;
            if ( FieldType == 0x20 && *MessageString == 7 )	// Bell char
            {
                fputc(7, stderr);
                fflush(stderr);
            }
            // return 0;
        }
    }
    return 0;	// exit_failure
}

void scat(short Attr, bool on_off, int FieldType, short a4, bool a5)			/// a4 param ignored!
{
    char	*v6;
	char	*v12;
	short	v13;
	char	v14[8];

	//printf("scat(Attr = %02X, %s, ft = %02X)\n",Attr, on_off?"On ":"Off", FieldType);
    switch ( getattr(Attr, FieldType) )	// FieldType == display field type
    {
        case 0x01:
			v6 = on_off ? _fs : _fe;	// feint
            break;
        case 0x02:
			v6 = on_off ? _so : _se;	// standout
            break;
        case 0x04:
			v6 = on_off ? _us : _ue;	// underline
            break;
        case 0x08:
			v6 = on_off ? _bo : _be;	// bold
            break;
        case 0x10:
			v6 = on_off ? _Bs : _Be;	// Blink
            break;
        case 0x20:
			v6 = on_off ? _rs : _re;	// reverse
            break;
        default:
            v6 = 0;
            break;
    }
    if ( (!v6 || !*v6) && a5 )
    {
        v12 = v14;					// fix for terminals with the "standout glitch"
		v13 = _sg;
		while ( v13-- )				
			*v12++ = ' ';
        *v12 = 0;
        v6 = v14;
    }
    if ( v6 )
        fputs(v6, stderr);
}

bool chg_attr(char *a1, short Attr, int FieldType)
{
    short v3;

    v3 = getattr(Attr, FieldType);
    return v3 != (char)(*a1 & 0xBF);
}

#endif

