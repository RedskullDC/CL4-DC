#ifndef CPYTUC_C
#define CPYTUC_C

#include "DBdefs.h"
#include "cl4.h"


int cpytuc(char *Dest, char *Src, int MaxLen)
{
	int i; // esi@1
	char v4; // al@2
	//char *v5; // edi@2

	//printf("cpytuc( DEST: x%08X, SRC: x%08X, MaxLen : %d)\n", Dest, Src, MaxLen);

	// No protection on MaxLen value.
	// Will happily copy past the end of the string !!!

	for ( i = MaxLen; i; --i )
	{
		v4 = *Src++;
		//if ( (v4 - 97) <= 25u )	// <- gcc opt creates this
		if ( v4 >= 'a' && v4 <= 'z')
			v4 -= 32;				// convert to uppercase
		*Dest++ = v4;
	}
	return MaxLen;
}


// 'a' == 0x61 (97)  -> 'z' == 0x7A (122)    122 - 97 = 25

#endif
