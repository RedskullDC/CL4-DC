#ifndef CONVERT_STRING_C
#define CONVERT_STRING_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values


char *convstr(char *Src, bool Encode)
{
    char	*Dest;
    
	signed int v4;
    
	int		oct_digits;
	int		oct_num;
    int		v13;
	
//printf("convstr(x%08X, %d) called\n",Src,Encode);
//DumpBlock(Src,strlen(Src));
    if ( !Encode )
    {
		// Encode == 0, decodes "printf" sequences to actual control chars

        Dest = buf_0_0;
        while ( *Src )	
        {
            if ( *Src == '\\' && Src[1] )	// filter control chars
            {
                Src++;				// bump Src past the slash character
                switch ( *Src )
                {
                    case 'e':				// \e	<ESC>
                        *Dest++ = 27;			
                        break;
                    case 't':				// \t	<TAB>
                        *Dest++ = '\t';
                        break;
                    case 'f':				// \f	<FF>
                        *Dest++ = '\f';
                        break;
                    case 'r':				// \r	<CR>
                        *Dest++ = '\r';
                        break;
                    case 'n':				// \n	<NEW LINE>
                        *Dest++ = '\n';
                        break;
                    case 'b':				// \b	<BELL>
                        *Dest++ = ' ';
                        break;
                    case '\\':				// \	SLASH
                        *Dest++ = '\\';
                        break;
                    case '*':					// regex wildcard chars
                    case '?':
                    case '[':
                        *Dest++ = *Src | 0x80;	// set high bit
                        break;
                    default:
                        if ( (unsigned char)(*Src - 48) > 7u )		// 48 == '0'
                        {
                            *Dest++ = '\\';
                            --Src;
                        }
                        else					// look for 3 digit OCTAL number ie: \065
                        {
                            oct_num		= 0;
                            oct_digits	= 0;
                            do
                            {
                                oct_num = *Src + 8 * oct_num - 48;	// 48 == '0'
                                oct_digits++;
                                Src++;
                            }
                            while ( (unsigned char)(*Src - 48) <= 7u && oct_digits <= 2 );		// 48 == '0'
                            
							if ( oct_digits )	// should always be true, since loop executed at least once above?
                                --Src;
                            if ( oct_num )
                                *Dest++ = oct_num;
                        }
                        break;
                }
            }
//---------------------------------------
            else if ( *Src == '^' && Src[1] )					// control character encoding
            {
                Src++;
                if ( (unsigned char)(*Src - 97) <= 0x19u )	// lower case
                    *Src -= 32;
                
				if ( (unsigned char)(*Src - 65) <= 0x19u )
                    *Dest++ = *Src - 64;
				else
				{
					*Dest++ = '^';
					*Dest++ = *Src;		// normal character, just copy
				}
            }
			else 
//---------------------------------
				*Dest++ = *Src;	// normal character, just copy

            Src++;
        }	// end while (*Src)
    }
	else
	{		// Encode == 1, encodes control chars to their "printf" equivalents:
		v13		= 0;
		Dest	= buf_0_0;
		while (*Src)
		{
			v4 = (unsigned char)*Src;
			switch (v4)
			{
			case 27:			// <ESC>
			    *Dest++ = '\\';
			    *Dest = 'e';
				break;
			case '\n':
			    *Dest++ = '\\';
			    *Dest = 'n';
				break;
			case '\f':
			    *Dest++ = '\\';
			    *Dest = 'f';
				break;
			case '\r':
			    *Dest++ = '\\';
			    *Dest = 'r';
				break;
			case '\t':
			    *Dest++ = '\\';
			    *Dest = 't';
				break;
			case '\\':
			    *Dest++ = '\\';
			    *Dest = '\\';
				break;
			case 191:	// '+'
			case 219:	// '|'
			case 170:	// '¬'
				if ( v13 )
					*Dest = *Src;
				else
				{
					*Dest++ = '\\';
					*Dest = *Src & 0x7F;	// ???
				}
				break;
			default:
				if ( *Src < 0 )
					++v13;
				if ( *Src <= 31 )		// control character
				{
					Dest += sprintf(Dest, "^%c", (unsigned char)*Src + 64);
					Src++;
					continue;
				}
			    if ( *Src == 0x7f )		// logic seems crazy
			    {
					if ( *Src > 31 )	// always true
						Dest += sprintf(Dest, "\\%3o", (unsigned char)*Src);
					else
						Dest += sprintf(Dest, "^%c", (unsigned char)*Src + 64);	// unreachable
					Src++;
			        continue;
				}
				*Dest = *Src;	// normal character, just copy over
				break;
			}
			Dest++;
			Src++;
		}
	}
	*Dest = 0;		// terminate string
	//DumpBlock(buf_0_0,strlen(buf_0_0));
	//printf("convstr- returning buf_0_0 = %s\n",buf_0_0); 
	return buf_0_0;
}

#endif
