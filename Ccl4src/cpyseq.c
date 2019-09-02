#ifndef CPYSEQ_C
#define CPYSEQ_C

#include "DBdefs.h"
#include "cl4.h"
int cpyseq(char *DEST, char *SOURCE, int MaxLen)
{
	int v3;
	char *v4;
	char *v6;
	char v7;

	//printf("cpyseq( DEST: x%08X, SRC: x%08X, MaxLen : %d)\n", DEST, SOURCE, MaxLen);

	v3 = MaxLen;
	v4 = DEST;
	if ( MaxLen )
	{
		v6 = SOURCE;
		do
		{
			v7 = *v6++;
			*v4++ = v7;
			if ( !--v3 )
				break;
		}
		while ( v7 );
	}
	return v4 - DEST;	// return number of bytes copied
}

#endif
