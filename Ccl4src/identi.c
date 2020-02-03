#ifndef IDENTIFY_C
#define IDENTIFY_C

#include "DBdefs.h"
#include "cl4.h"

// *** not public *** in libcl4
int _identify(int DBno, char *TableName, int Lock)
{
	DBinfo	*DBptr;
	TDinfo	*TDptr;
	TDef	*tdef;
	int		result;
	int		TDno;
	TD0REC	TD0;		// struct to take Table 0 data

	//printf("_identify(DBno: x%04X (%3d),TableName: %s , Lock: %d )\n",DBno,DBno,TableName,Lock);
	DBptr = _getattrib(DBno, TableName, &TD0);
	if ( DBptr && (tdef = _getshape(DBptr, TD0.IndexOff)) != 0 )
	{
		TDptr = (TDinfo *)ealloc((_keysize(tdef) + sizeof(TDinfo)), 0);	// zaps memory to ZERO. lot of what follows is unnecessary
		TDptr->TDDBinfo     = DBptr;
		TDptr->TDindexOff   = TD0.IndexOff;
		TDptr->HalfPageSize = TD0.HalfPageSize;
		TDptr->Rec_plus_DB  = TD0.RecPlusDBpagesize;
		TDptr->TDFlags      = TD0.KeyFlags | 9;		// 0x0008 | 0x0001 ?

		TDptr->TDKeySize = _keysize(tdef);
		TDptr->TDKeyMin  = _keymin(tdef);
		TDptr->TDRecSize = _rowsize(tdef);
		TDptr->TDRecMin  = _rowmin(tdef);

		if ( TDptr->TDRecMin != TDptr->TDRecSize )
			TDptr->TDFlags |= tdp_RecVar;			// 0x0040u Record has string entries -> Variable record size
		
		if ( TDptr->TDKeyMin != TDptr->TDKeySize )
			TDptr->TDFlags |= tdp_KeyVar;			// 0x0080u Key has string entries -> Variable Key size

		TDptr->TableDefs   = tdef;					// Variable length array of Table entry definitions
		TDptr->TDNodePtr   = 0;
		TDptr->TDKeyDefs   = (char *)&TDptr[1];     // Key data goes at end of main record
		TDptr->KeyDefSize  = 0;
		TDptr->KeyBuf1     = 0;
		TDptr->Key1Size    = 0;
		TDptr->KeyBuf2     = 0;
		TDptr->Key2Size    = 0;
		TDptr->TDSallocBuf = 0;
		TDno = _nxttd(TDptr);						// Insert TDptr into array, reference by TDno in future

		if ( TDno >= 0 && (!Lock || _lockr(DBptr, TDptr->TDindexOff, 0, 0, 2)) )	// Calling _lockr with no Keydefs, no keysize, lock mode(2).
		{
			result = TDno;
		}
		else
		{
			_rmtd(TDno);
			nfree(TDptr->TableDefs, 0);
			nfree(TDptr, 0);
			result = (TDno >> 31) | 0xFFFFFFF6;
		}
	}
	else
	{
		result = -1;
	}
	return result;	// TDno if found
}

// This is a hack! Should be called through xenter()

int identify(int DBno, char *TableName)
{
	//printf("identify(DBno: x%04X (%3d),TableName: %s)\n",DBno,DBno,TableName);
	return _identify(DBno, TableName, 0);
}

int lidentify(int DBno, char *TableName)
{
	//printf("lidentify(DBno: x%04X (%3d),TableName: %s)\n",DBno,DBno,TableName);
	return _identify(DBno, TableName, 1);
}

#endif
