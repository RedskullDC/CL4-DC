#ifndef ISFNAME_C
#define ISFNAME_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// for islower() and toupper() functions
#include "DBdefs.h"
#include "cl4.h"

// Does this string represent a valid filename? return (char) 0  if so.
// Starts with alpha, then only alphanumeric or underscore

char isfname(char *v1)
{
    if (isalpha(*v1))									// isalpha()  ==	0x0400
    {
        while (*v1 && (isalnum(*v1) || *v1 == '_'))		// isalnum()  ==	0x0008
            ++v1;
    }
    return *v1;		// if all the characters were legal, return ZERO
}


#endif
