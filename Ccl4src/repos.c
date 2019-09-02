#ifndef REPOS_C
#define REPOS_C

#include "DBdefs.h"
#include "cl4.h"

void _addlist(int *PageList, int PageNo)
{
	int *i;

	//printf("_addlist( PAGElistPtr: x%08X, PageNo: x%04X (%3d))\n", PageList,PageNo,PageNo);
	for ( i = PageList; *i; ++i )
		;
	*i = PageNo;
	i[1] = 0;
	// returns a value in clenter, but is never used anywhere

	/*for ( i = PageList; *i; ++i )
        ;
    *i = PageNo;
    v3 = i + 1;
    *v3 = 0;
    return (signed __int16)(v3 - PageList);
	*/
}

int _repos(TDinfo *TDptr, NODE *Node)
{
	DBinfo *DBptr;
	int PageNo;	

	//printf("_repos( TDptr: x%08X, Node = x%08X\n", TDptr,Node);

	DBptr = TDptr->TDDBinfo;
	PageNo = _capture(DBptr);
	if ( !PageNo )
		derror(42, 0, TDptr);					// Error, no free pages left!
	
	_addlist(DBptr->PageList2, Node->PageNo);	// Add *old* PageNo to list of Pages to be marked as freed
	Node->PageNo = PageNo;
	_addlist(DBptr->PageList3, PageNo);			// Add *new* PageNo to list of Pages to be marked as used
	putnode(TDptr, Node);
	return PageNo;
}

#endif
