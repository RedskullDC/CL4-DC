#ifndef NEWJUNC_C
#define NEWJUNC_C

#include "DBdefs.h"

EXPR* newjunc(EXPR *a1,short Operator,EXPR *a3)
{
	EXPR *result; // eax@1

	result = a3;
	if ( a1 )
	{
		result = a1;
		if ( a3 )
		{
			result = (EXPR *)alloc(12u, 0);	// sizeof(EXPR) is actually 16 bytes 
			result->NextEXPR = a1;
			result->PrevEXPR = a3;
			result->Operator = Operator;	// 1 = OR,  2 = AND
		}
	}
	return result;
}
#endif

