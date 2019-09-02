#ifndef BOUND_C
#define BOUND_C

#include "DBdefs.h"
#include "cl4.h"
EXPR* _bound(EXPR *Expr, short Operator, TDef *Tdefp);

EXPR* _bound(EXPR *Expr, short Operator, TDef *Tdefp)
{
	EXPR *v4;
	EXPR *v7;
	short v5;

	// possible for many levels of recursion to happen inside _bound() !!
	
	if ( Expr )
	{
		//printf("_bound( EXPR: x%08X,Operator: x%04X, Expr->Op: x%04X,TDef: x%08X)\n", Expr,Operator, Expr->Operator, Tdefp);
		if ( Expr->Operator == 1 )  // Expr is actually a "join". Operator == 1 means an "OR" join.
		{
			v7 = _bound(Expr->NextEXPR, Operator, Tdefp);
			if ( !v7 || (v4 = _bound(Expr->PrevEXPR, Operator, Tdefp), !v4) )
				return 0;

			v5 = _cmpattrib(&v7->ExprData, v7->ExprSize, &v4->ExprData, v4->ExprSize, Tdefp->TDFtype);
			if ( v5 < 0 )
				return (Operator & 0x20) ? v7 : v4;
			
			else if (v5 > 0)
				return (Operator & 0x20) ? v4 : v7;
			
			else	// ( v5 == 0 )
				return (v7->ExprSize < v4->ExprSize) ? v7 : v4;

		}
		else if ( Expr->Operator == 2 )  // Expr is actually a "join". Operator == 2 means an "AND" join.
		{
			v4 = _bound(Expr->NextEXPR, Operator, Tdefp);
			if ( v4 || (v4 = _bound(Expr->PrevEXPR, Operator, Tdefp)) != 0 )
				return v4;
		}
		else             // Normal single Expr type record
		{
			if ( ((unsigned short)Operator | Expr->Operator) == Operator && Expr->Type == Tdefp->TDFentry )
				return Expr;
		}
	}
	return 0;	// 0 == match. If no expr specified, default to match
}

	// 3 Operators bits: 
	// 0x08 == less than				'<'
	// 0x10 == equal to					'='
	// 0x20 == greater than				'>'

	// Mixed bit tests:
	// 0x18 == less than or equal		'<='
	// 0x30 == greater than or equal	'>='
	// 0x28 == not equal to				'<>'
	
#endif
