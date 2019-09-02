#ifndef CLRTABLE_C
#define CLRTABLE_C

/*
// clrtable()
// _clrtable() not called in clenter

int clrtable(int TDno)
{
    return (signed __int16)xenter((int)clrtable, TDno);
}

short _clrtable(int TDno)
{
    TDinfo *TDptr; // eax@1
    TDinfo *TDptr2; // esi@1
    int v3; // eax@1
    short Depth; // di@5
    int PageNo; // [sp+18h] [bp-10h]@5

    TDptr = chktd(TDno);
    TDptr2 = TDptr;
    v3 = check_bup_sem(TDptr->TDDBinfo);
    if ( v3 )
        derror(v3, 0, TDptr2);
    if ( TDptr2->TDFlags & 4 )
        derror(37, 0, TDptr2);
    Depth = rhead(TDptr2->TDDBinfo, TDptr2->TDindexOff, &PageNo);
    whead(TDptr2->TDDBinfo, TDptr2->TDindexOff, 0, 0);// Clobber header page and Depth values in Master table.
    return sparetree(TDptr2, PageNo, Depth);    // Go through tree and mark all pages as Free!
}

*/
bool _sparetree(TDinfo *TDptr, int PageNo, short Depth)
{
	NODE *node;
	int *PageList;
	DBinfo *DBptr; 
	short v7;

	//printf("_sparetree( TDptr: x%08X ,PageNo: %d, Depth: %d\n", TDptr, PageNo, Depth);

	if ( PageNo && Depth )
	{
		if ( Depth > 1 )
		{
			node = getnode(TDptr, PageNo, 2);

			for (v7 = 0 ; node->NumEntries >= v7 ; v7++ )
				_sparetree(TDptr, node->NODE2ptr[v7].PageNo, Depth - 1);	// recursion here

			relnode(node);
		}
		
		for ( PageList = TDptr->TDDBinfo->SpareList; *PageList; ++PageList )	// PageList points to last spare PageNo after this
			;
    
		DBptr = TDptr->TDDBinfo;
		if ( &DBptr->SpareList[32] <= PageList )              // At/Past end of table?
		{
			_mkfree(DBptr, DBptr->SpareList);
			PageList = TDptr->TDDBinfo->SpareList;
		}
		*PageList = PageNo;
		PageList[1] = 0;
		return true;		// pages were returned to freelist. Nothing currently examines return code!
	}
	else
		return false;		// no pages were returned.
}

#endif
