#ifndef GETATTR_C
#define GETATTR_C

#include "DBdefs.h"
#include "cl4.h"

short getattr(short Attr, int FieldType)
{
    short v2;

    v2 = Attr & 0x13F;
    if ( !v2 )
    {
        switch ( FieldType )
        {
            case 0x01:
                v2 = _Ya;			// Attribute for ACCEPT fields
                break;
            case 0x02:
                v2 = _Ye;			// Attribute for ERROR display fields
                break;
            case 0x04:
                v2 = _Yh;			// Attribute for HEAD statements
                break;
            case 0x08:
                v2 = _Ym;			// Attribute for cl messages and MESSAGE statement
                break;
            case 0x10:
                v2 = _Yp;			// Attribute for PROMPTS on FIELD statements
                break;
            case 0x20:
                v2 = _Yt;			// Attribute for TEXT statements
                break;
            case 0x40:
                v2 = _Yo;			// Attribute for PRINT statements
                break;
            default:
                v2 = 0;
                break;
        }
    }
    if ( v2 == 0x3F )
        v2 = _Yn;					// Attribute of terminal in NORMAL usage
    return v2;
}

short getnats(short Attr, int FieldType)	// get number of 'glitch' chars required
{
    short v2;
	
    v2 = Attr & 0x13F;
    if ( !v2 )
        v2 = getattr(0, FieldType);
    
	if ( v2 == 0x3F )
        v2 = _Yn;
    
	switch ( v2 )					// get number of 'glitch' chars for various field types
    {
        case 0x01:					// ACCEPT
            return _fg;	
        case 0x02:					// ERROR MSG
            return _sg;
        case 0x04:					// HEAD
            return _ug;
        case 0x08:					// MESSAGE
            return _bg;
        case 0x10:					// PROMPT
            return _Bg;
        case 0x20:					// TEXT
            return _rg;
        default:
            return 0;
    }
}

#endif
