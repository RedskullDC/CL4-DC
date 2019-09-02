#ifndef CAPTURE2_C
#define CAPTURE2_C

#include "DBdefs.h"
#include "cl4.h"
//#define _LARGEFILE64_SOURCE

// not public in libcl4
int findspace(unsigned char *Buffer, short a2, int *PageList, short PagesRequired, int a5)
{
	unsigned char*	v5;
	unsigned char* v10; 
	int v6;
	unsigned char bitmask;
	int *v11; 

	v5 = Buffer;
	v11 = PageList;
	v10 = &Buffer[a2];

	while ( v5 < v10 && PagesRequired > 0 )
	{
		if ( *v5 != 0xFF )	// all bits allocated = 0xFF
		{
			v6 = a5;
			bitmask = 0x80u;		// bit mask to test  1000 0000    >> 
			while ( bitmask && PagesRequired > 0 )
			{
				if ( !(bitmask & *v5) )	// true if corresponding bit in *v5 was 0	(page unallocated)
				{
					*v11 = v6 + 8 * (v5 - Buffer);	// 1 bit per page
					*v5 |= bitmask;	// mark page as "in use"
					++v11;
					--PagesRequired;
				}
				++v6;
				bitmask >>= 1;
			}
		}
		++v5;
	}
	return (v11 - PageList);	// Pages found, (may not match number requested!)
}

// not public in libcl4
int getspare(DBinfo *DBptr)
{
	int		*PageList;
	unsigned int BitMapBytes;
	int		v4;
	off64_t v8;
	int		BasePageNo;
	int		PagesFound;
	short	PagesRequired;
	short	v14;
	char	Buffer[2072];	// 2048 would do

	//printf("getspare(DBptr: x%08X)\n" ,DBptr);

	v14 = (1 << (8 - (DBptr->DBmaxpages & 7))) - 1;		// Bitmask of bytes to mask out in last bitmap byte 
	BitMapBytes = (DBptr->DBmaxpages + 7) >> 3;			// Whole bytes in Bitmap. Round up to nearest whole byte (8 Pages)
	
	if ( !_lockmap(DBptr, 1) )							//lock the Page bitmap while we extract free pages!
		derror(18, DBptr, 0);
	
	PageList = DBptr->SpareList;
	
	v8 = DBptr->DBfreeaddress;		
	if ( DBptr->DBversion == 1767 )
		v8 *= DBptr->DBpgsize;

	if ( lseek64(DBptr->DBfilehandle, v8, 0) < 0 )										// couldn't seek to bitmap area, error
	{
		_lockmap(DBptr, 0);
		derror(24, DBptr, 0);
	}
	PagesRequired = _nspare > 32 ? 32 : _nspare;					// default _nspare value is 20

	for ( BasePageNo = 0; BitMapBytes && PagesRequired > 0; BitMapBytes -= v4 )
	{
		v4 = BitMapBytes < 2049 ? BitMapBytes : 2048;				// Max 2048 bytes in buffer at a time

		if ( readfull(DBptr->DBfilehandle, Buffer, v4) != v4 )
		{
			_lockmap(DBptr, 0);
			derror(24, DBptr, 0);
		}
		if ( BitMapBytes == v4 )									// we reached end of bitmap buffer 
			Buffer[BitMapBytes - 1] |= v14;							// Last byte may have non-existant pages, mask them out.

		PagesFound = findspace(Buffer, v4, PageList, PagesRequired, BasePageNo);
		if ( PagesFound > 0 )
		{
			if ( BitMapBytes == v4 )								// we reached end of bitmap buffer 
				Buffer[BitMapBytes - 1] &= ~v14;					// undo earlier last byte page fudge above....

			if ( lseek64(DBptr->DBfilehandle, -v4, 1) < 0 || write(DBptr->DBfilehandle, Buffer, v4) != v4 )	// re-seek to start of buffer we just read. write it back to disk
			{
				_lockmap(DBptr, 0);
				derror(31, DBptr, 0);
			}
			PageList += PagesFound;
			PagesRequired -= PagesFound;
		}
		BasePageNo += 8 * v4;	// v4 bytes in buffer, 8 pages for each byte
	}

	_lockmap(DBptr, 0);	// we're done, unlock the bitmap
	return (PageList - DBptr->SpareList); // Number of pages allocated
}

int _capture(DBinfo *DBptr)
{
	int		*SpareList;
	int		PageNo;
	
	//printf("_capture(DBptr: x%08X)\n" ,DBptr);

	SpareList = DBptr->SpareList;

	// If no free pages in the list, use getspare to find some more.
	//if ( !DBptr->SpareList[0] )
	if ( !SpareList[0] )
		SpareList[getspare(DBptr)] = 0;	// and null terminate the list
	
	// pop a page, and move remaining entries down the list.
	for ( PageNo = *SpareList; *SpareList; *(SpareList - 1) = *SpareList )
		++SpareList;

	//printf("_capture(DBptr: x%08X = %d)\n" ,DBptr, PageNo);
	return PageNo;	// return a free page number
}


#endif
