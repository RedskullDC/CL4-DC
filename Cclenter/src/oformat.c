#ifndef OFORMAT_C
#define OFORMAT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include <monetary.h>	// for strfmon()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

char *nfmt(char *a1, const char *format, double a3)
{
	const char *v4;
	char	*i;
	char	*v6;
	size_t	v8;
	int		DecPlaces;
	
    signed int v3;
    int		NumLen = 0;
    bool	isNegative = false; 
    unsigned short mask_flg;			// bitmask which is set from features found in the format Mask
    
	char	NumBuff[112];
    char	v18[128];

	//printf("nfmt(a3 = '%g', Mask = '%s')\n", a3,format);

    if ( strchr(format, '%') )	// apply normal financial style formats
    {
        strfmon(a1, 0x20u, format, a3);
		//printf("a1 = %s\n",a1);
        return a1;
    }

    v3 = 2;
    v4 = format;
    for ( i = v18; *v4; v4++ )
    {
        *i = *v4;
        if ( v3 == 1 )
        {
            v6 = i++;
            *v6 |= 0x80u;
            v3 = 2;
        }
        else if ( *v4 == '\\' )
			v3 = 1;
		else
			i++;
    }
    if ( v3 == 1 )
        i++;
    *i = 0;

//--------------------
    mask_flg = issign(v18);
//printf("nfmt(v18 = '%s', mask_flg = %d [x%04X])\n",v18,mask_flg,mask_flg);
    //v7 = strlen(v18) + 1;
    //v8 = v7 - 1;
    v8 = strlen(v18);
    if ( v8 > 0 )
    {
        NumLen = getdlen(v18, v8, mask_flg);			// v8 ignored by getdlen
        DecPlaces = getdp(v18, mask_flg);
        isNegative = 0.0 > a3;
        if ( DecPlaces && a3 == 0.0)
        {
            fillbuf(NumBuff, NumLen, ' ');
            NumBuff[NumLen] = 0;
        }
        else
        {
            if (isNegative)
				a3 = -a3;
            sprintf(NumBuff, "%*.*f", NumLen, DecPlaces, a3);
			//printf("nfmt(NumBuff = '%s') NumLen = %d, DecPlaces = %d, a3 = %g\n", NumBuff,NumLen,DecPlaces,a3);
        }
    }

    if ( !v8 || NumLen < (unsigned int)lenstr(NumBuff) || isNegative && !(mask_flg & 1) )
    {
        if ( mask_flg & 0x20 )		// "V" virtual decimal point
            --v8;
        else if ( mask_flg & 4 )	// true if trailing CR/DB is in fmt string
			++v8;

		fillbuf(a1, v8, '?');
        a1[v8] = 0;
        return a1;
    }
    return form(a1, NumBuff, v18, mask_flg, isNegative);
}

unsigned short issign(char *a1)	// does the output number need to be signed in anyway
{
    unsigned short MaskBits = 0;
	char *v3;
    char *i;

    switch ( *a1 )				// look at first char in string
    {
        case '\0':
            return 0;			// Null string, no formatting flags required
        case '(':
        case '+':
        case '-':
        case '<':
            MaskBits = 0x01u;			// 0x01 leading sign
            if ( a1[1] == *a1 )
                MaskBits = 0x09u;		// 0x08 indicates multiple leading signs
            break;
        case '#':
        case '&':
        case '*':
        case '.':
        case '9':
        case 'C':
        case 'D':
        case 'V':
        case 'Z':
        case 'c':
        case 'd':
        case 'v':
        case 'z':
            break;

        default:
            if ( a1[1] != *a1 )
                break;
            MaskBits = 0x18u;
            currency_symbol = a1[1];
            v3 = a1;
            if ( !*a1 )
                goto LABEL_17;
            do
            {
                if ( *v3 == currency_symbol )
                    *v3 = '$';
                v3++;
            }
            while ( *v3 );
            break;
    }

    for ( i = a1; *i; i++ )
    {
        if ( *i == 'v' || *i == 'V' )	// 0x20 == virtual decimal point
			MaskBits |= 0x20u;					
        else if ( *i == '$' )			// 0x10 indicates currency found
			MaskBits |= 0x10u;
    }

LABEL_17:
    switch ( a1[lenstr(a1) - 1] )	// examine last char in buffer
    {
        case '+':
        case '-':
            if ( !(MaskBits & 1) )		// if no leading sign, allow trailing sign
                MaskBits |= 0x3u;
            break;
        case 'C':
        case 'D':
        case 'c':
        case 'd':
            MaskBits |= 0x07u;			// accounting CR/DB or cr/db
            break;
    }
    return MaskBits;
}

int getdlen(char *a1, int a2, unsigned short MaskBits)
{
    char	*i;
	int		v3;
	int		result;
	
    v3 = 0;
    for ( i = a1; *i; ++i )
    {
        switch ( *i )
        {
            case '#':
            case '$':
            case '&':
            case '(':
            case '*':
            case '+':
            case '-':
            case '.':
            case '9':
            case '<':
            case 'V':
            case 'Z':
            case 'v':
            case 'z':
                ++v3;
                break;
            default:
                continue;
        }
    }
    if ( (MaskBits & 5) == 1 )
        --v3;
    result = v3 - 1;
    if ( !(MaskBits & 0x10) )
        result = v3;
    return result;
}

int getdp(char *a1, unsigned short Mask)		// find the number of decimal places required for the result string
{
    int result = 0;
    char *v3;
    char *i;

    v3 = a1;
    if ( *a1 )
    {
        if ( *a1 == '.' || *a1 == 'v' || *a1 == 'V' )
        {
LABEL_9:
            if ( *v3 )
            {
                for ( i = v3 + 1; *i; ++i )
                {
                    if ( !(Mask & 2) || *i != '+' && *i != '-' && *i != 'd' && *i != 'D' && *i != 'c' && *i != 'C' )
                    {
                        if ( *i != '>' && *i != ')' && *i != '0' && *i != '/' && *i != ' ' && *i != '_'
                          && (*i == '#' || *i == 'z' || *i == 'Z' || *i == '&' || *i == '9') )
                            result++;
                    }
                }
            }
        }
        else
        {
            while ( 1 )
            {
                ++v3;
                if ( !*v3 )
                    break;
                if ( *v3 == '.' || *v3 == 'v' || *v3 == 'V' )
                    goto LABEL_9;
            }
        }
    }
    return result;
}

char *form(char *OutBuff, char *Src, char *Mask, unsigned short mask_flg, bool Negative)
{
    char *Dest;
	char *i;
	
	bool ShowSign;
	bool DollarShown = false;	// flag to indicate '$' has appeared in output string
    
	char v10;
	
	printf("form(Src = '%s', Mask = %s, mask_flg = x%04X, Negative = %s)\n", Src,Mask,mask_flg,Negative? "true" : "false");
    for ( i = Src; *i == ' '; i++ )
        ;
    if ( *i == '0' )
        *i = ' ';

    ShowSign = (mask_flg >> 3) & 1;	// 0x0008 multiple leading signs flag bit
    Dest = OutBuff;

	//Src = a2;
    //Mask = a3;
    while ( *Mask )
    {
        switch ( *Mask )
        {
            case ',':
                if (isdigit(*(Dest - 1)))
				{
					*Dest++ = *Mask++;
					break;
				}
                if ( *(Dest - 1) == '*' )
                {
                    *Dest = '*';
                }
                else
                {
                    if ( mask_flg & 0x18 )
                    {
                        *Dest = *(Dest - 1);
                        *(Dest - 1) = *(Dest - 2);
                        *(Dest - 2) = ' ';
                    }
                    else
                    {
                        if ( *(Dest - 1) == ' ' )
						{
                            *Dest++ = ' ';
						    Mask++;
							break;
						}
                        *Dest = *(Dest - 1);
                        *(Dest - 1) = ' ';
                    }
                }
                Dest++;
                Mask++;
                break;
            
			case 'V':	// This is an assumed decimal point. An assumed decimal point is never printed, it is used to align the number and the mask.
            case 'v':
                Src++;
                --Dest;	//******
                Dest++;
                Mask++;
                break;
            
			case '.':	// This character is a decimal point. A decimal point is always printed if there is a number to print.
                if ( isdigit(*(Dest - 1)) || Mask[1] == '&' || Mask[1] == '9' )
				{
				    *Dest++ = *Mask++;
					Src++;
				}
                else if ( *Src != ' ' )
				{
	                *Dest++ = *Src++;
					Mask++;
				}
				else
				{
					v10 = *(Dest - 1);
					*Dest = v10;
					
					if ( v10 != '*' )		// v7 == '*' means *(Dest - 1) was NOT a number
						*(Dest - 1) = ' ';

					Src++;
					Dest++;
					Mask++;
				}
				break;

            case '&':	// These are fill characters. Unless this position is occupied by a number it is filled with a zero (0).
            case '9':
                
				if ( *Src != ' ' )
					*Dest++ = *Src;
				else
					*Dest++ = '0';
				Src++;
				Mask++;
				break;

			case '(':
            case '+':
            case '-':
            case '<':
                if ( mask_flg & 8 )	// multiple leading signs flag bit
                {
                    if ( ShowSign )	// Do we need to add sign to dest buffer?
                    {
                        if ( *Mask == '+' && Negative )
						{
							*Dest++ = '-';
							Mask++;
						}
                        else if ( !Negative )
						{
							*Dest++ = ' ';
							Mask++;
						}
						else
						{
							*Dest++ = *Mask++;
						}
						ShowSign = false;	// indicate sign has been added to dest for next loop around
						break;
                    }
                    if ( *Src == ' ' )
                    {
                        v10 = *(Dest - 1);
                        *Dest = v10;
                        *(Dest - 1) = ' ';
                        Src++;
                    }
                    else
                    {
                        if ( mask_flg & 0x10 && !DollarShown )
                        {
                            *Dest = currency_symbol;
                            DollarShown = true;
                        }
                        else
                        {
                            *Dest = *Src++;
                        }
                    }
                }
                else
                {
                    if ( *Mask == '+' && Negative )
                    {
						*Dest++ = '-';
						Mask++;
						break;
                    }
                    else if ( !Negative )
                    {
						*Dest++ = ' ';
						Mask++;
						break;
                    }
                    *Dest = *Mask;
                }
                Dest++;
                Mask++;
                break;
            
			case '#':	// Space fill (or zero suppression) character. Unless this position is occupied by a number it is filled with a space.
            case 'Z':
            case 'z':
				*Dest++ = *Src++;	// Doesn't look like that is what is happening.....??
                Mask++;
                break;

            case '$':
                if ( !DollarShown )
				{
					*Dest++ = currency_symbol;
					DollarShown = true;
					Mask++;
					break;
				}
                if ( *Src != ' ' )
                {
	                *Dest++ = *Src++;
					Mask++;
					break;
				}
                if ( !DollarShown )	// unreachable?
				{
					Src++;
					Dest++;
					Mask++;
					break;
				}

				v10 = *(Dest - 1);
                if ( v10 != currency_symbol )
                {
                    *Dest++ = ' ';
				}
				else
				{
				    *(Dest - 1) = ' ';
				    *Dest++ = v10;
				}
				Src++;
				Mask++;
				break;

            case '*':				// Asterisk fill character. Unless this position is occupied by a number it is filled with an asterisk
                if ( *Src == ' ' )
                    *Dest++ = *Mask;
                else
                    *Dest++ = *Src;
				Src++;
                Mask++;
                break;

			case ')':		// This is a fixed accounting parenthesis that is used in place of a minus sign to indicate a negative number. 
            case '>':		// A single parenthesis generally closes a mask that begins with an opening parenthesis.
				if ( Negative )
					*Dest++ = *Mask;
				else
					*Dest++ = ' ';
				Mask++;
				break;

			case 'D':		// Fixed accounting indicator that is used in place of a trailing sign to indicate a negative number. Negative numbers are followed by 'db'.
            case 'd':
                if ( !Negative )
				{
                    *Dest++ = ' ';
                    *Dest = ' ';
				}
				else
				{
					*Dest++ = *Mask;
					if ( *Mask == 'd' )
					    *Dest = 'b';
					else
					    *Dest = 'B';
				}
				Dest++;
				Mask++;
				break;
            
			case 'C':		// Fixed accounting indicator that is used in place of a trailing sign to indicate a negative number. Negative numbers are followed by 'cr'.
            case 'c':
                if ( !Negative )
                {
                    *Dest++ = ' ';
                    *Dest = ' ';
                }
				else
				{
					*Dest++ = *Mask;
					if ( *Mask == 'c' )
					    *Dest = 'r';
					else
					    *Dest = 'R';
				}
				Dest++;
				Mask++;
				break;
			
            case '_':		// Underscore character (_) is used to indicate a fixed space. A space is always left at this position.
 
				*Dest++ = ' ';
				Mask++;
				break;

			case ' ':
            case '/':
            case '0':
			default:				// default action, just copy character unmodified
                
				*Dest++ = *Mask++;
                break;
        }
    }
    *Dest = 0;
    return OutBuff;
}

#endif
