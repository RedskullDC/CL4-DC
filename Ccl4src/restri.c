#ifndef RESTRICT_C
#define RESTRICT_C

#include <sys/param.h>			// for MIN/MAX
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int getwpos(char *Buffer, short Size)
{
	int v2;
	char *v3;
	unsigned char v4;
	int result;
	
	//printf("getwpos( Buffer: x%08X , Size: x%04X (%d)\n", Buffer, Size, Size);

	v2 = Size;
	v3 = Buffer;
	//DumpBlock(Buffer,Size);

	if ( !Size || !*Buffer )
		return -1;
	while ( *v3 >= 0 )			// while normal ascii
	{
		++v3;
		--v2;
		if ( !v2 || !*v3 )
			return -1;			// reached end of string, or field size, and no wildcard found
	}
	v4 = *v3;
	
	// Wildcard chars with the high bit set

	// if ( v6 == '*' || v6 == '?' || v6 == '[')
    // if ( v6 == 0x2A || v6 == 0x3F || v6 == 0x5B)
	
	// if ( v4 == -86 || v4 == -65 || v4 == -37 )
	if ( v4 == 0xAAu || v4 == 0xBFu || v4 == 0xDBu )
		result = v3 - Buffer;	// wildcard found at Buffer offset
	else
		result = -1;	// -1 means wildcard not found
	//printf("getwpos: result = %d\n",result);
	return result;
}

// ** Not public in libcl4 **
short xkey(char *KeyBuf, EXPR *Expr, short Operator, TDef *tdef)
{
	EXPR *expr;
	char *dest;

	dest = KeyBuf;
	*KeyBuf = 0;
	
	while ( tdef->TDFentry && tdef->TDFtype & tdf_KEY )	// only look at Key fields
	{
		expr = _bound(Expr, Operator, tdef);
		if ( !expr || tdef->TDFtype & tdf_STRING && getwpos(&expr->ExprData, expr->ExprSize) >= 0 )
			break;

		dest += cpybuf(dest, &expr->ExprData, MIN(expr->ExprSize,tdef->TDFsize));

		if ( tdef->TDFsize > expr->ExprSize )
		{
			if ( !(tdef->TDFtype & tdf_STRING) || *(dest - 1) )
				break;
		}
		tdef++;
	}
	return dest - KeyBuf;
}

// ** Not public in libcl4 **
short _restrict(int TDNo, EXPR *Expression)
{
	TDinfo *TDptr; 
	short result = 0;

	TDptr = _chktd(TDNo);
	relseq(TDptr);
	TDptr->KeyDefSize = 0;

  	//printf("_restrict( TDno: x%04X (%3d), EXPR: x%08X, TD->f0 x%04X)\n", TDNo,TDNo,Expression, (unsigned short)TDptr->TDFlags);

	TDptr->TDFlags &= 0xFDEF;			// 1111 1101 1110 1111

	if ( TDptr->TDFlags & 0x0002 )
		TDptr->TDFlags |= 0x1000;

	if ( TDptr->TDFlags & 0x0008 )
	{
		newseq(TDptr);
		TDptr->Key1Size = xkey(TDptr->KeyBuf1, Expression, 0x30u, TDptr->TableDefs);	// 0x30 == ">="
		TDptr->field_3A = 1;
		TDptr->Key2Size = xkey(TDptr->KeyBuf2, Expression, 0x18u, TDptr->TableDefs);	// 0x18 == "<="
		TDptr->field_42 = 2;
		//result = 0;
		if ( TDptr->Key1Size || TDptr->Key2Size )
			result = 1;
	}
	return result;
}

// HACK!!  Should call through xenter()

short cdbrestrict(int TDno,EXPR *Expr)
{
  return _restrict(TDno, Expr);
}

#endif
