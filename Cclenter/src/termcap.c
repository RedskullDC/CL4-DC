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
	"co", &_co,			// columns
	"li", &_li,			// lines
	"sg", &_fg,
	"sg", &_rg,
	"sg", &_sg,			// magic_cookie_glitch		number of blank characters left by smso or rmso
	"sg", &_ug,
	"Yd", &_Yd,			// Timeout Delay in seconds (default = 0, no timeout) 
		0,0
};

TCBOOLATTR tc_bool_attr[] = {
	
	"YF", &_YF,				// cpi_changes_res			pitch changes resolution ??
		0,0
};

TCSTRATTR tc_str_attr[] = {

	"bc", &_bc,				// cursor_left				Very obsolete name for the "le" capability
	"me", &_Be,				// exit_attribute_mode		turn off all attributes			****
	"mb", &_Bs,				// enter_blink_mode			turn on blinking
	"me", &_be,				// exit_attribute_mode		turn off all attributes			****
	"md", &_bo,				// enter_bold_mode			turn on bold (extra bright) mode
	"cd", &_cd,				// clr_eos					clear to end of screen 
	"ce", &_ce,				// clr_eol					clear to end of line
	"cm", &_cm,				// cursor_address			move to row #1 columns #2
	"me", &_fe,				// exit_attribute_mode		turn off all attributes			****
	"mh", &_fs,				// enter_dim_mode			turn on half-bright mode
	"nd", &__nd,			// cursor_right				non-destructive space (move right one space)
	"me", &_re,				// exit_attribute_mode		turn off all attributes			****
	"mr", &_rs,				// enter_reverse_mode		turn on reverse video mode
	"se", &_se,				// exit_standout_mode		exit standout mode
	"so", &_so,				// enter_standout_mode		begin standout mode
	"te", &_te,				// exit_ca_mode				strings to end programs using cup
	"ti", &_ti,				// enter_ca_mode			string to start programs using cup
	"ue", &_ue,				// exit_underline_mode		exit underline mode
	"us", &_us,				// enter_underline_mode		begin underline mode
	"ve", &_ve,				// cursor_normal			make cursor appear normal (undo civis/cvvis)
	"vi", &_vi,				// cursor_invisible			make cursor invisible
	"vs", &_vs,				// cursor_visible			make cursor very visible
	"Yf", &_Yf,				// the fill character		defaults to space  We use "dot" [Yf=.]
							
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
    char *v3; // edx@6
    char *v6; // eax@20
    char *a2; // [sp+1Ch] [bp-805Ch]@5
    char *v8[16]; // [sp+20h] [bp-8058h]@19
    char v9[1024]; // [sp+60h] [bp-8018h]@1
	//char TCLArea[1024];	//**** needs to be in global memory area **. ???
    short v1; // dx@2

//printf("termcap1: _yh = x%08X \n",_yh);
	zap(v9,1024);
    gettcent(v9);	// does nothing?
    for ( i = tc_num_attr; i->AttrName; ++i )
    {
        v1 = tgetnum(i->AttrName);
        if ( v1 > 0 )
		{
//printf("termcap: AttrName = %s,  v1 = %d\n",i->AttrName,v1);
            *i->AttrData = v1;
		}
    }
	//screen_change();
    a2 = TCLArea;				// a2 points at free space to get the result. GNU only!!!
    for ( j = tc_str_attr; j->AttrName; ++j )
    {
        v3 = tgetstr(j->AttrName, &a2);
        if ( v3 )
		{
//printf("termcap: AttrName = %s,  v3 = x%08X, [%s]  \n",j->AttrName,v3, v3);
            *j->AttrStrData = v3;
		}
    }
//	DumpBlock(TCLArea,100);
//printf("termcap3: _yh = x%08X \n",_yh);
    for ( k = tc_bool_attr; k->AttrName; ++k )
    {
        if ( tgetflag(k->AttrName) )
            *k->AttrBool = 1;
    }

    gettccl(&a2);				// get entries from the CLINFO file (can be term specific)
    
	if ( !_bc )					// ensure sane values for backspace char
        _bc = (char*)&bc;
    if ( !_yh )
        _yh = _bc;
    if ( !_yl )
        _yl = __nd;				// cursor right

//=========================================================================
    
	// get attributes for various screen fields
	a2 = (char*)v8;	// doesn't look right
    for ( scratt = tc_scr_attr; scratt->AttrName; scratt++ )
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
    char *TermName; // eax@1
    short result; // eax@3

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
                    while ( v9 <= v5 )
					{
						if ( *v5 == ',' )
							*v5 = 0;
						else if (v9 == v5 || isspace(*v5))
						{
							getclatt(&v5[(bool)isspace(*v5)], a2);
							*v5 = 0;
						}
						--v5;
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
    TCSCRATTR	*tc_scr;
	TCSTRATTR	*tc_str;
	TCBOOLATTR	*tc_bool;
	TCSCRATTR	*v8;
	char		*v2;
	char		*v6;
	
    char		v3;
	char		Src[2];
	char		DateStr[32];
	
//printf("getclatt: AttrName = %s\n",AttrName);

    v2 = AttrName + 3;                          // assume 2 chars of AttrName, then '=' or '^', then data
    if ( *AttrName )
    {
        zap(Src, 2u);
        if ( AttrName[3] == '^' )	// Ctrl char specifier
            Src[0] = AttrName[4] & 0x9F;
        else
            Src[0] = *v2;

		if ( AttrName[2] == '=' )
        {
            tc_scr = tc_scr_attr;		// Numeric values (flag bits)
            if ( tc_scr->AttrName )
            {
                while ( !cmpbuf(tc_scr->AttrName, AttrName, 2) )
                {
                    ++tc_scr;
                    if ( !tc_scr->AttrName )
                        goto LABEL_19;
                }
                switch ( Src[0] )
                {
                    case 's':
                        *tc_scr->AttrData = 0x02u;		// standout
                        break;
                    case 'u':
                        *tc_scr->AttrData = 0x04u;		// underline
                        break;
                    case 'r':
                        *tc_scr->AttrData = 0x20u;
                        break;
                    case 'B':
                        *tc_scr->AttrData = 0x10u;
                        break;
                    case 'b':
                        *tc_scr->AttrData = 0x08u;		
                        break;
                    case 'f':
                    case 'y':
                        *tc_scr->AttrData = 0x01u;		// feint
                        break;
                    default:
                        *tc_scr->AttrData = 0x3Fu;
                        break;
                }
            }
            else
            {
LABEL_19:
                tc_str = tc_str_attr;		// this allows the CLINFO file to overide entries from the termcap/terminfo files!!! 
                if ( tc_str->AttrName )
                {
                    while ( !cmpbuf(tc_str->AttrName, AttrName, 2) )
                    {
                        ++tc_str;
                        if ( !tc_str->AttrName )
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
						cdbcpystr(*tc_str->AttrStrData, Src, 0);	// String value
                        *a2 += 2;
                    }
                }
            }
        }
        else if ( AttrName[2] == '#' )				// Numeric entry
        {
			v8 = tc_num_attr;
            while ( v8->AttrName )
            {
				if ( cmpbuf(v8->AttrName, AttrName, 2) )
                {
					*v8->AttrData = strtol(v2, 0, 10);	// Assume long integer values
					//printf("getclatt #: AttrName = %s, v8->AttrName = %s, *v8->AttrData = %d\n",AttrName, v8->AttrName, *v8->AttrData);
					return;
				}
				v8++;
			}
		}
        else if ( !AttrName[2] )
        {
			tc_bool = tc_bool_attr;
            while ( tc_bool->AttrName )
            {
				if ( cmpbuf(tc_bool->AttrName, AttrName, 2) )
                {
					*tc_bool->AttrBool = 1;	// Boolean value
						return;
				}
				tc_bool++;
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

void whereat(short Column, short Line, bool allow_mask, int align_chars)
{
	// updates global _cx, _cy variables

    short v4;
	short v5;
	unsigned short Mask;
	
	// default to using the values of line,column passed in
    v4 = Column;
    v5 = Line;

	// Check for Line attribute mask bits
    Mask = Line & 0x7E00;	// Mask out Line# bits: 0-01FF  (Max 512 Lines)
    if ( Mask )
    {
        if ( allow_mask )
        {
            if ( Mask == 0x200 )
                v5 = oy_1 + (Line & 0x1FF);
            else if ( Mask == 0x400 )
				v5 = oy_1 - (Line & 0x1FF);
        }
        else
            v5 = oy_1;
    }

	// look for any column attribute mask bits: $,c,
    Mask = Column & 0x7E00; // Mask out Column# bits: 0-01FF  (Max 512 Column)
    if ( Mask )
    {
        if ( allow_mask )
        {
            if ( Mask == 0x1000 )
                v4 = ox_0 + (Column & 0x1FF);
            else if ( Mask == 0x2000 )
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
        v5 = 1;					// overflow!!, goto Line #1
	if ( v4 <= 0 || _co < v4 )
        v4 = 1;					// overflow!!, goto Column #1
    
	_cy = v5;

// check for field alignment
	if ( align_chars && align_chars < align )	// *** global var ***
        _cx = align + v4 - align_chars;
    else
        _cx = v4;

//------------------------
	if ( allow_mask )	// update the running values if we allowed mask to modify them
    {
        ox_0 = v4;
        oy_1 = v5;
    }
}

int outc(int c)
{
    return _IO_putc(c, stderr);
}

void at(void)
{
    tputs(tgoto(_cm, _cx - 1, _cy - 1), 1, outc);						// _cm == cursor address command string
}

void qat(short X, short Y)
{
    tputs(tgoto(_cm, X - 1, Y - 1), 1, outc);
}

void erase_page(bool clear_internal)
{
    short ScreenOffset;

	tputs(_cd, _li, outc);												// _cd == clear to end of screen string
    if ( clear_internal )												// also wipe internal buffers
    {
        ScreenOffset = getosbuf();										// get current offset into screen buffer
        fillbuf(&abuf[ScreenOffset], (_li * _co) - ScreenOffset, ' ');	// character buffer
        zap(&ccbuf[ScreenOffset], (_li * _co) - ScreenOffset);			// attribute buffer
    }
}

void erase_line()
{
    short RemainChars;
    short ScreenOffset;

    at();
    tputs(_ce, 1, outc);										// _ce == clear to end of line string
    RemainChars = _co - _cx + 1;								// characters remaining on the line.
    if ( RemainChars > 0 )
    {
        ScreenOffset = getosbuf();								// get current offset into screen buffer
        fillbuf(&abuf[ScreenOffset], RemainChars, ' ');			// character buffer
        zap(&ccbuf[ScreenOffset], RemainChars);					// attribute buffer
    }
}

/*

clstandout()
underline()
feint()
reverse()
blink()
bold()

*/

void clrefresh(void)
{
    char	*v1;
	char	*Attrpt;		// pointer to Attribute buffer ccbuf[]
	short	ScreenSize;		// Number of chars on screen (zero based)
	short	AttOff;			// current offset into Attribute buffer

	
    qat(1, 1);				// position to top left corner
    erase_page(false);		// wipe displayed screen, but not internal buffers :o)
    Attrpt = ccbuf;

    ScreenSize = _li * _co - 1;
    while ( ScreenSize > 0 )
    {
		if ( *Attrpt )		// look for a screen attribute specifier at current offset
		{
			do
			{
				v1 = Attrpt;
				if ( ScreenSize >= 0 )
				{
					do
					{
						++v1;
						--ScreenSize;
					}
					while ( ScreenSize >= 0 && *v1 == *Attrpt );	// look for consecutive identical attribute bytes
				}
				if ( *v1 < 0 )	// high bit set on Attribute byte specifier?
				{
					qosat(v1 - ccbuf);
					qatt(*v1, 0);
				}
				
				AttOff = Attrpt - ccbuf;
				if ( _YF == 1 && !*v1 )								// _YF == pitch changes resolution??
				{
					qosat(&v1[AttOff] - Attrpt);
					qatt(*Attrpt, 0);
				}
				qosat(AttOff);
				qatt(*Attrpt, 1);
				
				fwrite(&abuf[AttOff], 1u, v1 - Attrpt, stderr);
				if ( *v1 >= 0 )
				{
					qatt(*Attrpt, 0);
				}
				else
				{
					++v1;
					--ScreenSize;
				}
				Attrpt = v1;					// advance Attrpt to where we are now
			}
			while ( ScreenSize > 0 && *v1 );
		}
		ScreenSize--;
		Attrpt++;
    }
    
	at();
    v1 = &ccbuf[getosbuf()];	// pointer to current offset in attribute buffer
    if ( *v1 & 0x40 )
        qatt(*v1, 1);
    fflush(stderr);
}

void qosat(short Offset)
{
    qat(Offset % _co + 1, Offset / _co + 1);
}

void qatt(char a1, int a2)
{
    char **v2;

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



#endif

