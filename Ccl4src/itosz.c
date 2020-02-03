#ifndef ITOSIZE_C
#define ITOSIZE_C

short _itosz(PAGE *pg, short N1_2idx)
{
	short	v3;
	short	*idx;

	//printf("_itosz(PAGE: x%08X, Index: x%02X (%4d), Pagetype = x%02X\n" ,pg,N1_2idx,N1_2idx,(pg->pgData.PageType & 0x40));
	
	if ( pg->pgData.PageType & 0x40 )	// variable record size
	{
		idx = (short*)pg->header.tsrs.TabStart;
		
		v3 = SWAP_SHORT(&idx[N1_2idx]);	// idx[0] is the real size of the first entry in page
		if ( N1_2idx )
			v3 -= SWAP_SHORT(&idx[N1_2idx-1]);
	}
	else
		v3 = pg->header.tsrs.RecSize;	// Fixed record size

	//printf("_itosz: returned x%02X (%4d)\n", v3,v3);
	return v3;
}

#endif
