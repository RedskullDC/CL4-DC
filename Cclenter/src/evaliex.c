#ifndef EVALIEX_C
#define EVALIEX_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"            // for flag bits

// only called from _ploop() : if (300) /while (310) handlers

bool evaliex(ENTAB *entab, bool Skip)			// Evaluate an expression for true/false
{

	ENTAB	*entb;
    char	*v23;
	FLDdesc *fldp;
	double	v31;
	
    int		OpCode;
	int		v18;
	signed int v24;
	
    short	entTTno;
	short	OpBits; 
	FLDdesc fld;
	
	char	Buf1[1008];
    char	Buf2[1008];
    char	ExpType;
	
    if ( !entab )
        return false;

	entTTno = entab->TTno;
	OpCode	= entTTno & 0xFC00;				// Integer calc flag masked out 0x0200
	
	//printf("evaliex(x%08X, %d) : entab->entype == %d, entab->TTno = x%04X\n",entab,Skip,entab->entype,entTTno);

	if ( entab->entype == 2 )
	{
		if ( entTTno & 0x0100 )				// reserved word flag, always test as a string value
			return *evalstr(entab, Buf2) != 0;

		else if ( entTTno & 0x3F )			// relational operators
		{
			OpBits = entTTno & 0x1FF;		// 0001 1111 1111
			switch ( OpBits )				//      **          = invalid bits - trapped at end of block
			{
				case 0x01:											// OR of two expressions
					if ( evaliex(ENARR(entab->enleft), Skip))		// ** recursion
						return true;
					return evaliex(ENARR(entab->enright), Skip);
				
				case 0x02:											// AND of two expressions
					if ( !evaliex(ENARR(entab->enleft), Skip))		// ** recursion
						return false;
					return evaliex(ENARR(entab->enright), Skip);
		
				// 3 Operators bits: 
				//                   GE L  
				//                   TQ T
				// operator bits : 0011 1000
				
				case 0x08:		// 0x08 == less than				'<'
				case 0x10:		// 0x10 == equal to					'='
				case 0x20:		// 0x20 == greater than				'>'
				case 0x14:		// 0x14 == equal to/wildcard test?	'like'	0x04 == regex/wildcard test?

				// Mixed bit tests:
				case 0x18:		// 0x18 == less than or equal		'<='
				case 0x30:		// 0x30 == greater than or equal	'>='
				case 0x28:		// 0x28 == not equal to				'<>'

					ExpType = getetype(ENARR(entab->enleft));
					
					if ( entTTno & 0x0200 && ExpType != 'C' )	// 0200 == int calc flag
						v18 = evalint(ENARR(entab->enleft)) - evalint(ENARR(entab->enright));
					
					// ucase(), lcase(), day(), getarg(), chr(), getenv(), sdel(), sword(), getxml(), getcgi(), substr(), sconv(), srep(), scat()
					// smult() 0x7800 should be in here???

					else if (ExpType == 'C' || OpCode == f_UCASE || OpCode == f_LCASE || OpCode == f_DAY || OpCode == f_GETARG
						     || OpCode == f_CHR || OpCode == f_GETENV || OpCode == f_SDEL || OpCode == f_SWORD || OpCode == f_GETXML 
						     || OpCode == f_GETCGI || OpCode == f_SUBSTR || OpCode == f_SREP || OpCode == f_SCONV || OpCode == f_SCAT )
					{
						evalstr(ENARR(entab->enleft), Buf2);
						evalstr(ENARR(entab->enright), Buf1);
						
						if (OpBits == 0x14)		// 0x14 == regex/like test 
						{
							v23 = Buf1; 
							while ((v24 = (unsigned char)*v23))
							{
								//if ( v24 == '[' || v24 == '*' || v24 == '?')			wildcard specifiers. xor the high bits
								if ( v24 == 0x5Bu || v24 == 0x2Au || v24 == 0x3Fu)
									*v23 |= 0x80u;
								else if ( v24 == 0xAAu || v24 == 0xDBu || v24 == 0xBFu)	// same as above with high bits set
									*v23 &= 0x7Fu;
								v23++;
							}
						}
						v18 = compare(Buf2, Buf1);
					}
					else	// numeric compare
					{
						v31 = clround(clround(evalnum(ENARR(entab->enleft), ExpType), ExpType) - clround(evalnum(ENARR(entab->enright), ExpType), ExpType), ExpType);
						v18 = (v31 < 0.0) ? -1 : (v31 > 0.0);	// sign(v31)  -1, 0, 1
					}
					// check comparison in relation to specified operator.
					switch ( OpBits )
					{
						case 0x08:					// <  less than
							return v18 < 0;
						case 0x10:					// =  equal to
						case 0x14:					// =  equalt to, with wildcard
							return v18 == 0;
						case 0x18:					// <= less than or equal
							return v18 <= 0;
						case 0x20:					// >  greater than
							return v18 > 0;		
						case 0x28:					// != not equal to
							return v18 != 0;
						case 0x30:					// >= greater than or equal
							return v18 >= 0;		
						default:					// no-op, shouldn't be reachable
							return 0;
					}
					break;
				default:
					assert(0);						// *ERROR*
			}
		}
		else	// rest are single endpoint. Check for non-zero
		{
			switch (OpCode)
			{
			case 0x0000:								// *ERROR*
				assert(0);
				break;
			
			case f_NOT:									// not()
				return (evaliex(ENARR(entab->enleft), Skip) == false);		// *** recursive ***
				break;

			case f_SQRT :								// sqrt()	
			case f_LOG  :								// log()
			case f_SIN  :								// sin()
			case f_COS  :								// cos()
			case f_TAN  :								// tan()
			case f_LOG10:								// log10()
			case f_ASIN :								// asin()
			case f_ACOS :								// acos()
			case f_ATAN :								// atan()
			case f_POW  :								// exp()	** power()
			case f_DEGREES :							// degrees()
			case f_RADIANS :							// radians()
			case f_EXP  :								// exp()
				return evalnum(entab, 'F') != 0.0;
				break;

			case f_NAME :								// name()
				evalstr(ENARR(entab->enleft), Buf2);
				return (isfname(Buf2) == 0);
				break;
			
			case f_SLEN :								// slen()
			case f_ASC  :								// asc()
			case f_CHR  :								// chr()
				Buf2[0] = 0;	// evaluate as string, and check for non-zero string
				return *evalstr(ENARR(entab->enleft), Buf2) != 0;
				break;

			case f_MATCH:								// match()
				evalstr(ENARR(entab->enleft), Buf2);
				evalstr(ENARR(entab->enright), Buf1);
				return (smatch(Buf1, Buf2) != 0);
				break;

			case f_ARRSTART :							// Array subscript operation [] - check ZERO flag
				return ((unsigned char)((unsigned int)getftf(entab, 1, &fld)->FLDstat >> 2) ^ 1) & 1;
				break;

			case f_KEYREADY :							// keyready()
				return Skip ? Skip : Tty_KeyIsReady();
				break;

			default:
				entb = ENARR(entab->enleft);			// drill down the left tree to the end
					while (entb && entb->enleft)
						entb = ENARR(entb->enleft);

				fldp = getftf(entb, 0, &fld);
				
				if ( entab->entype == 2 && entab->TTno & 0x0200 )		// 0x0200 == int calc flag	move this up?
					return (evalint(entab) != 0);
				
				else if ( fldp->FLDtype == 'C' )
					return (*evalstr(entab, Buf2) != 0);

				else
					return evalnum(entab, fldp->FLDtype) != 0.0;
				break;
			}
		}
	}
	else if ( entab->entype == 8 )				// literal int value
		return *(int *)&entab->TTno != 0;
	else										// test ZERO flag on other types
		return ((unsigned char)((unsigned int)getftf(entab, 1, &fld)->FLDstat >> 2) ^ 1) & 1;
}
#endif
