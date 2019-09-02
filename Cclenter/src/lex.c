#ifndef LEXX_C
#define LEXX_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int Lex(_IO_FILE *stream)
{
    char	*v6;
    int		NextState;
    int		CharRead;
    char	s2[80];

    NextState = 1;	// state machine default. Look for control chars
    v6 = 0;

    while ( 2 )
    {
        CharRead = Lex_GetChar(stream);
        if ( CharRead == -1 )
            return -1;
        switch ( NextState )
        {
            case 1:
                switch ( CharRead )
                {
                    case '\t':				// ignore, these are treated as white space chars
                    case ' ':
                        continue;
                    case '{':				// Add a new sub-list
                        return 1;
                    case '}':				// end of sub-list
                        return 2;
                    case '<':				// "keypress" Next item will be KeyName/Function name
                        NextState = 8;
                        continue;
                    case '+':
                        v6 = s2;
                        NextState = 7;
                        continue;
                    case '\'':				// string with *single* quotes
                        Lex_KeyType = Lex_GetSChar(stream, &Lex_CommRStart, &Lex_CommREnd);
                        NextState = 2;		// look for a closing single quote
                        continue;
                    case '"':				// string with *double* quotes
                        v6 = Lex_CommString;
		                NextState = 4;		// look for a closing double quote
				        continue;
                    case '#':				// comment line
                        NextState = 10;
                        continue;
                    case '@':
                        NextState = 11;
                        continue;
                    default:
                        if ( isalpha(CharRead))
						{
                            Lex_CommFunctionName[0] = CharRead;
                            v6 = &Lex_CommFunctionName[1];
                            NextState = 3;
                        }
                        else
                        {
                            if ( !(isdigit(CharRead)) )
                            {
                                fprintf(stderr, "file error bad char %c in file, line %d\n", CharRead, LineNumber);
                                exit(1);
                            }
                            s2[0] = CharRead;
                            v6 = &s2[1];
                            NextState = 6;
                        }
                        break;
                    case '\n':
                    case '\r':
	                    ++LineNumber;
		                continue;
                }
                continue;

            case 2:		// look for a closing single quote
                if ( CharRead == '\'' )
                    return 3;
                fprintf(stderr, "file error no closing quote at line %d\n", LineNumber);
                exit(1);
            
			case 3:		// Return a function name? 
                if ( isalnum(CharRead)|| CharRead == '_' )
				{
                    *v6++ = CharRead;
                    continue;
				}
                ungetc(CharRead, stream);
                *v6 = 0;
				//printf("Lex case:3, Lex_CommFunctionName = %s\n",Lex_CommFunctionName);
                return 4;		// Function name found and returned to caller

            case 4:								// string value handler
                if ( CharRead == '"' )			// end of string value.
                {
					*v6 = 0;
					return 5;
				}
                else if ( CharRead == '\\' )	// skip over slash chars
                    NextState = 5;
				else
                    *v6++ = CharRead;			// copy to output stream
                continue;
            case 5:
                *v6++ = CharRead;
                NextState = 4;
                continue;
            case 6:								// look for a numeric value
                if (isdigit(CharRead))
				{
                    *v6++ = CharRead;
	                continue;
				}
                ungetc(CharRead, stream);
                *v6 = 0;
                Lex_CommDigit = strtol(s2, 0, 10);
				//printf("Lex case:6, Lex_CommDigit = x%04X\n",Lex_CommDigit);
                return 6;
            case 7:
                if (isalnum(CharRead) || CharRead == '_' )
				{
                    *v6++ = CharRead;
	                continue;
				}
                ungetc(CharRead, stream);
                *v6 = 0;
                Lex_CommRStart = KeyToNumber(s2);
				//printf("Lex case:7, Lex_CommRStart = x%04X, KeyName = %s\n",Lex_CommRStart,s2);
                if ( !Lex_CommRStart )
                    fprintf(stderr, "no such key name |%s| at line %d\n", s2, LineNumber);
                Lex_CommREnd = 0;
                Lex_KeyType = 1;
                return 3;

            case 8:
                if ( CharRead != '\t' && CharRead != ' ' )				// whitespace nextstate still 8
				{
					if ( CharRead == '\n' )
						LineNumber++;
					else if ( isalnum(CharRead) || CharRead == '_' )
					{
						s2[0] = CharRead;
						v6 = &s2[1];
						NextState = 9;
					}
					else if ( CharRead == '@' )
						NextState = 14;
					else
					{
						ungetc(CharRead, stream);
						Lex_CommRStart = -1;
						return 8;
					}
				}
                continue;
            case 9:					// look for a text keyname
                if ( isalnum(CharRead) || CharRead == '_' )
                {
                    *v6++ = CharRead;
                    continue;
                }
                ungetc(CharRead, stream);
                *v6 = 0;
                Lex_CommRStart = KeyToNumber(s2);	// Convert KeyName to int
                if ( !Lex_CommRStart )
                    fprintf(stderr, "no such key name |%s| line %d\n", s2, LineNumber);
                return 8;
            case 10:								// comment line handler.
                if ( CharRead == '\n' )				// Ignore chars till NewLine found
                    NextState = 1;
                continue;
            case 11:
                if ( CharRead == '\'' )
                {
                    Lex_KeyType = Lex_GetSChar(stream, &Lex_CommRStart, &Lex_CommREnd);
                    Lex_CommDigit = Lex_CommRStart;
                    NextState = 13;
                }
                else
                {
                    s2[0] = CharRead;
                    v6 = &s2[1];
                    NextState = 12;
                }
                continue;
            case 12:
                if ( isalnum(CharRead) || CharRead == '_' )
				{
                    *v6++ = CharRead;
	                continue;
				}
                ungetc(CharRead, stream);
                *v6 = 0;
				//printf("Lex case:12, s2 = %s\n",s2);
                Lex_CommDigit = KeyToNumber(s2);
                if ( !Lex_CommDigit )
                    fprintf(stderr, "no such key name |%s| at line %d\n", s2, LineNumber);
                return 6;
            case 13:
                if ( CharRead == '\'' )
                    return 6;
                fprintf(stderr, "file error no closing quote at line %d\n", LineNumber);
                exit(1);
            case 14:
                if ( CharRead != '\'' )
                    fwrite("expected a ' after an @ in a <\n", 1u, 0x1Fu, stderr);
                Lex_GetSChar(stream, &Lex_CommRStart, &Lex_CommREnd);
                NextState = 15;
                continue;
            case 15:
                if ( CharRead == '\'' )
                    return 8;
                fprintf(stderr, "no closing quote for < @'#' construct at line %d\n", LineNumber);
                exit(1);
            default:
                continue;
        }
    }
}

int Lex_GetSChar(_IO_FILE *fp, int *Rstart, int *Rend)
{
    char	*v12;
	int		NextState;
	int		result;
	int		CharRead;
	int		v14;
	int		v15;
	int		v16;
	char	Buffer[80];
	
    NextState = 1;		// default state
    v12 = 0;
    while ( 1 )
    {
_L185_0:
        while ( 1 )
        {
            CharRead = _IO_getc(fp);
            if ( CharRead != '\t' )
            {
                if ( CharRead != ' ' )
                    break;
            }
            if ( CharRead == '\n' )		// end of line, go around again
                goto LABEL_7;
        }
        if ( CharRead != '\n' )
            break;
LABEL_7:
        ++LineNumber;
    }

    switch ( NextState )
    {
        case 1:		// deal with first character. determine what should follow.
            if ( CharRead == '[' )	// range definition should follow  eg. [A-K]
            {
                if ( Lex_GetSChar(fp, &v15, &v16) != 1 )
                {
                    fprintf(stderr, "Specifics only in range start at line %d", LineNumber);
                    exit(1);
                }
                *Rstart = v15;				// range starting char
                NextState = 8;				// Range handler
            }
			else if (CharRead == '*')		// wildcard?
				return 3;
            else if ( CharRead == '\\' )
				NextState = 7;				// slash char should follow
            else if ( CharRead == '^' )
				NextState = 9;				// control char should follow
			else
			{
				*Rstart = CharRead;
				return 1;
			}
            goto _L185_0;

        case 8:
            if ( CharRead != '-' )
            {
                fprintf(stderr, "'-' missing line %d\n", LineNumber);
                exit(1);
            }
            if ( Lex_GetSChar(fp, &v14, &v16) != 1 )
            {
                fprintf(stderr, "Specifics only in range end line %d", LineNumber);
                exit(1);
            }
            *Rend = v14;
            NextState = 3;						
            goto _L185_0;

        case 3:									// look for end of range specifier
            if ( CharRead != ']' )
            {
                fprintf(stderr, "missing closing ] line %d\n", LineNumber);	// error_exit
                exit(1);
            }
            return 2;							// range read ok

        case 4:									// deal with HEX value digits
            if (isxdigit(CharRead))				// 0-9,A-F
			{
		        *v12++ = CharRead;
	            goto _L185_0;
			}
            ungetc(CharRead, fp);				// hit a non hex-digit character, back up one
            *v12 = 0;
            sscanf(Buffer, "%x", &CharRead);	// try and interpret as hex number
			*Rstart = CharRead;
			return 1;

        case 5:									// deal with octal digits
            if (isdigit(CharRead))				// 0-9
            {
				*v12++ = CharRead;
				goto _L185_0;
            }
			ungetc(CharRead, fp);				// hit a non-digit character, back up one
			*v12 = 0;
			sscanf(Buffer, "%o", &CharRead);	// try and interpret as octal number
			*Rstart = CharRead;
			return 1;
        
		case 9:									// deal with control chars (0-31)
            if ( CharRead == '^' )
                *Rstart = '^';
			else
				*Rstart = CharRead & 0x1F;		// mask 0-31
			return 1;

		case 7:									// deal with slash chars
            switch ( CharRead )
            {
                case 'n':
                    *Rstart = '\n';
                    return 1;
                case 't':
                    *Rstart = '\t';
                    return 1;
                case 'b':
                    *Rstart = '\b';
                    return 1;
                case 'E':
                case 'e':
                    *Rstart = '\x1B';			// <ESC>
                    return 1;
                case 'f':
                    *Rstart = '\f';
                    return 1;
                case 'r':
                    *Rstart = '\r';
                    return 1;
                case 's':
                    *Rstart = ' ';				// <SPC>
                    return 1;
                case '\\':
                    *Rstart = '\\';
					return 1;
                case '\'':
                    *Rstart = '\'';
                    return 1;
                case 'x':						// hex value follows
                    NextState = 4;
				    v12 = Buffer;
		            goto _L185_0;
                default:						// octal value follows
                    if (!isdigit(CharRead))
					{
						*Rstart = CharRead;
						return 1;
					}
                    Buffer[0] = CharRead;		// assume that following values are octal digits
                    v12 = &Buffer[1];
                    NextState = 5;
                    break;
            }
            goto _L185_0;
        default:
            goto _L185_0;
    }
}

int Lex_GetChar(_IO_FILE *stream)
{
    char	*v5;
	int		CharRead;
	int		NextState;
	char	Buffer[80];
	
    NextState = 1;							// default state
    v5 = 0;
    while ( 2 )
    {
        CharRead = _IO_getc(stream);
        if ( CharRead == -1 )
            return -1;
        switch ( NextState )
        {
            case 1:
                if ( CharRead == '\\' )		// esc char
                    NextState = 3;
                else if ( CharRead == '^' )	// ctrl char
                    NextState = 2;
				else
					return CharRead;
                continue;
            case 2:							// handle <ctrl> char
                return CharRead & 0x1F;	
            case 3:
                switch ( CharRead )
                {
                    case 'n':
                        return '\n';
                    case 't':
                        return '\t';
                    case 'b':
                        return '\b';
                    case 'r':
                        return '\r';
                    case 's':
                        return ' ';
                    case '\\':
                        return '\\';
                    case '\'':
                        return '\'';
                    case 'f':
                        return '\f';
                    case 'E':
                    case 'e':
                        return '\x1B';
                    case 'x':
                        NextState = 4;				// look for hex digits
                        v5 = Buffer;
                        continue;
                    default:
                        if (!isdigit(CharRead))		// isdigit() == 0x0800
                            return CharRead;
                        Buffer[0] = CharRead;
                        v5 = &Buffer[1];
                        NextState = 5;
                        break;
                }
                continue;
            case 4:									// hex digit handler
                if (isxdigit(CharRead))
                {
                    *v5++ = CharRead;
                    continue;						// NextState still 4
                }
                ungetc(CharRead, stream);
                *v5 = 0;
                sscanf(Buffer, "%x", &CharRead);	// scan buffer to int as hex number
                return CharRead;
            case 5:									// *octal* digit handler
			    if (isdigit(CharRead))
                {
                    *v5++ = CharRead;
                    continue;						// NextState still 5
                }
                ungetc(CharRead, stream);
                *v5 = 0;
                sscanf(Buffer, "%o", &CharRead);	// scan buffer to int as octal number
                return CharRead;
            default:
                continue;
        }
    }
}

#endif
