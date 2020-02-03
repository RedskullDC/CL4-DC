#ifndef TERM_CAP_C
#define TERM_CAP_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <termios.h>
#include "DBdefs.h"
#include "cl4.h"

/*	Not called in clenter
int screen_change()
{
    int v0; // esi@1
    __int16 v2[2]; // [sp+18h] [bp-10h]@1

    v0 = open64("/dev/tty", 2);
    if ( !ioctl(v0, 0x5413u, v2) )
    {
        _co = v2[1];
        _li = v2[0];
    }
    return close(v0);
}
*/

TCSCRATTR tc_num_attr[] = {

	"DF", &_DF,			// Date Format 0,1,2
	"sg", &_Bg,
	"sg", &_bg,
	"co", &_co,			// 
	"li", &_li,
	"sg", &_fg,
	"sg", &_rg,
	"sg", &_sg,
	"sg", &_ug,
	"Yd", &_Yd,			// Timeout Delay in seconds (default = 0, no timeout) 
		0,0
};

TCBOOLATTR tc_bool_attr[] = {
	
	"YF", &_YF,
		0,0
};

TCSTRATTR tc_str_attr[] = {

	"bc", &_bc,
	"me", &_Be,
	"mb", &_Bs,
	"me", &_be,
	"md", &_bo,
	"cd", &_cd,
	"ce", &_ce,
	"cm", &_cm,
	"me", &_fe,
	"mh", &_fs,
	"nd", &__nd,
	"me", &_re,
	"mr", &_rs,
	"se", &_se,
	"so", &_so,
	"te", &_te,
	"ti", &_ti,
	"ue", &_ue,
	"us", &_us,
	"ve", &_ve,
	"vi", &_vi,
	"vs", &_vs,
	"Yf", &_Yf,				// the fill character - defaults to space  We use "dot" [Yf=.]
							
	"ya", &_ya,				// y[a-x] - control character remapping for cl field entry editing
	"yb", &_yb,				// ya=^A, yb=^B, yc=^C, yd=^D, ye=^E, yf=^F, yh=^H, yi=^I,
	"yc", &_yc,				// yl=^L, yp=^P, yr=^R, yt=^T, yu=^U, yw=^W, yx=^X, yz=^Z,
	"yd", &_yd,
	"ye", &_ye,
	"yf", &_yf,
	"yh", &_yh,
	"yi", &_yi,
	"yl", &_yl,
	"yp", &_yp,
	"yr", &_yr,
	"yt", &_yt,
	"yu", &_yu,
	"yv", &_yv,				// ????
	"yw", &_yw,
	"yx", &_yx,
	"yz", &_yz,

	"YD", &_YD,				// year range for 2 digit entry : default display width [ YD=1990-2050:8 ]
	"YL", &_YL,				// the alternate language for input, T=Thai, C=Chinese, J=Japanese, K=Korean
	"YP", &_YP,				// the default pause prompt  We use  [YP=<CR>_to_continue]
		0,0
};

TCSCRATTR tc_scr_attr[] = {
	"Ya", &_Ya,				// attribute for accept fields
	"Ye", &_Ye,				// attribute for error displays
	"Yh", &_Yh,				// attribute for head statements
	"Ym", &_Ym,				// attribute for cl messages and message statement
	"Yn", &_Yn,				// attribute of terminal in normal usage
	"Yo", &_Yo,				// attribute for print statements	
	"Yp", &_Yp,				// attribute for prompts on field statements
	"Yt", &_Yt,				// attribute for text statements
							// attributes are: bold feint(dim) normal standout underscore (default is n)

	"YA", &_YA,				// auto Accept fields  (y/n)
	"YR", &_YR,				// auto Reply fields   (y/n)
	"YT", &_YT,				// allow tab as enter  (y/n)
	"Y3", &_Y3,				// rawdisplay?         (y/n) [y=cl3 style, n=cl4 style with wildcards]
		0,0
};

void termcap()
{
    TCSCRATTR *i; // esi@1
    TCSTRATTR *j; // esi@5
    TCBOOLATTR *k; // esi@9
    TCSCRATTR *scratt; // esi@19
    char	*v6; // eax@20
    char	*a2; // [sp+1Ch] [bp-805Ch]@5
    char	*v8[16]; // [sp+20h] [bp-8058h]@19
    char	v9[1024]; // [sp+60h] [bp-8018h]@1
	//char TCLArea[1024];	//**** needs to be in global memory area **. ???
    short	v1; // dx@2

//printf("termcap1: _yh = x%08X \n",_yh);
	memset(v9, 0, sizeof(v9));
    gettcent(v9);	// does nothing.
    for ( i = tc_num_attr; i->AttrName; ++i )
    {
        v1 = tgetnum(i->AttrName);
        if ( v1 > 0 )
		{
//printf("termcap: AttrName = %s,  v1 = %d\n",i->AttrName,v1);
            *i->AttrData = v1;
		}
    }
    a2 = TCLArea;				// a2 points at free space to get the result. GNU only!!!
    for ( j = tc_str_attr; j->AttrName; ++j )
    {
        v6 = tgetstr(j->AttrName, &a2);
        if ( v6 )
		{
//printf("termcap: AttrName = %s,  v6 = x%08X, [%s]  \n",j->AttrName,v6, v6);
            *j->AttrStrData = v6;
		}
    }
//	DumpBlock(TCLArea,100);
//printf("termcap3: _yh = x%08X \n",_yh);
    for ( k = tc_bool_attr; k->AttrName; ++k )
    {
        if ( tgetflag(k->AttrName) )
            *k->AttrBool = 1;
    }
    gettccl(&a2);
    if ( !_bc )
        _bc = (char*)&bc;
    if ( !_yh )
        _yh = _bc;
//printf("termcap4: _yh = x%08X \n",_yh);
    if ( !_yl )
        _yl = __nd;	//***

    a2 = (char*)v8;	// doesn't look right
    for ( scratt = tc_scr_attr; scratt->AttrName; ++scratt )
    {
        v6 = tgetstr(scratt->AttrName, &a2);
        if ( v6 )
        {
            switch ( *v6 )
            {
                case 'f':
                    *scratt->AttrData = 0x01u;
                    continue;
                case 's':
                    *scratt->AttrData = 0x02u;
                    break;
                case 'u':
                    *scratt->AttrData = 0x04u;
                    break;
                case 'r':
                    *scratt->AttrData = 0x20u;
                    break;
                case 'B':
                    *scratt->AttrData = 0x10u;
                    break;
                case 'b':
                    *scratt->AttrData = 0x08u;
                    break;
                default:
                    if ( *v6 == 'n' || *v6 != 'y' )
                        *scratt->AttrData = 0x3Fu;
                    else
						*scratt->AttrData = 0x01u;
					continue;
            }
        }
    }
    if ( _YA == 0x3F )
        _YA = 0;
    if ( _YR == 0x3F )
        _YR = 0;
    if ( _YT == 0x3F )
        _YT = 0;
    if ( _Y3 == 0x3F )
        _Y3 = 0;
    abuf = (char *)mmalloc(_li * _co + 10);	 // Number of visible characters on screen + 10 ?
    ccbuf = (char *)mmalloc(_li * _co + 10);
}

short gettcent(char *a1)
{
    char	*TermName;
    short	result;

    TermName = getenv("TERM");
	//printf("gettcent() - TermName = %s\n",TermName);
    if ( !TermName )
        tcerror("$TERM not in your environment\n", 0);
    result = tgetent(a1, TermName);	// does nothing when running terminfo
	//printf("gettcent() - result = %d\n",result);
    if ( result < 0 )
        tcerror("can't open terminfo file\n", 0);
    if ( !result )
        tcerror("%s not in terminfo file\n", TermName);
    return result;
}

void gettccl(char **a2)
{
    char	*TermName;
	char	*InfoName;
	unsigned short v4;
	char	*v5;
	FILE	*stream;
	char	pathname[256];
	char	v9[8216];
	
    TermName = getenv("TERM");
    if ( !TermName )
        tcerror("$TERM not in your environment\n", 0);
    InfoName = getevar("CLINFO");
    if ( InfoName )
    {
        v4 = clgetmode(InfoName) & 0xF000 ;								// v4 has the bits from stat64.st_mode  x8000 = file, x4000 = directory
        if ( v4 == 0x4000 )												// CLINFO points at a directory, look for terminal specific ".cl" file
            cdbcpystr(pathname, InfoName, "/", TermName, ".cl", 0);		
        else if (v4 == 0x8000 )											// CLINFO points at a file, use that explicitly.
            cdbcpystr(pathname, InfoName, 0);
        else
			return;

		stream = fopen64(pathname, "r");
        if ( stream )
        {
            while ( fgets(v9, 8192, stream))
            {
                if ( v9[0] != '#' )
                {
                    v5 = &v9[lenstr(v9) - 1];	// v5 set to end of the string line, then works backwards.
                    if ( v9 <= v5 )
					{
						while ( 1 )
						{	
							if ( *v5 == ',' )
								*v5 = 0;
							else if (v9 == v5 || isspace(*v5))
							{
								getclatt(&v5[(bool)isspace(*v5)], a2);
								*v5 = 0;
							}
							--v5;
							if ( v9 > v5 )
							    break;
						}
					}
                }
            }
            fclose(stream);
            return;
        }
    }
}

void getclatt(char *AttrName, char **a2)
{
    TCSCRATTR	*v4;
    TCSTRATTR	*v5;
    TCBOOLATTR	*v7;
	TCSCRATTR	*v8;
	char		*v2;
	char		*v6;
	
	char		Src[2];
	char		DateStr[32];
	
//printf("getclatt: AttrName = %s\n",AttrName);

    v2 = AttrName + 3;                          // assume 2 chars of AttrName, then '=' or '^', then data
    if ( *AttrName )
    {
        memset(Src, 0, sizeof(Src));
        if ( AttrName[3] == '^' )				// Ctrl char specifier
            Src[0] = AttrName[4] & 0x9F;
        else
            Src[0] = *v2;

		if ( AttrName[2] == '=' )
        {
            v4 = tc_scr_attr;
            if ( v4->AttrName )
            {
                while ( !cmpbuf(v4->AttrName, AttrName, 2) )
                {
                    ++v4;
                    if ( !v4->AttrName )
                        goto LABEL_19;
                }
                switch ( Src[0] )
                {
                    case 's':
                        *v4->AttrData = 0x02u;
                        break;
                    case 'u':
                        *v4->AttrData = 0x04u;
                        break;
                    case 'r':
                        *v4->AttrData = 0x20u;
                        break;
                    case 'B':
                        *v4->AttrData = 0x10u;
                        break;
                    case 'b':
                        *v4->AttrData = 0x08u;
                        break;
                    case 'f':
                    case 'y':
                        *v4->AttrData = 0x01u;
                        break;
                    default:
                        *v4->AttrData = 0x3Fu;
                        break;
                }
            }
            else
            {
LABEL_19:
                v5 = tc_str_attr;
                if ( v5->AttrName )
                {
                    while ( !cmpbuf(v5->AttrName, AttrName, 2) )
                    {
                        ++v5;
                        if ( !v5->AttrName )
                            return;
                    }
                    if ( cmpbuf(AttrName, "YD", 2) )
                    {
                        _YD = mstrcpy(AttrName + 3, 0);
                        sysdate(DateStr);       // Forces a check_YD, and datedone.
                    }
                    else if ( cmpbuf(AttrName, "YP", 2) )
                    {
						v6 = mstrcpy(AttrName + 3, 0);
                        for ( _YP = v6; *v6; ++v6 )
                        {
							if ( *v6 == '_' )
								*v6 = ' ';
						}
					}
                    else if ( cmpbuf(AttrName, "YL", 2) )
					{
						_YL = mstrcpy(AttrName + 3, 0);
					}
                    else
                    {
						cdbcpystr(*v5->AttrStrData, Src, 0);	// String value
                        *a2 += 2;
                    }
                }
            }
        }
        else if ( AttrName[2] == '#' )
        {
			v8 = tc_num_attr;
            if ( v8->AttrName )
            {
				while ( !cmpbuf(v8->AttrName, AttrName, 2) )
                {
					++v8;
                    if ( !v8->AttrName )
						return;
				}
                *v8->AttrData = strtol(v2, 0, 10);	// Numeric value
			}
		}
        else if ( !AttrName[2] )
        {
			v7 = tc_bool_attr;
            if ( v7->AttrName )
            {
				while ( !cmpbuf(v7->AttrName, AttrName, 2) )
                {
					++v7;
                    if ( !v7->AttrName )
						return;
				}
                *v7->AttrBool = 1;	// Boolean value
            }
        }
    }
}

void tcerror(char *format, char	*a2)
{
	// need to typedef this to take and forward var args!!!!
    //va_start(va, format);
    fwrite("termcap: ", 1u, 9u, stderr);
    fprintf(stderr, format,a2);
    exit(1);
}


/*
void whereat(int Column, int Line, int a3, int a4)
{
    short v4; // esi@1
    short v5; // edi@1
    int v6; // eax@1
    int v7; // eax@8

    v4 = Column;
    v5 = Line;
// Check for Line attributes??
    v6 = Line & 0x7E00;	// Mask out Line# bits: 0-01FF  (Max 512 Lines)
    if ( v6 )
    {
        if ( a3 )
        {
            if ( v6 == 0x200 )
                v5 = (Line & 0x1FF) + oy_1;
            else if ( v6 == 0x400 )
				v5 = oy_1 - (Line & 0x1FF);
        }
        else
            v5 = oy_1;
    }
//---------------------------
// look for any column attributes: $,c,
    v7 = Column & 0x7E00; // Mask out Column# bits: 0-01FF  (Max 512 Column)
    if ( v7 )
    {
        if ( a3 )
        {
            if ( v7 == 0x1000 )
                v4 = (Column & 0x1FF) + ox_0;
            else if ( v7 == 0x2000 )
                v4 = ox_0 - (Column & 0x1FF);
        }
        else
            v4 = ox_0;
    }
//------------------------
// Check for range overflow
	for ( ; _co < v4; ++v5 )
        v4 -= _co;
    
	if ( v5 <= 0 || _li < v5 )
        v5 = 1;
    
	if ( v4 <= 0 || _co < v4 )
        v4 = 1;
    
	_cy = v5;
	if ( a4 && a4 < align )
        _cx = align + v4 - a4;
    else
        _cx = v4;
//------------------------
	if ( a3 )
    {
        ox_0 = v4;
        oy_1 = v5;
    }
}
*/

//int outc(int c)
//{
//    return _IO_putc(c, stderr);
//}

/*

void at(void)
{
    tputs(tgoto(_cm, _cx - 1, _cy - 1), 1, outc);
}

void qat(int X, int Y)
{
//	char	*v2;
//	v2 = tgoto(_cm, X - 1, Y - 1);
//	tputs(v2, 1, outc);

    tputs(tgoto(_cm, X - 1, Y - 1), 1, outc);
}

void erase_page(int a1)
{
    short ScreenOffset;

    tputs(_cd, _li, outc);
    if ( a1 )
    {
        ScreenOffset = getosbuf();
        memset(&abuf[ScreenOffset], ' ', (_li * _co) - ScreenOffset);		// character buffer
        memset(&ccbuf[ScreenOffset], 0 , (_li * _co) - ScreenOffset);			// attribute buffer
    }
}
*/

/* not called in clcomp *
void erase_line()
{
    short RemainChars; // eax@1
    short ScreenOffset; // esi@2

    at();
    tputs(_ce, 1, outc);
    RemainChars = _co - _cx + 1;	// characters remaining on the line.
//printf("erase_line : _ce = %s, RemainChars = %d\n",_ce,RemainChars);
    if ( RemainChars > 0 )
    {
        ScreenOffset = getosbuf();
//printf("erase_line : ScreenOffset = %d, abuf = x%08X\n",ScreenOffset,abuf);
        memset(&abuf[ScreenOffset], ' ', RemainChars);			// character buffer
//printf("erase_line : memset returned\n\n\n");
        memset(&ccbuf[ScreenOffset], 0, RemainChars);					// attribute buffer
//printf("erase_line : memset returned\n\n\n");
    }
}
*/

/*

clstandout()
underline()
feint()
reverse()
blink()
bold()

*/

/* not called in clcomp
void clrefresh(void)
{
    char *v1;
	char *v2;
	char *v4;
	signed int a1;
	short v0;
	
    qat(1, 1);
    erase_page(0);
    v4 = ccbuf;

    v0 = _li * _co - 1;		// start at the end of the screen and work backwards 
    if ( v0 > 0 )
    {
        do
        {
            if ( *v4 )
            {
                do
                {
                    v1 = v4;
                    if ( v0 >= 0 )
                    {
                        do
                        {
                            ++v1;
                            --v0;
                        }
                        while ( v0 >= 0 && *v1 == *v4 );
                    }
                    if ( *v1 < 0 )
                    {
                        qosat((signed short)(v1 - ccbuf));
                        qatt(*v1, 0);
                    }
                    a1 = (signed short)(v4 - ccbuf);
                    if ( _YF == 1 && !*v1 )
                    {
                        qosat(&v1[a1] - v4);
                        qatt(*v4, 0);
                    }
                    qosat(a1);
                    qatt(*v4, 1);
                    fwrite(&abuf[a1], 1u, v1 - v4, stderr);
                    if ( *v1 >= 0 )
                    {
                        qatt(*v4, 0);
                    }
                    else
                    {
                        ++v1;
                        --v0;
                    }
                    v4 = v1;
                }
                while ( v0 > 0 && *v1 );
            }
            --v0;
            ++v4;
        }
        while ( v0 > 0 );
    }
    at();
    v2 = &ccbuf[getosbuf()];
    if ( *v2 & 0x40 )
        qatt(*v2, 1);
    fflush(stderr);
}
void qosat(int a1)
{
    qat((short)(a1 % _co + 1), (short)(a1 / _co + 1));
}

void qatt(char a1, int a2)
{
    char **v2; // eax@3

    switch ( a1 & 0x3F )
    {
        case 0x01:
			v2 = a2 ? &_fs : &_fe;		// feint
            break;
        case 0x02:
			v2 = a2 ? &_so : &_se;		// standout
            break;
        case 0x04:
			v2 = a2 ? &_us : &_ue;		// underscore
            break;
        case 0x08:
			v2 = a2 ? &_bo : &_be;		// bold
            break;
        case 0x10:
			v2 = a2 ? &_Bs : &_Be;		// Blink
            break;
        case 0x20:
			v2 = a2 ? &_rs : &_re;		// reverse
            break;
        default:
            return;
    }
	fputs(*v2, stderr);
}
*/



#endif

