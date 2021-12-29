#ifndef ITOPTR_C
#define ITOPTR_C
/*
char * _itoptr(PAGE *pg,short N1_2idx)
{
	char *result;

	//printf("_itoptr(PAGE: x%08X, Index: x%02X (%4d)\n" ,pg,N1_2idx,N1_2idx);
	if ( pg->PageType & 0x40 )
	{
		result = (char*)pg->TabEnd;
		if ( N1_2idx )
		{
			//v2 = SWAP_SHORT((pg->TabStart_or_RecSize + 2 * (N1_2idx - 1)));
			//result += v2;
			result += SWAP_SHORT((pg->TabStart_or_RecSize + 2 * (N1_2idx - 1)));
		}
	}
	else
	{
		result = &pg->TabEnd[(int)pg->TabStart_or_RecSize * N1_2idx];
	}
	//printf("_itoptr: returned x%08X\n",result);
	return result;
}
*/


char * _itoptr(PAGE *pg,short N1_2idx)
{
	char	*result;
	short	*offset;

	//printf("_itoptr(PAGE: x%08X, Index: x%02X (%4d), PT=x%02X \n" ,pg,N1_2idx,N1_2idx, pg->pgData.PageType & 0xc0 );

	if ( pg->pgData.PageType & 0x40 )
	{
		if ( N1_2idx )
		{
			
			offset = (short*)(pg->header.tsrs.TabStart + 2 * (N1_2idx - 1));	// short* value
			result = &pg->header.TabEnd[__builtin_bswap16(*offset)];
		}
		else
		{
			result = pg->header.TabEnd;
		}
	}
	else
	{
		result = &pg->header.TabEnd[pg->header.tsrs.RecSize * N1_2idx];  /// problem here
	}
	//printf("_itoptr: returned x%08X\n",result);
	return result;
}

/*

from 32 bit clenter

char *__cdecl itoptr(PAGE *pg, __int16 N1_2idx)
{
  if ( !(pg->pgData.PageType & 0x40) )
    return &pg->header.TabEnd[pg->header.tsrs.RecSize * N1_2idx];
  
  if ( N1_2idx )
    return &pg->header.TabEnd[pg->header.tsrs.TabStart[2 * N1_2idx - 1] | (pg->header.tsrs.TabStart[2 * N1_2idx - 2] << 8)];
 
  return pg->header.TabEnd;
}
*/

#endif
