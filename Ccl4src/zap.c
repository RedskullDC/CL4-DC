#ifndef ZAP_C
#define ZAP_C

#include "DBdefs.h"
#include "cl4.h"

// production version will inline these functions

inline char* zap(char *a1, size_t NumBytes)
{
	//printf("zap( Buffer: x%08X , Size: %d [x%04X])\n", a1,NumBytes, NumBytes);
	return memset(a1, 0, NumBytes);
}

inline char* fillbuf(char *Buff,size_t NumBytes, char value)
{
	//printf("fillbuf( Buffer: x%08X , Size: %d [x%04X], x%02X)\n", Buff,NumBytes, NumBytes, value);
	return memset(Buff, value, NumBytes);
}

#endif
