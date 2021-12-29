#ifndef GETSHAPE_C
#define GETSHAPE_C

#include "DBdefs.h"
#include "cl4.h"

unsigned short maptype(short a1)
{
	unsigned short result;

	result = a1 & 0x0003;			// preserve KEY/STRING flag bits
	//if ( (char)a1 >= 0 )	// **don't think this is correct
	if ((a1 & 0x80u) == 0 )
	{
		if ( a1 & 0x0200 )
			result |= 0x0044u;		// INT + allow signed
		
		else if ( a1 & 0x0040 )
			result |= 0x0200u;		// ???
		
		else if (a1 & 0x0400 )
			result |= 0x0018u;		// 
	}
	else
		result |= 0x0084u;			// ???
	return result;
}

unsigned short map41type(short a1)
{
	unsigned short result;

	result = a1;
	if ( a1 & 0x4000 )						// clear:					set
		result = a1 & 0xBFFF | 0x3000;		// 1011 FFFF FFFF FFFF		0011 0000 0000 0000
	return result;							// x3000 = 'K'	Key Table
}

TDef* ldshape(DBinfo *DBptr, short TDno)
{
	TDinfo	*TDptr;
	FIELD	*field_p;
	FIELD	*field_head_p;
	EXPR	*Expr;

	TDef	*tdef;
	TDef	*tdef_start;

	short	v6;
	short	TDFtype;
	short	NumFields;
	short	ExprData;

	TDptr = _chktd(td_fields);					// "fields" TDno #1
	TDptr->TDDBinfo = DBptr;
	
	if ( DBptr->DBvflag == 2 )					// version 2
		TDptr->TableDefs = _xfshape;			// pre-defined shape tables for system dbases
	else
		TDptr->TableDefs = _fshape;

// ------------
	ExprData = (unsigned char)TDno | ExprData & 0xFF00;
	Expr = newexpr(106, 0x10u, (char *)&ExprData, 1u);	// 106 == part of Key in 'fields' ** see below ** 0x10 == "equal to"
	cdbrestrict(td_fields, Expr);						// apply expression to fixed 'fields' table
	
// Read all the entries for this TDno
	field_p = (FIELD *)alloc(sizeof(FIELD), 0);
	field_head_p = field_p;
	for ( NumFields = 0; ; NumFields++ )
	{
		v6 = nxtr(td_fields, (char *)field_p, My_pos_0, Expr);		// My_pos_0 maps TDno1 to FIELDS struct
		if ( v6 <= 0 )	// error or no more entries
			break;
		field_p->NextFIELD = (FIELD *)alloc(sizeof(FIELD), 0);
		field_p = field_p->NextFIELD;
	}
	
	fretre(Expr, 0);
//-------------
	tdef_start = 0;
	if ( v6 >= 0 ) // <0 if read error above
	{
		tdef_start = (TDef *)alloc(sizeof(TDef) * (NumFields + 1), 0);	// null terminated TDef array
		field_p = field_head_p;
		for ( tdef = tdef_start; field_p->NextFIELD; ++tdef )
		{
			tdef->TDFentry = field_p->TDFentry;
			tdef->TDFsize = field_p->TDFsize;

			if ( DBptr->DBvflag == 2 )				// Do some tricky re-mapping of the TDFtype value, depending on DBase version
				TDFtype = maptype(field_p->TDFtype);
			else if ( DBptr->DBversion == 1765 )
				TDFtype = map41type(field_p->TDFtype);
			else
				TDFtype = field_p->TDFtype;			// CL4 DBases are version 1766, so have *no* re-mapping of types
			
			tdef->TDFtype = TDFtype;
			field_p = field_p->NextFIELD;
		}
		tdef->TDFentry = 0;	// mark end of the TDef table with a ZERO entry
	}
	frelst(field_head_p, 0);
	return tdef_start;	// return pointer to start of TDef table
}

//-----

TDef *_getshape(DBinfo *DBptr,short TableIdx)
{
	TDef *result;

	result = _chkindex(TableIdx, DBptr);	// look for a match on a reserved system table
	if ( !result )
		result = ldshape(DBptr, TableIdx);	// otherwise read in shape table from DBASE.
	return result;
}

#endif
/*
-> d fields
dbase [/home/leslie/Cclenter/TESTDB]  table < fields >
 No. Type  Description
[ 1]  *B   106
[ 2]  *B   101
[ 3]   I   102
[ 4]   I   103
[ 5]   I   104
key = [ 2] record = [8]

Example:
[ 1]  *B  106                  : 26			: Table Number in DBASE
[ 2]  *B  101                  : 3			: Field Number
[ 3]   I  102                  : 29655		: Pointer to item in "elements" table
[ 4]   I  103                  : 68			: Variable type
[ 5]   I  104                  : 2			: Variable Size (in bytes)

-> g /home/leslie/Cclenter/TESTDB elements
-> l elements w 1 = 29655
[ 1]  *I  30003                : 29655
[ 2]   A  30005                : Y_Coord
[ 3]   I  30013                : 6
[ 4]   A  30007                :
[ 5]   A  30011                :
[ 6]   A  30012                : n

*/