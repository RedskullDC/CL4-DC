#ifndef POWER_C
#define POWER_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// for islower() and toupper() functions
#include <math.h>		// avoid 'implicit definition' warning
#include "DBdefs.h"
#include "cl4.h"

double power(double base, double exp)
{
    return pow(base, exp);
}

#endif
