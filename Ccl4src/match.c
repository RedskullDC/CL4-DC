#ifndef MATCH_C
#define MATCH_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// not public in libcl4 
bool Star(char *s, char *p)
{
	while ( !wildmat(s, p) )
	{
		if ( *s )
		{
			++s;
			if ( *s )
				continue;
		}
		return false;
	}	
	return true;
}

// not public in libcl4 
bool isregexp(char *v1)
{
	char v2;
	while ( *v1 >= 0 )	// look for a char with high bit set
	{
		if ( !*v1++ )
			return false;
	}
	v2 = *v1 & 0x7F;
	return ( v2 == '?' || v2 == '[' || v2 == '*');
}

// not public in libcl4 
int wildmat(char *s, char *p)
{
	signed int v4;	
	int v5;			
	int v6;			
	int result;		
	char *v8;		
	int v9;			
	int v10;		
	int v11;		
	int v12;		
	int v13;		
	int v14;		
	int reverse;	
	int matched;	
	signed int v17;

	//printf("wildmat(%s,%s)",s,p);

	if ( !*p )
		return *s == 0;
	
	while ( 1 )
	{
		v4 = *p;
		if ( v4 == -65 )
		{
			if ( !*s )
				return 0;
			goto LABEL_50;
		}
		if ( v4 > -65 )
		{
			if ( v4 == -37 )
			{
				/* [^....] means inverse character class. */
				reverse = p[1] == '^';
				if ( reverse )
					p++;

				v17 = 256;
				matched = 0;
				++p;
				if ( !*p || *p == ']' )
				{
					result = 0;
					if ( matched == reverse )
						return 0;
					goto LABEL_50;
				}
				while ( 2 )
				{
					if ( *p == '-' )
					{
						++p;
						
						v9 = *s;
						if ( (*s - 65) <= 0x19u )
							v9 += 32;

						v10 = *p;
						if ( (*p - 65) <= 0x19u )
							v10 += 32;
						
						if ( v9 <= v10 )
						{
							v11 = (*s - 65) > 0x19u ? *s : *s + 32;
							if ( v11 >= v17 )
								matched = 1;
						}
					}
					else
					{
						v12 = *s;
						if ( (*s - 65) <= 0x19u )
							v12 += 32;
						
						v13 = *p;
						if ( (*p - 65) <= 0x19u )
							v13 += 32;
						
						if ( v12 == v13 )
							matched = 1;
					}

					v14 = *p;
					if ( (*p - 65) <= 0x19u )
						v14 += 32;

					v17 = v14;
					++p;
					if ( !*p || *p == ']' )
					{
						if ( matched == reverse )
							return result;
						goto LABEL_50;
					}
					continue;
				}
			}
			if ( v4 == 92 )	// '\\'
				++p;
			goto LABEL_9;	// fall into default routine
		}
		if ( v4 == -86 )	// '*'
		{
			v8 = (p + 1);
			if ( *v8 )
				result = Star(s, v8);
			else
				result = 1;
			return result;
		}

LABEL_9:
		v5 = *s;
		if ( (*s - 65) <= 0x19u )
			v5 += 32;
		
		v6 = *p;
		if ( (*p - 65) <= 0x19u )
			v6 += 32;

		if ( v5 != v6 )
			return 0;
LABEL_50:
		++s;
		++p;
		if ( !*p )
			return *s == 0;
	}
}

bool _match(char *src, EXPR *Expr, TDef *tdef)
{
	size_t FieldLen;	
	int v8;				
	short v9;			
	bool IsReg;			
	char dest[1036];
	
	//printf("_match(src = x%08X, EXPR = x%08X, tdef = x%08X, Operator = x%02X, ExprType = x%04X)\n", src, Expr, tdef,Expr->Operator, Expr->Type);

	if ( !Expr )					// No expression always matches true
		return true;
	
	if ( Expr->Operator == 1 )      //  Expr was an "OR" type join
	{
		//printf("_match : Op == 1\n");
		return ( _match(src, Expr->NextEXPR, tdef) | _match(src, Expr->PrevEXPR, tdef));		// recursive
	}

	if ( Expr->Operator == 2 )		// Expr is actually a "join". Operator == 2 means an "AND" join.
	{
		//printf("_match : Op == 2\n");
		return ( _match(src, Expr->NextEXPR, tdef) & _match(src, Expr->PrevEXPR, tdef));		// recursive
	}

	// 4 Operators bits: 
	// 0x08 == less than				'<'
	// 0x10 == equal to					'='
	// 0x20 == greater than				'>'

	// Mixed bit tests:
	
	// 0x14 == equal to/wildcard test?  'like'  0x04 == regex/wildcard test?

	// 0x18 == less than or equal		'<='
	// 0x30 == greater than or equal	'>='
	// 0x28 == not equal to				'<>'
	
	//                   GE LW  
	//                   TQ TL
	// operator bits : 0011 1100

	if ( Expr->Operator & 0x38 )                // Leave operator bits remaining: 0011 1000
	{											// This is a single expression.
		IsReg = isregexp(&Expr->ExprData);		// Only meaningful is expression is a string
		
		while ( tdef->TDFentry )
		{
			FieldLen = tdef->TDFtype & tdf_STRING ? _fwidth(src, tdef->TDFsize, tdef->TDFtype) : tdef->TDFsize;
			//printf("_match - tdef->TDFentry = x%04X, Expr->Type x%04X, Operator = x%02X\n",tdef->TDFentry,Expr->Type,(Expr->Operator &0x38));
			if ( tdef->TDFentry == Expr->Type )
			{
				// we found a match on TDFentry and Expr-Type if we got here.
				if ( tdef->TDFtype & tdf_STRING && IsReg )	// String type field and RegEx found above
				{
					if ( tdef->TDFsize == FieldLen )
					{
						strncpy(dest, src, FieldLen);
						dest[FieldLen] = 0;
						v8 = wildmat(dest, &Expr->ExprData);
					}
					else
						v8 = wildmat(src, &Expr->ExprData);

					if ( v8 || Expr->Operator & 0x10 )	// wildmatch or equal to
					{
						return ( v8 && Expr->Operator & 0x10 );
					}
				}
				else	// non-string type field, or not trying to match a regex
				{
					v9 = _cmpattrib(src, FieldLen, &Expr->ExprData, Expr->ExprSize, tdef->TDFtype);
					//printf("_match - cmpattrib returned %d\n",v9); 
					if ( (v9 || !(Expr->Operator & 0x10)) && (v9 >= 0 || !(Expr->Operator & 0x08)) && (v9 <= 0 || !(Expr->Operator & 0x20)) )
						return false;
				}
				return true;
			}
			src += FieldLen;	// Keep moving through the buffer
			tdef++;
		}
	}
	return true;	// no operator bits present. Always match exit?
}

#endif



