#ifndef FIXBNAME_C
#define FIXBNAME_C
#include "DBdefs.h"
#include "cl4.h"

short fixbname(char *Dest, char *Src)
{
     cdbcpystr(Dest, Src, 0);
     Dest[20] = 0;					// max buffer size == 21
     return strlen(Dest) + 1;		// include the \0 terminator byte
}

#endif
