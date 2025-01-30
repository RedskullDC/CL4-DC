#ifndef GETPATH_C
#define GETPATH_C

#include "DBdefs.h"
#include "cl4.h"

int _getpath(int *PageList, TDinfo *TDptr)
{

	short i;
	int Depth; // Depth - Level into the tree structure
	PAGE_NODE p_pgnode;
	short N1_2idx;

	//printf("_getpath(PagePtr: %08X,TDptr: %08X) Called - ",PageList,TDptr);
	Depth = _rhead(TDptr->TDDBinfo, TDptr->TDindexOff, PageList);	// Updates PageList[0] with Master index PageNo
	for ( i = Depth; i > 1; --i )
	{
		p_pgnode.PAGE = _indexpg(TDptr, *PageList);		// Descend down tree till we find target PageNo that matches TDptr->TDKeyDefs data
		
		if ( _scanpg(p_pgnode, TDptr, &N1_2idx, 9))	// 9 indicates PAGE structure being passed (&8)
			++N1_2idx;

		PageList++;
		*PageList = mstol((int *)&p_pgnode.PAGE->header.DataStart[4 * N1_2idx]);	// Update rest of PageList array as required.
		//printf("Pagelist[%d] = x%04X (%4d) \n",i,*PageList2,*PageList2);
	}
	//printf("Returning %d\n",Depth);
	return Depth;
}
#endif
