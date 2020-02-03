#ifndef NEWJUNC_C
#define NEWJUNC_C

#include "DBdefs.h"

EXPR* newjunc(EXPR *a1,short Operator,EXPR *a3)
{
	EXPR *exp; // eax@1

	exp = a3;
	if ( a1 )
	{
		exp = a1;
		if ( a3 )
		{
			//exp = (EXPR *)alloc(12u, 0);	// sizeof(EXPR) is actually 16 bytes 
			exp = (EXPR *)alloc(sizeof(EXPR), 0);	// sizeof(EXPR) is actually 16 bytes / 32 on X64
			exp->NextEXPR = a1;
			exp->PrevEXPR = a3;
			exp->Operator = Operator;	// 1 = OR,  2 = AND
		}
	}
	return exp;
}
#endif

