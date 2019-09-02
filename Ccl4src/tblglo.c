#ifndef TABLEGLOBAL_C
#define TABLEGLOBAL_C

#include "DBdefs.h"
#include "cl4.h"

// _tbl[3005] is not public in libcl4!

TDinfo* _chktd(unsigned int TDno)
{
	//printf("_chktd(%3d)\n", TDno);
	if ( TDno > 3003 || !_tbl[TDno] )
		derror(35, 0, 0);
	return _tbl[TDno];
}

signed int _nxttd(TDinfo *TDptr)
{
	short TDno; // edx@1

	TDno = 3;
	do
	{
		if ( !_tbl[TDno] )
		{
			_tbl[TDno] = TDptr;
			return TDno;
		}
		TDno++;
	}
	while ( TDno <= 3003 );
	return -1;
}

TDinfo* _rmtd(int TDno)
{
	TDinfo *TDptr; // ecx@1

	TDptr = 0;
	if ( (unsigned int)(TDno - 3) <= 3000 )
	{
		TDptr = _tbl[TDno];
		_tbl[TDno] = 0;
	}
	return TDptr;
}

inline TDef* tblrow(unsigned int TDno)
{
	TDinfo *TDptr;
	TDef *result;
	
	if ( TDno <= 3003 && ((TDptr = _tbl[TDno]) != 0) )
		return TDptr->TableDefs;
	else
		return 0;
}

bool _chkname(DBinfo *DBptr,TD0REC *Workarea)
{
	short TDno;
	TDinfo *TDptr;

	//printf("_chkname( DBptr: x%08X, TD0REC: x%08X)\n", DBptr, Workarea);
	for ( TDno = 0; ; TDno++)
	{
		if ( TDno > 2 )
			return false;
		if ( cmpstr(Workarea->TableName, sname_0[TDno]) )// System  Table names: "tables","fields","locks"
			break;
	}
	if ( TDno > 2 || TDno == 2 && DBptr->DBvflag <= 3 )	// legacy support for old CL3 dbases
		return false;

	//printf("_chkname: TDno = %d",TDno);
	TDptr = def_1[TDno];
	Workarea->IndexOff = TDptr->TDindexOff;
	Workarea->KeyFlags = TDptr->TDFlags & 0xD;			//   0000 1101
	Workarea->HalfPageSize = DBptr->DBpgsize / 2;
	Workarea->RecPlusDBpagesize = TDptr->TDRecSize + DBptr->DBpgsize;
	return true;
}

int _shuttd(DBinfo *DBptr)
{
	short TDno;
	int result;
	
	TDno = 3;
	do
	{
		if ( _tbl[TDno] )
		{
			if ( _tbl[TDno]->TDDBinfo == DBptr )
				release(TDno);
		}
		result = TDno + 1;
		TDno++;
	}
	while ( TDno <= 3003 );
	return result;
}

short nr2td(TDinfo *TDptr)
{
	short TDno; // eax@1

	TDno = 0;
	while ( _tbl[TDno] != TDptr )
	{
		TDno++;
		if ( TDno > 3003 )
			return -1;
	}
	return TDno;
}

#endif
