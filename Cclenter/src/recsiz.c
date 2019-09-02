#ifndef RECSIZ_C
#define RECSIZ_C

#include "DBdefs.h"

// functions will be "inlined" when building release version with optimisation ON.

inline int recsize(int TDno)
{
	//printf("recsize(%3d)\n", TDno);
	return _chktd(TDno)->TDRecSize;

}
// Possible name conflict with _keysize which exists in src/keysiz.c
inline int keysize(int TDno)
{
	//printf("keysize(%3d)\n", TDno);
	return _chktd(TDno)->TDKeySize;
}

#endif
