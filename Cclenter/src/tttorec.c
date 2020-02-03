#ifndef TTTOREC_C
#define TTTOREC_C

#include <ctype.h>		// for islower() and toupper() functions
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

//void DumpBlock(char* Buffer, int NumBytes);

// Copy data from *fld->FLDdata to WorkArea in preparation for writing to DBase.
// Optionally: Only update the Key fields, and convert strings to uppercase

// Makes assumption that the WorkArea has already been cleared!!

void tttorec(int TTno, char *WorkArea, int KeysOnly)
{
	FLDdesc *fld;
	char	*buff_src;
	char	*v10;
	char	*v11;
	
	double	tmpDouble;
	int		tmpInt;
	short	tmpShort;

	int		v8;
	short	FLDstat;
	char	v14[8206];	// not sure why not 8192??

	//printf("tttorec(TTno = %d, Workarea = x%08X, KeysOnly = %d)\n" ,TTno,WorkArea, KeysOnly);

	// KeysOnly == 0 : copy all fields
	// KeysOnly == 1 : copy only *Key* fields, *and* convert and TEXT fields [C] to uppercase
	// KeysOnly == 2 : copy only *Key* fields, no case conversion.

	for ( fld = ttab[TTno].TTfields; fld->FLDelemID; fld++ )
	{
		//if ( KeysOnly && fld->FLDstat >= 0 )  // high bit set (<0) means key field
		if ( KeysOnly && !(fld->FLDstat & fld_KEY))  // high bit set (<0) means key field
			break;
		if ( fld->FLDlen )
		{
			FLDstat = fld->FLDstat;
			//if ( FLDstat & 0x0010 || (FLDstat & 0x0005) == 0x0004 )
			if ( FLDstat & fld_DATA_AVAIL || (FLDstat & (fld_REC_LOADED|fld_ZERO)) == fld_ZERO )
			{
				if ( fld->FLDtype != 'C' )
					tmpDouble = *(double*)fld->FLDdata;
				
				switch ( fld->FLDtype )
				{
					case 'B':
					case 'I':
						tmpShort = (short)tmpDouble;
						buff_src = (char*)&tmpShort;
						break;
					case 'D':
					case 'N':
						tmpInt = (int)tmpDouble;
						buff_src = (char *)&tmpInt;
						break;
					case 'C':
						if ( KeysOnly == 1 )	// KeysOnly == 1, convert string data to upper case
						{
							v10 = v14;		// 8192 0x2000 buffer
							v11 = (char *)fld->FLDdata;
							//v8 = *v11++;
							//while ( v8 )
							;
							while ((v8 = *v11++))
							{
								if (islower(v8))
									v8 = toupper(v8);
								*v10++ = v8;
								//v8 = *v11++;
							}	
							*v10 = 0;			// terminate dest string
							buff_src = v14;
							//printf("tttorec: Src = %s, Dest = %s\n",(char *)i->FLDdata, buff_src);
						}
						else
							buff_src = (char *)fld->FLDdata;
						break;
					default:
						buff_src = (char *)&tmpDouble;
						break;
				}
				
				xtodom(WorkArea, fld->FLDlen, fld->TDFtype, buff_src);
				
				fld->FLDstat |= fld_REC_LOADED;		// 0x0001u data is now in TTptr->Workarea for this field	
			}		
		}
		WorkArea += fld->FLDlen;          // Bump workarea pointer to next field
	}
}

#endif
