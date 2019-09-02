#ifndef USAGE_C
#define USAGE_C

#include "DBdefs.h"

bool _usage(DBinfo *DBptr, int *UsedPages, int *Grow)
{
	int v4;
	uint64_t BitMapAddress;
	unsigned int TotalBytesToRead;
	size_t BytesToRead;

	int Pages;

	unsigned char v7;		// unsigned to stop arith shifting in 1's
	unsigned char v29; 
	unsigned char *v6;
	unsigned char BUF[1024]; 

	//printf("_usage(x%08X)\n", DBptr);

	TotalBytesToRead = (unsigned int)(DBptr->DBmaxpages + 7) >> 3; // One bit per page, 8 bits per byte
	BitMapAddress = (uint64_t)DBptr->DBfreeaddress;
	if (DBptr->DBversion == 1767) // Lge DB version
		BitMapAddress *= DBptr->DBpgsize;

	//-----------------
	if ( lseek64(DBptr->DBfilehandle, BitMapAddress ,0) >= 0 )
	{
		*UsedPages = 0;		// no sanity check on pointers!
		*Grow = 0;

		v4 = 0;
		if ( TotalBytesToRead )
		{
			while ( 1 )
			{
				if ( TotalBytesToRead < 1025 )
					BytesToRead = TotalBytesToRead;
				else
					BytesToRead = 1024;				// Read in 1024 byte slabs
				
				if ( BytesToRead != readfull(DBptr->DBfilehandle,BUF, BytesToRead))
					return false;					// Error condition. Can't read bitmap bytes!
				
				v6 = BUF;
				while ( v6 < &BUF[BytesToRead])     // Examine volume bitmap for used page info
				{
					v4 = (v4 + 15) & 0xFFFFFFF8;	// Round Grow size up to nearest 8 bits
					v7 = *v6;						// Current BitMap byte
					v29 = v7;

					if (v7 == 0xFF)					// Entire cluster used, skip loop check
						Pages = 8;
					else
					{
						for ( Pages = 0; v7; v7 >>= 1 )
						{
							if ( v7 & 0x01 )	// Page Marked as used.
								Pages++;
							else
							{
								if ( !Pages )
									--v4;
							}
						}
					}
					*UsedPages += Pages;
					if ( v29 )
						*Grow = v4;					// Num free pages this bitmap can describe without growing DBase??
					v6++;
				}
				TotalBytesToRead -= BytesToRead;
				if (!TotalBytesToRead)				// No bytes left to examine in BitMap?
					return true;					// correct exit. BitMap fully read.
			}
		}
		else
			return true;
	}
	return false;
}

#endif
