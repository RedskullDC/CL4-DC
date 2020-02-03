#ifndef NEWEXP_C
#define NEWEXP_C

#include "DBdefs.h"

EXPR*	newexpr(short Type, short Operator, char *ExprData, size_t ExprSize)
{
	EXPR *Expr;

	Expr = (EXPR *)alloc(ExprSize + sizeof(EXPR), 0);	// Expression = variable length struct. EXPR + expr size
	//printf("newexpr( EXPR: x%08X, Operator: x%04X)\n", Expr,Operator);
	Expr->NextEXPR	= 0;
	Expr->PrevEXPR	= 0;
	Expr->Operator	= Operator;
	
	Expr->Type		= Type;								// Normally a TDFtype from a tdef record
	Expr->ExprSize	= ExprSize;
	cpybuf(&Expr->ExprData, ExprData, ExprSize);		// Data is copied to Offset 0x0E (14)
	*(char*)(&Expr->ExprData + ExprSize) = 0;			// Terminate with a ZERO

	return Expr;
}

#endif

