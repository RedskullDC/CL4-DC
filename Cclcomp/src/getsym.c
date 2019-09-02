#ifndef GETSYM_C
#define GETSYM_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

int getch(void)
{
	int		NextChar;
	int		OctalValue;
	int		OctDigitsFound;
	
	esc_char = false;
	
	if ((NextChar = _IO_getc(MainFile)) == -1)	// EOF
		return -1;
	
	if (NextChar != '\\')			// Normal next char exit
		return NextChar;

	if ((NextChar = _IO_getc(MainFile)) == -1)		// NextChar = char following the slash
		return -1;       
	
	esc_char = true;				// nasty globalness!
	
	switch (NextChar)
	{
	case 'f':
		return '\f';				// Formfeed
	case 'r':
		return '\r';				// <CR>
	case 't':
		return '\t';				// TAB
	case 'n':
		return '\n';				// New Line
	case 'b':
		return 8;					// back space
	case 'e':
		return '\x1B';              // ESC

	case '?':						// Wildcard regex chars
	case '[':
	case '*':
		return NextChar | 0x80;

	case '\n':						// real newline in the stream
		++ll;
		++lla;
		do
		{
			NextChar = _IO_getc(MainFile);
		}
		while ( NextChar == ' ' || NextChar == '\t' );	// <SPC> and <TAB> are whitespace chars
		
		if ( NextChar == '\n' )
		{
			++ll;
			++lla;
		}
		return NextChar;			// if EOF was reached, NextChar will be -1 anyway

	default:						// default = Octal number handler
		OctalValue		= 0;
		OctDigitsFound	= 0;

		while ((unsigned int)(NextChar - '0') < 8 && OctDigitsFound < 3) // 3 or less octal digits
		{
			OctalValue = NextChar - '0' + 8 * OctalValue ;	// octal arithmetic
			if ((NextChar = _IO_getc(MainFile)) == -1)
				return -1;
			OctDigitsFound++;
		}
		if ( OctDigitsFound )
		{
			ungetc(NextChar, MainFile);	// put last non-octal char back in the read buffer
			return OctalValue;
		}
		else
			return NextChar;	// just return last char, since it wasn't an octal digit
	}
}

short getsym(void)
{
     const char *v3;
	 char		*v10;
	 long double v4;
	 int	NextChar;
	 char	QuoteChar;
	 bool	hexval;
	 
     //inLit = 0;
     NextChar = c;

     if ( !symbol )
     {
          fwrite("unexpected end of file\n", 1u, 0x17u, stderr);
          fflush(stderr);
          exit(1);
     }
     if ( symbol == 930 )                       // NewLine
     {
          ++ll;
          ++lla;
          if ( codeb )
          {
               fprintf(stderr, "\r%d", (unsigned short)ll);	// last line
               fflush(stderr);
          }
     }
     v10 = sym;

     if ( !NextChar )
     {
          if ((NextChar = getnch()) == -1)
               return 0;
     }

//-----------------------------
//	ignore white space and comment lines till next alpha character
	while (isspace(NextChar) && NextChar != '\n' || NextChar == '#' && !esc_char )
    {
		if ( NextChar == '#' )	// we found a comment line/ ignore everything till after next <CR>
        {
            while ( NextChar != '\n' )
            {
                if ((NextChar = _IO_getc(MainFile)) == -1 )
				{
	                c = -1;
			        return -1;
				}
                if ( NextChar == '\\' )      // Line continue character. Bump line counters
                {
                    if ((NextChar = _IO_getc(MainFile)) == '\n')	// <CR> must follow '\'  *NO SPACES*
                    {
						++ll;
                        ++lla;
                        NextChar = _IO_getc(MainFile);
                    }
				}
                if ( NextChar == -1 )
				{
					c = -1;
				    return -1;
				}
			}
 		}
        else
        {
			if ((NextChar = getnch()) == -1)
				return 0;
        }
	}
//--------------------
	// Look for a reserved word, variable/table name. or possibly a program LABEL
	if (isalpha(NextChar) || NextChar == '_' )
    {
		while ( 1 )
        {
			*v10++ = NextChar;	// copy source buffer into sym[]
            if ((NextChar = getnch()) == -1)
				return 0;

			// check if next digit is not a valid char in a variable/table name 
			if ( !(isalpha(NextChar)) && !(isdigit(NextChar)) && NextChar != '_' )
            {
                if ( NextChar == ':' )      // Program LABEL:   (trumps reserved name!)
                {
					*v10++	= ':';
                    *v10	= 0;
                    symbol	= 720;			// we found a program label  "LABEL:"
                    syml	= strlen(sym);
                    if ((NextChar = getnch()) == -1)
						return 0;
				}
				else
				{
					*v10	= 0;			// terminate the sym string
					symbol	= loadsym(sym);	// look in reserved words table
					if ( symbol < 0 ) 
						symbol = 2230;		// -1 == not in reserved words. 2230 = variable/table name
					syml = strlen(sym);
				}
				c = NextChar;
				return symbol;
			}
		}
	}
	//  look for a literal numeric value, may start with a '-'
	if (isdigit(NextChar) || NextChar == '-' )
    {
		hexval = false;		// DC
		symbol = 0;
        while ( 1 )
        {
			if ( NextChar == '.' )
				symbol = 2210;			// floating point literal	eg:	456.4323
			*v10++ = NextChar;
			if ((NextChar = getnch()) == -1)	// EOF
				return 0;
			// **** routine doesn't check for multiple dec points ****. 34.5.789 is valid :o(
			if (isdigit(NextChar) || (hexval && isxdigit(NextChar)))
				continue;

			if ( NextChar != '.' )		// No decimal point, must be an INT value
			{
				if (NextChar == 'x')	// 0x...  == HEX value		*** DC extension
				{
					hexval = true;
					v10--;
					if ((NextChar = getnch()) == -1)	// EOF
						return 0;		// remove leading '0x' from output string
					continue;
				}

				*v10 = 0;				// we hit a character which wasn't a number or decimal place. terminate the string
				if ( symbol )			// true if already set to 2210
				{
					syml = strlen(sym);
					c = NextChar;
					return symbol;
				}
				if ( sym[0] != '-' || sym[1] ) // can only be true if a digit immediately followed a '-'
				{
					if ( strlen(sym) <= 4 && hexval )
						symbol = 2240;		// hex value
					else if ( strlen(sym) <= 9 || (v4 = strtod(sym, 0), v4 <= 2147483647.0) && v4 >= -2147483647.0 )
						symbol = 2200;		// Integer literal, and within range limits
					else
						symbol = 2210;		// floating point literal
					//printf("returning symbol = %d, [%s]\n",symbol,sym);
				}
				// at this point we have a '-' 
				else if ( NextChar == '=' )
				{
					*v10++ = '=';
					*v10 = 0;
					if ((NextChar = getnch()) == -1)
						return 0;
					symbol = 1560;		// "-="
				}
				else
					symbol = 1580;		// "-"

				syml = strlen(sym);
				c = NextChar;
				return symbol;
			}
		}
	}
//---------
	if ( NextChar == '\'' || NextChar == '"' )		// looking for a string literal here
	{
		//inLit = 1;
		QuoteChar = NextChar;						// Save the quote char we found to look for a match
		if ((NextChar = getnch()) == -1)			// EOF
			return 0;
		while ( NextChar != QuoteChar || esc_char )
		{
			if ( NextChar == '\n' && !esc_char )	// this allows a <CR> to end a string without a matching quote
				break;
			*v10++ = NextChar;
			if ((NextChar = getnch()) == -1)
				return 0;
		}
		if ( NextChar != QuoteChar || (NextChar = getnch(), NextChar != -1) )
		{
			*v10	= 0;				// terminate string
			symbol	= 2220;				// string literal
            syml	= strlen(sym);
            c		= NextChar;
            return	symbol;
		}
		return 0;
	}
	else	// everything else!
	{
		v10 = sym;
		switch (NextChar)
		{
		case '*':
			*v10++ = '*';
            if ((NextChar = getnch()) == -1)
				return 0;
			if ( NextChar == '*' )
            {
				*v10++ = '*';
                symbol = 1170;              // "**"
                if ((NextChar = getnch()) == -1)
					return 0;
				if ( NextChar == '=' )
				{
					*v10++ = '=';
                    symbol = 1160;         // "**="
                    break;	//goto LABEL_85;
				}
                *v10 = 0;
                syml = strlen(sym);
                c = NextChar;
                return symbol;
			}
            if ( NextChar != '=' )
            {
				symbol	= 890;               // "*"
                *v10	= 0;
                syml	= strlen(sym);
                c		= NextChar;
                return symbol;
			}
            *v10++ = '=';
            symbol = 880;                    // "*="
			break;	//goto LABEL_85;

		case '<':
			*v10++ = '<';
            if ((NextChar = getnch()) == -1)
				return 0;
			if ( (unsigned int)(NextChar - 61) > 1 )
				symbol = 1840;    // "<"
			else
            {
				*v10++ = NextChar;
                symbol = 1830;        // "<=", "le"
				if ( NextChar != '=' )
					symbol = 1850;   // "<>", "ne"
                if ((NextChar = getnch()) == -1)
					return 0;
			}
            *v10 = 0;
            syml = strlen(sym);
            c = NextChar;
            return symbol;
			break;

		case '&':
			*v10++ = '&';
            NextChar = getnch();
            if ( NextChar == -1 || NextChar == '&' && (*v10++ = '&', NextChar = getnch(), NextChar == -1) )
				return 0;
			*v10	= 0;
            symbol	= 80;					// "&" , "&&", "and"
			syml	= strlen(sym);
			c		= NextChar;
			return symbol;
			break;

		case ('|'):
			*v10++ = '|';
            NextChar = getnch();
            if ( NextChar == -1 || NextChar == ('|') && (*v10++ = '|', NextChar = getnch(), NextChar == -1) )
				return 0;
			*v10	= 0;
            symbol	= 1040;					// "|", "||", "or"		"or" is caught in first block by loadsym
			syml	= strlen(sym);
			c		= NextChar;
			return symbol;
			break;

		case '+':
			*v10++ = '+';
            if ((NextChar = getnch()) == -1)
				return 0;
			if ( NextChar == '=' )
            {
				*v10++ = '=';
                if ((NextChar = getnch()) == -1)
					return 0;
				symbol = 40; // "+="
			}
            else
				symbol = 50; // "+"
			*v10 = 0;
            syml = strlen(sym);
            c = NextChar;
            return symbol;
			break;

		case '/':
			*v10++ = '/';
            if ((NextChar = getnch()) == -1)
				return 0;
			if ( NextChar == '=' )
            {
				*v10++ = '=';
                if ((NextChar = getnch()) == -1)
					return 0;
				symbol = 320;	// "/="
			}
            else
				symbol = 330;	// "/"
			*v10 = 0;
            syml = strlen(sym);
            c = NextChar;
            return symbol;
			break;
		
		case '%':
			*v10++ = '%';
            if ((NextChar = getnch()) == -1)
				return 0;
            if ( NextChar == '=' )
            {
				*v10++ = '=';
                if ((NextChar = getnch()) == -1)
					return 0;
				symbol = 850;// %="
			}
            else
                symbol = 860;// "%"
			*v10 = 0;
            syml = strlen(sym);
            c = NextChar;
            return symbol;
			break;

		case '>':
			*v10++ = '>';
            if ((NextChar = getnch()) == -1) return 0;
			
			if ( NextChar != '=' )
            {
				symbol	= 1820;              // ">", "gt"
                *v10	= 0;
                syml	= strlen(sym);
                c		= NextChar;
                return symbol;
			}
            *v10++	= '=';
            symbol	= 1810;                   // ">=", "ge"
			break;	//goto LABEL_85;

		default:							// default handler
            *v10++	= NextChar;
            *v10	= 0;
            symbol	= loadsym(sym);			// examine list of pre-defined symbols
			break;
		}

// general exit routine
//LABEL_85:
		if ((NextChar = getnch()) == -1)
			return 0;
		*v10 = 0;
        syml = strlen(sym);
        c = NextChar;
        return symbol;
	}
}

int getnch()
{
	int NextChar; // edx@3

	while ( 1 )
	{
		if ( (NextChar = getch()) != -1 )	// -1 == EOF
			break;
		
		// reached EOF at current file, see if we are in an "include" file

		if ( !FileTree->parentINC )			// true if we have descended into an include file
			break;	// this means we are at end of all files!
		
		fclose(FileTree->filePTR);			// close currently open include file
		mfree_0(FileTree->FileName);
		
		FileTree = FileTree->parentINC;		// back up one level
		mfree_0(FileTree->childINC);		// free memory used by the (now-closed) child
		
		MainFile = FileTree->filePTR;		// current file's parent
		ll = FileTree->LastLine;
  }
  return NextChar;
}

int nxtch()
{
	return c;
}

void gotonl()
{
     if ( c != '\n' )
     {
          do
          {
               c = getch();
          }
          while ( c != -1 && c != '\n' ); // EOF or <CR>
     }
     symbol = 930;
     cdbcpystr(sym, "\n", 0);
}

#endif
