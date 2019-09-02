#ifndef BEYOND_C
#define BEYOND_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for TDFtype bit field values

int _beyond(EXPR *SelectExpr, short Operator, char *a3, TDef *tdef)
{
	EXPR	*Expr;
	short	v5;
	short	v7;
	short	TDFsize2;
	bool	Wild;
	char	v17[152];

	//printf("_beyond( EXPR: x%08X,Operator: x%04X, TDef: x%08X)\n", Expr,Operator,tdef);
	
	Wild = false;
	while ( tdef->TDFtype & tdf_KEY)	// tdef->TDFtype & 1 = key fields
	{	
		Expr = _bound(SelectExpr, Operator, tdef);
		if ( !Expr )
			break;

		if ( tdef->TDFtype & tdf_STRING )
			TDFsize2 = _fwidth(a3, tdef->TDFsize, tdef->TDFtype);
		else
			TDFsize2 = tdef->TDFsize;

		if ( tdef->TDFtype & tdf_STRING && (v5 = getwpos(&Expr->ExprData, Expr->ExprSize), v5 >= 0))	// We found a wildcard character at v17[v5]!!
		{	
			strncpy(v17, &Expr->ExprData, v5);
			v17[v5] = 0;
			Wild = true;
			v7 = _cmpattrib(a3, TDFsize2, v17, v5, tdef->TDFtype);
		}
		else
			v7 = _cmpattrib(a3, TDFsize2, &Expr->ExprData, Expr->ExprSize, tdef->TDFtype);
    
		if ( Operator & 0x08 && v7 > 0 || Operator & 0x20 && v7 < 0 )
			return 1;	// we are beyond all records that match, exact match type

		if ( Operator & 0x08 && v7 < 0 || Operator & 0x20 && v7 > 0 || Wild || Expr->ExprSize < TDFsize2 )
			break;		// we found a wildcard match?

		a3 += TDFsize2;
		tdef++;
	}
	return 0;
}

#endif

	// 3 Operators bits: 
	// 0x08 == less than				'<'
	// 0x10 == equal to					'='
	// 0x20 == greater than				'>'

	// Mixed bit tests:
	// 0x18 == less than or equal		'<='
	// 0x30 == greater than or equal	'>='
	// 0x28 == not equal to				'<>'

