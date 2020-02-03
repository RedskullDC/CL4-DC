#ifndef GETTFEXP_C
#define GETTFEXP_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for strtod() strlon()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"            // for flag bits

short gettfexp(short entn)			// Get Table/Field Expression
{
	FLDdesc *fld;
	ENTAB	*v14;
	ENTAB	*entab;
	
    int		TDno;
	int		MemSize;
	int		v2;
	short	FieldNo;
	short	v4;
	short	ENTno;

    char	TableBlock[64];
	char	TableName[64];
	char	FLDtype;

	// when entering this routine, symbol is pointing at a known variable/table name as an lvalue.
	
    fld = 0;
    cdbcpystr(TableName, sym, 0);
    MemSize = (signed short)(syml + 1);
    symbol = getsym();

	if ( symbol == 1080 )            // "." This is a table.field specifier. Expect a fieldname to follow
	{
        TDno = findtd(TableName, -1);
        if ( TDno < 0 )
		{
            loaderr(14, TableName);			// "table not open"
			return 0;
        }
        symbol = getsym();					// sym now holds the FieldName string		
        cdbcpystr(TableName, TableName, ".", sym, 0);
        FieldNo = findfno(TDno, sym);
        v2 = FieldNo;
        if ( FieldNo < 0 )
        {
			loaderr(8, TableName);		// "invalid field name or number"
			return 0;
        }
        FLDtype = ttab[TDno].TTfields[FieldNo].FLDtype;
        if ( FLDtype == 'S' || FLDtype == 'K' )// Error!!!
		{
            loaderr(24, TableName);		// "invalid operation on 'set' or 'key' field"
			return 0;
        }
		symbol = getsym();				// advance to fieldname
	}
    else // This is a normal variable specifier
	{
        sprintf(TableBlock, "%s#%d", TableName, cur_block);
        troot = gettnode(troot, TableBlock, strlen(TableBlock) + 1, 0);
        if ( !tnodewasfound )
			troot = gettnode(troot, TableName, MemSize, 1);
        v2 = last_fnd;
        fld = getvars(last_fnd);
        if ( !fld )						// variable not defined, or trying to access it *before* it is defined
		{
            fld = getvarmem();						// create it anyway for debug purposes?
			assert(fld == getvars(v2));				// should exist now
            fld->FLDname = getmem(MemSize);			// size of Variable Name + '\0'
            cdbcpystr(fld->FLDname, TableName, 0);
            loaderr(30, TableName);					// "undefined variable"
			return 0;
        }
        TDno = 0;						// Normal variables hang off TD0
		fld->FLDstat |= fld_VAR_USED;	// 0x0010u;			// Flag to indicate variable has been accessed in program
    }

	if ( symbol == 740 )                    // "[" - Array subscript follows
    {
		symbol = getsym();
        allenpt(entn, &entab);
        entab->Enun.Enop.Enoper		= f_ARRSTART;		// 0xAC00u Array de-reference operator.
        entab->entype				= 2;				// Array expression
		ENTno = getenmem();					// Don't remove this.
        entab->enleft				= ENTno;			// getenmem can re-locate entab!!
        allenpt(entab->enleft, &v14);
        v14->Enun.Enref.TTno		= TDno;
        v14->Enun.Enref.VarNum		= v2;				// FieldNo or Variable Number
        v14->entype					= 1;
        dallenpt(&v14);
        
		if ( !TDno && !fld->FLDelemID )		// ElemID holds number of subscripts. If \0, not an Array variable 
			loaderr(69, TableName);         // "not an array"

		v4 = loadexp(0, 1);					// not in conditional, and we want an integer result
        if ( !v4 )
        {
			dallenpt(&entab);
            return 0;
		}
        entab->enright = v4;
        dallenpt(&entab);
        if ( symbol != 1310 )               // "]"
        {
			if ( symbol == 930 )
				loaderr(7, sym);			// "unexpected end of line"
			else
				loaderr(33, sym);			// "unexpected symbol"
            return 0;
        }
		symbol = getsym();
    }
    else	// Not an array, update the ENTAB
    {
		entab = ENARR(entn);
		entab->Enun.Enref.TTno		= TDno;
        entab->Enun.Enref.VarNum	= v2;				// FieldNo if a Table, or Variable number
        entab->entype				= 1;				// Normal variable expression
	}
    return entn;		// exit success
}

#endif
