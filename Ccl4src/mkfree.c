#ifndef MARKFREE_C
#define MARKFREE_C

#include "DBdefs.h"
#include "cl4.h"

// *not public in libcl4
short mkspace(char *BitMapBuf, short PagesInBuf, int *PageList, int PageAtBufStart)
{
	short v4; // di@1
	int *i; // esi@1
	signed int PageNo; // ecx@2

	v4 = 0;
	for ( i = PageList; *i; ++i )
	{
		PageNo = *i - PageAtBufStart;
		if ( *i - PageAtBufStart >= 0 && PagesInBuf > PageNo >> 3 )
		{
			++v4;
			BitMapBuf[PageNo >> 3] &= ~(0x80u >> (PageNo - 8 * (PageNo >> 3)));
		}
	}
	return v4;
}

void _mkfree(DBinfo *DBptr, int *PageList)
{
	unsigned int BitMapBytesRemaining;
	signed short BytesToRead; // ax@8
	int ByteCount; // esi@10
	off64_t Offset; // [sp+38h] [bp-830h]@3
	int PageNoAtBufStart; // [sp+44h] [bp-824h]@7
	short ErrorCode; // [sp+48h] [bp-820h]@1
	char BitMapBuff[2048]; // [sp+50h] [bp-818h]@10

	//printf("_mkfree(DBptr: x%08X, PageList: x%08X)\n" ,DBptr,PageList);

	ErrorCode = 0;
	BitMapBytesRemaining = (DBptr->DBmaxpages + 7) >> 3;// Bit map entries = Num Pages/8
	
	if ( !_lockmap(DBptr, 1) )	// Lock the bitmap while we play with it.
		derror(18, DBptr, 0);
	
	Offset = DBptr->DBfreeaddress;
	if ( DBptr->DBversion == 1767 )		// large DBase support. FreeAddress is now a PageNo, not Byte offset in file.
		Offset *= DBptr->DBpgsize;
	
	if ( lseek64(DBptr->DBfilehandle, Offset, 0) < 0 )	// <0 == error seeking
	{
		_lockmap(DBptr, 0);
		derror(24, DBptr, 0);
	}
	
	for ( PageNoAtBufStart = 0; BitMapBytesRemaining; BitMapBytesRemaining -= ByteCount )
	{
		BytesToRead = 2048;
		if ( BitMapBytesRemaining < 2049 )
			BytesToRead = BitMapBytesRemaining;
		ByteCount = BytesToRead;
		if ( readfull(DBptr->DBfilehandle, BitMapBuff, BytesToRead) != BytesToRead )
		{
			_lockmap(DBptr, 0);
			derror(24, DBptr, 0);
		}

		if ( mkspace(BitMapBuff, ByteCount, PageList, PageNoAtBufStart) )
		{
			// returned pages to bitmap ok. Rewind and save that section of bitmap back
			if ( lseek64(DBptr->DBfilehandle, -ByteCount, 1) < 0 || write(DBptr->DBfilehandle, BitMapBuff, ByteCount) != ByteCount )
			{
				ErrorCode = 31;	// oops! we couldn't seek, or we couldn't save bitmap section back to disk!
				break;
			}
		}
		PageNoAtBufStart += 8 * ByteCount;
	}
	_lockmap(DBptr, 0);	// unlock bitmap
	if ( ErrorCode )
		derror(ErrorCode, DBptr, 0);
}
#endif
