#ifndef CMPATTRIBUTE_C
#define CMPATTRIBUTE_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short _cmpattrib(char *src, short TDFsize, char *Buffer, short TDFsize2, short TDFtype)
{
	char	*v5;
	signed int v16;
	short	v6;
	short	v13;
	short	v14;
	short	v18;
	
	unsigned char v9;
	unsigned char v10;
	unsigned char v11;
	char v12;
	bool isNum;
	char dest[1048];

	//printf("_cmpattrib(src = x%08X, TDFsize = %2d, Buffer = x%08X, TDFsize2 = %2d, TDFtype = x%04X)\n",src,TDFsize,Buffer,TDFsize2,TDFtype );

	v16 = 0;
	isNum = false;
	if ( TDFtype & 0x4000 )	// ???
	{
		v5 = src;			// swap src<->dest values , sizes
		src = Buffer;
		Buffer = v5;
		
		v6 = TDFsize;
		TDFsize = TDFsize2;
		TDFsize2 = v6;
	}

	if ( TDFtype & tdf_SIGNED && TDFsize && TDFsize2 && (*Buffer ^ *src) < 0 )	// 0x0004 allow negative values
	{
		if ( *src >= 0 )
			v18 = 1;
		else
			v18 = -1;
		//printf("_cmpattrib: v18 = %d\n",v18);
	}
	else
	{
		if ( TDFtype & tdf_STRING && (getwpos(Buffer, TDFsize2) >= 0))		// 0x0002 = string value, and with a wildcard char
		{
			//printf("_cmpattrib[51]: \n");
			strncpy(dest, src, TDFsize);
			dest[TDFsize] = 0;
			return owildmat(dest, Buffer, TDFtype);
		}
		else
		{
			//printf("_cmpattrib[59]: \n");
			v18 = 0;
			while ( TDFsize && TDFsize2 )
			{
				v9 = *src;
				v10 = *Buffer;
				v18 = (v9 - v10);
				//printf("cmpattr_1: TDF1 %d , TDF2 %d, v9 = %d [%c], v10 = %d [%c], v18 = %d\n",TDFsize,TDFsize2,v9,v9,v10,v10,v18);
				if ( v9 != v10 )
				{
					if ( !(TDFtype & tdf_NOCASE))		// allow case-insentive match
						break;
					
					v11 = v9;
					if ((v9 - 0x41u) <= 0x19)
						v11 = v9 + 0x20;		// tolower()
					//printf("cmpattr_1: v11 = %d x%02X [%c]\n",v11,v11,v11);
					
					v12 = v10;
					if ((v10 - 0x41u) <= 0x19)
						v12 = v10 + 0x20;		// tolower()
					//printf("cmpattr_1: v12 = %d x%02X [%c]\n",v12,v12,v12);
					
					v12 = v11 - v12;
					//printf("cmpattr_1: v11 - v12 = %d\n",v12);
					v18 = v12;
					if ( v12 )	// ZERO if they match
						break;
				}
				if ( TDFtype & tdf_STRING && !v9 )
					break;
				isNum = (v9 - 48) <= 9;		// 48 = '0'
				++src;
				++Buffer;
				TDFsize2--;
				TDFsize--;
			}
		}
	}
	
	if ( TDFtype & 0x10 )	// allow comparison of numbers stored as strings
	{
		//printf("_cmpattrib[97]: \n");
		v13 = numsz(src, TDFsize);
		v14 = numsz(Buffer, TDFsize2);
		if ( v13 != v14 )
		{
			if ( isNum || v13 && v14 )
				v18 = v13 - v14;
		}
	}
	//printf("_cmpattrib[105]: returning %d\n",v18);
	return v18;
}

#endif
