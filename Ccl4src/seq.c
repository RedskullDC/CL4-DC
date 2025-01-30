#ifndef SEQUENCE_C
#define SEQUENCE_C

#include "DBdefs.h"
#include "cl4.h"

void newseq(TDinfo *TDptr)
{
	char *v1;

	//printf("newseq( TDptr: x%08X)\n", TDptr);
	if ( !TDptr->KeyBuf1 )
	{
		v1 = ealloc(2 * TDptr->TDKeySize, 0);
		TDptr->KeyBuf1 = v1;
		TDptr->KeyBuf2 = &v1[TDptr->TDKeySize];
	}
}

void relseq(TDinfo *TDptr)
{
	//printf("relseq( TDptr: x%08X)\n", TDptr);
	if ( TDptr->KeyBuf1 )
	{
		nfree(TDptr->KeyBuf1, 0);
		TDptr->KeyBuf1 = 0;
		TDptr->KeyBuf2 = 0;
	}
	relnode(TDptr->TDNodePtr);
	TDptr->TDNodePtr = 0;
}

#endif
