#ifndef FUNCTION_BIND_C
#define FUNCTION_BIND_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

intfnptr Function_Bind(const char *FnName, KEYFUNC *KFp)
{
	//print("Function_Bind(%s,)\n",FnName);
    while ( KFp->KeyFnName )
    {
		if (!strcmp(FnName, KFp->KeyFnName))
			return KFp->Key_Fn;					// return function ptr
		KFp++;
    }
	return 0;	// no match or KEYFUNC ptr was null
}

/*
typedef struct 
{
	char*	KeyFnName;
	int		(*Key_Fn)(KEY_Elist*);		// *** Need to check ***
	char*	KeyFnDesc;
}KEYFUNC;
*/
#endif
