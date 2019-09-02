#ifndef ITOPTR_C
#define ITOPTR_C

char * _itoptr(PAGE *PageBuffer,short N1_2idx)
{
	char *result;

	//printf("_itoptr(PAGE: x%08X, Index: x%02X (%4d)\n" ,PageBuffer,N1_2idx,N1_2idx);
	if ( PageBuffer->PageType & 0x40 )
	{
		result = (char*)PageBuffer->TabEnd;
		if ( N1_2idx )
		{
			//v2 = SWAP_SHORT((PageBuffer->TabStart_or_RecSize + 2 * (N1_2idx - 1)));
			//result += v2;
			result += SWAP_SHORT((PageBuffer->TabStart_or_RecSize + 2 * (N1_2idx - 1)));
		}
	}
	else
	{
		result = &PageBuffer->TabEnd[(int)PageBuffer->TabStart_or_RecSize * N1_2idx];
	}
	//printf("_itoptr: returned x%08X\n",result);
	return result;
}

#endif
