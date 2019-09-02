#ifndef REDISTRIBUTE_C
#define REDISTRIBUTE_C

#include "DBdefs.h"
#include "cl4.h"

int _redistrib(NODE *ND_left, NODE *ND_right, short PageSize)
{
	int NewSize;
	int RecsMoved = 0;

    //printf("_redistrib(ND_left: x%08X, ND_right: x%08X, Pagesize: %d [x%04X])\n" ,ND_left,ND_right,PageSize, PageSize);

	while( 1 )	// Try shifting recs from Node ND_left to Node ND_right
	{
		NewSize = ND_right->DataEnd + (ND_left->field_16 + ND_left->NODE1ptr[ND_left->NumEntries - 1].Size);
		if ( PageSize < NewSize )	// would moving this record exceed pagesize for Node ND_right?
			break;
		
		if ( ND_left->DataEnd - (ND_left->field_16 + ND_left->NODE1ptr[ND_left->NumEntries - 1].Size) < NewSize )
			break;				// Does Node ND_left now fit in a page? yes == break
		
		moveright(ND_left, ND_right, 1);		// ND_left > ND_right, 1 entry
		RecsMoved++;
	}

	while ( 1 )  // Try shifting recs from ND_right back to ND_left
	{
		NewSize = ND_left->DataEnd + (ND_right->field_16 + ND_right->NODE1ptr[0].Size);
		if ( PageSize < NewSize )	// would moving this record exceed pagesize for Node ND_left?
			break;
		
		if ( ND_right->DataEnd - (ND_right->field_16 + ND_right->NODE1ptr[0].Size) < NewSize )
			break;				// Does Node ND_right now fit in a page? yes == break
		
		moveleft(ND_left, ND_right, 1);	// ND_left < ND_right , 1 entry
		RecsMoved--;
	}
	//printf("_redistrib - recs moved: %d\n",RecsMoved); 
	return RecsMoved; // = Final number of recs moved from ND_left to ND_right, may be -ve value
}

#endif
