#ifndef MAKE_TABLE_C
#define MAKE_TABLE_C

#include <stdlib.h>		// for mkstemp()
#include <unistd.h>		// for unlink()
#include <stdarg.h>		// for var args stuff
#include <string.h>
#include <errno.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

TDef *scntbl(int *TTnos, short NumTTs, short TDFentry)
{
    TDef *tdef; // eax@2

//print("scntbl(x%08X, %d, %d [x%04X])\n",TTnos,NumTTs,TDFentry,TDFentry);
	while ( NumTTs > 0 )
    {
		tdef = tblrow(*TTnos);
        if ( tdef )
        {
			while (tdef->TDFentry)				// nil terminated array
			{
				if (tdef->TDFentry == TDFentry)
					return tdef;					// exit_success   found a match
				tdef++;
			}
		}
        ++TTnos;
        --NumTTs;
	}
    return 0;		// error_exit  not found
}

TDef *newdesc(int *TTnos, short NumTTs, short *KeyIDs, short *DataIDs, short *TDFtypes)
{
    TDef	*tdef;
	TDef	*v10;
	TDef	*v17;
	
    short	*j;
	short	NumKeyIDs;
	short	NumDataIDs;
	
    short	*v9;
	short	*Types;
	
    for ( j = KeyIDs; j && *j; ++j )
        ;
    NumKeyIDs = j - KeyIDs;
    for ( j = DataIDs; j && *j; ++j )
        ;
    NumDataIDs = j - DataIDs;					// pointer arithmetic

    tdef = (TDef *)ealloc(sizeof(TDef) * (NumDataIDs + NumKeyIDs + 1), 0);					// 6 = sizeof(TDef)
    v17 = tdef;									// v17 is returned at the end

	// Copy Key fields to TDef Array first
    Types = TDFtypes;
    for ( v9 = KeyIDs ; v9 && *v9; ++v9 )		
    {
        v10 = scntbl(TTnos, NumTTs, *v9);
        if ( v10 )
        {
            tdef->TDFentry	= v10->TDFentry;
            tdef->TDFsize	= v10->TDFsize;
            tdef->TDFtype	= v10->TDFtype | tdf_KEY;	// 0x0001
			if ( Types && *Types == 0x4000 )			// "TDFtypes" Array allows overide of default TDFtype value. (Key/non-Key etc.)
                tdef->TDFtype |= 0x4000;
            else
                tdef->TDFtype &= 0xBFFF;
            tdef++;

			if (Types)			// \0 can be passed for the "TDFtypes" pointer. 
				Types++;
        }
    }

	// Copy extra Data fields to TDef Array last
    for ( v9 = DataIDs; v9 && *v9; ++v9 )	
    {
        v10 = scntbl(TTnos, NumTTs, *v9);
        if ( v10 )
        {
            tdef->TDFentry	= v10->TDFentry;
            tdef->TDFsize	= v10->TDFsize;
            tdef->TDFtype	= v10->TDFtype & 0xBFFE;	// 1011 1111 1111 1110   mask out: 0x4001
            tdef++;
        }
    }
    tdef->TDFentry = 0;	// nil terminate the TDef array
    return v17;
}

DBinfo *newfdf(int TDno)
{
    short v1; // ax@1
    unsigned int v2; // edi@3
    int i;
    signed int v4; // edx@4
    signed int v5; // edx@6
    char v6; // al@6
    unsigned int j; // esi@9
    signed short v8; // ax@10
    int v9; // edi@12
    signed int v10; // esi@15
    unsigned int v12; // [sp+18h] [bp-420h]@3
    DBinfo *DBptr; // [sp+1Ch] [bp-41Ch]@1
    char Buffer[1024]; // [sp+20h] [bp-418h]@3

    DBptr = (DBinfo *)ealloc(sizeof(DBinfo), 0);
    DBptr->DBpgsize = _psize;				// careful!!!   _psize, not psize
    v1 = _mktmp();
    DBptr->DBfilehandle = v1;
    if ( v1 < 0 )
        derror(47, 0, 0);
    DBptr->DBmaxpages = _fsize;
    DBptr->DBvflag = 3;
    DBptr->DBversion = 1766;
    DBptr->DBmaxtables = 1;
    DBptr->PageList2[0] = 0;
    DBptr->PageList3[0] = 0;
    DBptr->PageList1[0] = 0;
    DBptr->DBopenflg = -1;
    DBptr->DBtableaddress = 16;
    _whead(DBptr, 0, 0, 0);
    fill(Buffer, sizeof(Buffer), 0);
//print("newfdf - _fsize = %d, _psize = %d\n",_fsize, _psize);
    v2 = (unsigned int)(_fsize + 7) >> 3;        // default DBsize = 500000
    v12 = (_psize + v2 + 31) / _psize;            // default pagesize = 4096
//print("newfdf - v2 = %d, v12 = %d\n",v2,v12);
    for ( i = v12 + 31; i >= 0; --i )           // preset the volume bit map
    {
        v4 = i + 7;
        if ( i > -1 )
            v4 = i;

        v5 = v4 >> 3;		// div by 8
        v6 = i + 7;
        if ( i > -1 )
            v6 = i;

        Buffer[v5] |= (signed int)0x80u >> (i - (v6 & 0xF8));
    }

    DBptr->DBfreeaddress = 32;
    lseek64(DBptr->DBfilehandle, 32LL, 0);
    for ( j = v2; (signed int)j > 0; j -= v9 )
    {
        v8 = 1024;
        if ( j < 1025 )
            v8 = j;
        v9 = v8;
        if ( write(DBptr->DBfilehandle, Buffer, v8) != v8 )
            derror(31, 0, 0);
        fill(Buffer, sizeof(Buffer), 0);
    }

    v10 = 0;
    do
    {
        DBptr->SpareList[v10] = v12 + v10;
        ++v10;
    }
    while ( v10 <= 32 );
    DBptr->SpareList[32] = 0;
    return DBptr;
}


int _mktable(int *TTnos, short NumTTs, short *KeyIDs, short *DataIDs, short *a5)
{
    TDef *tdef; // edi@1
    int v6; // ST18_4@1
    TDinfo *TDptr; // esi@1
    int v8; // eax@1
    int v9; // eax@3
    int TDno; // edi@5
    int result; // eax@6
    DBinfo *v12; // eax@7

    tdef = newdesc(TTnos, NumTTs, KeyIDs, DataIDs, a5);
    v6 = _keysize(tdef);
    TDptr = (TDinfo *)ealloc(v6 + sizeof(TDinfo), 0);	// variable size record
	fill((char *)TDptr, sizeof(TDinfo), 0);			// Zeroes structure, so lot of what follows is unnecessary!

    TDptr->TDFlags			= 0x0088u;			// 0000-0000-1000-1000
    TDptr->TDindexOff		= 0;
    TDptr->TableDefs		= tdef;
    TDptr->TDRecSize		= _rowsize(tdef);
    TDptr->TDKeySize		= v6;
    TDptr->TDKeyDefs		= (char *)&TDptr[1];	// Key data stored after main record
    TDptr->KeyDefSize		= 0;
    v8 = _rowmin(tdef);
    TDptr->TDRecMin			= v8;
    if ( v8 != TDptr->TDRecSize )
        TDptr->TDFlags |= tdp_RecVar;	// 0x40u;	// Indicates record contains strings, so may vary in length
    v9 = _keymin(tdef);
    TDptr->TDKeyMin			= v9;
    if ( v9 != TDptr->TDKeySize )		// Indicates Key fields include strings, so may vary in length
        TDptr->TDFlags |= tdp_KeyVar;	// 0x80u;	// already set above in TDptr->TDFlags = 0x0088u;  ??
    TDptr->TDNodePtr		= 0;
    TDptr->KeyBuf1			= 0;
    TDptr->Key1Size			= 0;
    TDptr->KeyBuf2			= 0;
    TDptr->Key2Size			= 0;
    TDptr->TDSallocBuf		= 0;
    TDno = _nxttd(TDptr);
    if ( TDno >= 0 )
    {
        v12					= newfdf(TDno);
        TDptr->TDDBinfo		= v12;
        TDptr->Rec_plus_DB	= v12->DBpgsize;
        TDptr->HalfPageSize = TDptr->TDRecSize;
        result = TDno;
    }
    else
    {
        nfree(TDptr->TableDefs, 0);
        nfree(TDptr, 0);
        result = -1;
    }
    return result;
}

int mktable(int *TTnos, short NumTTs, short *KeyIDs, short *DataIDs, short *a5)
{
	//return xenter((int)mktable);
	return _mktable(TTnos, NumTTs, KeyIDs, DataIDs, a5);

}

#endif
