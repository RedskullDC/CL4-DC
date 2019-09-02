#ifndef CDBRINDEX_C
#define CDBRINDEX_C

#include "DBdefs.h"
#include "cl4.h"
void DumpPOS(POS* Pos);

short cdbrindex(POS *pos, short TDFentry)
{
  	//printf("cdbrindex(x%08X,%d, [x%04X])\n" ,pos,TDFentry,TDFentry);
	while ( pos && pos->ElementID )
	{
		if ( pos->ElementID == TDFentry )
			return pos->BuffOffset;			// found a match. return offset to caller
		pos++;
	}
	return -1;	// not found or error with pos pointer
}

#endif
