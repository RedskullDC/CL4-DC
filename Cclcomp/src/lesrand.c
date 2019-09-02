#ifndef LESRAND_C
#define LESRAND_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

bool lesrand(PTAB *ptab)
{
     short v1;
	 
     symbol = getsym();
     if ((v1 = loadexp(0, 1)))
     {
          ptab->TABno = v1;
          return true;			// success
     }
     return false;				// error
}

#endif
