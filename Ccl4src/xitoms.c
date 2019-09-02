#ifndef XITOMS_C
#define XITOMS_C

#include <math.h>               // for exp functions stuff
#include "DBdefs.h"
#include "cl4.h"

int *xitoms(int *a1, int a2)
{
	unsigned char	*v1;
	v1 = (unsigned char*)a1;

	itoms((unsigned short*)(v1 + 1), a2);
	*v1 = a2 >> 16;
	return a1;
}



#endif
/*
BFFFF2F8 = 00 00 00 00 22 EE 01 FF 00 00 00 00 00 00 00 00  ...."...........

xitoms starts: xFF01EE22
BFFFF2FC = 22 EE 01 FF 00 00 00 00 00 00 00 00 F4 FD 64 30  ".............d0

itoms(a2 = xFFFFEE22) returns x22EE
xitoms returned 2289153 [x          22EE01]

BFFFF2F8 = 01 EE 22 hh 22 EE 01 FF 00 00 00 00 00 00 00 00  .."."...........

!!*real* routine doesn't touch high byte of the int. hh above

*/


