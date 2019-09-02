#ifndef PCREATE_C
#define PCREATE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

int mktable(int *TTnos, short NumTTs, short *KeyIDs, short *DataIDs, short *a5);

bool _tdopen(int DBno, TDesc *TTptr)	// Only called by pcreate()
{
    short v13; // ax@2
    short NumFields; // esi@6
    short size; // edx@6
    short NameLen; // kr04_4@16
    int TDno; // [sp+24h] [bp-2044h]@8
    
	TDef *tdef;
    TDef *tdef2;
	FLDdesc *fld;

	char Dest[32];
    ELEMENTREC elem;

	//printf("_tdopen(%d, x%08X)\n", DBno, TTptr);
    if ( no_ttabs - 1 >= TTptr->TTrtd )
    {
        tdef = tblrow(TTptr->TTrtd);
        if ( tdef )
        {
            tdef2 = tdef;
			NumFields = 0;
            for ( size = 0; tdef2->TDFentry; tdef2++ )
            {
                size += tdef2->TDFsize;
                NumFields++;
            }
            TTptr->TDworkarea1	= (char *)mmalloc(size + 1);
            TTptr->NumFields	= NumFields;
            TTptr->TTfields		= (FLDdesc *)mmalloc(sizeof(FLDdesc) * (NumFields + 1));				// null terminated array

            if ((TDno = identify(DBno, "elements")) >= 0 || (TDno = identify(DBno, "domains")) >= 0 )	// "domains" in pre-v4 dbases?
            {
                tdef2 = tdef;
                for ( fld = TTptr->TTfields; tdef2->TDFentry; tdef2++ )
                {
                    Dest[0] = 0;
                    if ( TDno > 0 )
                    {
                        itoms(&elem.TDFentry, tdef2->TDFentry);
                        v13 = getr(TDno, (char *)&elem, 0);
                        if ( v13 <= 0 )
                        {
                            Dest[0] = 0;							// error, or couldn't find. blank name buffer.
                            if ( v13 < 0 )
                                dberror(v13, 0, TDno);
                        }
                        else
                        {
                            elem.FLDlen = 0;						// ensure FieldName string terminated
                            cdbcpystr(Dest, elem.FieldName, 0);
                        }

                        NameLen = strlen(Dest) + 1;
                        if ( NameLen != 1 )							// true if not a blank string (should always be true in clenter)
                        {
                            fld->FLDname = (char *)mmalloc(NameLen);
                            cpybuf(fld->FLDname, Dest, NameLen);
                        }
                    }
                    
					fld->FLDtype = gettype(tdef2->TDFtype, tdef2->TDFsize);
					switch (fld->FLDtype)
					{
					case 'C':
                        fld->FLDdata = mmalloc(tdef2->TDFsize + 1);		// strings are variable length
						break;
					case 'S':											// do nothing. Keys and Subrecs have no data field
					case 'K':
						break;
					default:
						fld->FLDdata = mmalloc(sizeof(double));			// mmalloc(8) all other var types stored as doubles internally
						break;
					}

                    fld->FLDstat |= fld_ZERO;				// 0x0004;	quick zero flag
					if ( tdef2->TDFtype & tdf_KEY )			// Key field
                        fld->FLDstat |= fld_KEY;			// 0x8000u;
                    else
                        fld->FLDstat &= ~fld_KEY;			// 0x7FFFu;

                    fld->FLDelemID	= tdef2->TDFentry;
                    fld->FLDlen		= tdef2->TDFsize;
                    fld->TDFtype	= tdef2->TDFtype;
                    ++fld;
                }
                v13 = release(TDno);
                if ( v13 < 0 )
                    dberror(v13, 0, TDno);
                
				return true;	// exit_success - table opened ok.
            }
        }
    }
    else
    {
        fwrite("too many tables open\n", 1u, 0x15u, stderr);
        v13 = release(TTptr->TTrtd);
        if ( v13 < 0 )
            dberror(v13, 0, TTptr->TTrtd);					// dead end error handler
    }
    return false;
}


void pcreate(KXTAB *kxtab)
{
    int		*TTnos;
	void	*ptr;
	TDesc	*TTptr; // [sp+24h] [bp-14h]@1
    EXPR	*expr;
	
	int		DBno;
	int		v9;
    short	v2;
	short	v4;
	short	v7;
	
    tdclose(kxtab->TTno);								// close if already open!
    TTptr	= &ttab[kxtab->TTno];
    cdbcpystr(TTptr->TableName, kxtab->TableName, 0);
    TTnos	= (int *)mmalloc(4 * kxtab->NumTTs);

	v2 = kxtab->NumTTs - 1;
    while ( v2 >= 0 )
    {
		TTnos[v2] = rtdget(&ttab[kxtab->TTlist[v2]]);
        v2--;
    }

    v4 = mktable(TTnos, kxtab->NumTTs, kxtab->KeyElemIDs, kxtab->DataElemIDs, kxtab->TkeyTy);
    if ( v4 >= 0 )
    {
        TTptr->TTrtd = v4;
        DBno = rdbget(TTptr->DBnumber);
        if ( _tdopen(DBno, TTptr) )		// _tdopen(), *not* tdopen()!!
        {
            TTptr = &ttab[kxtab->TTno];
            TTptr->TDlocked |= (ttl_OPEN | ttl_CREATE);				// 0xA00u; 0800|0200
            kxtab->KXFlags |= 0x800u;								// ??? doesn't appear to ever be read back ???
			// kxf_EMPTY == 0x1000 "empty" flag
            if ( kxtab->KXFlags & kxf_EMPTY || ((expr = getexpr(ENARR(kxtab->WhereEXP), 0), ptr = mmalloc(_ldsize), (kxtab->KXFlags & 0x0400)) 
					? (v7 = compose(ptr, _ldsize, TTptr->TTrtd, TTnos, kxtab->NumTTs, expr)) 
					: (v7 = reorder(ptr, _ldsize, TTptr->TTrtd, TTnos, kxtab->NumTTs, expr)),
					mfree_0(ptr), fretre(expr, 0), v7 >= 0) )
				v9 = 0;												// success
            else
                v9 = ((kxtab->KXFlags & kxf_UNIQUE) != 0) + 13;		// 0x0400 == "unique" flag
            setvar(v_ERROR, v9);
        }
        else
        {
            setvar(v_ERROR, 12);
        }
        mfree_0(TTnos);						// memory leak if mktable() fails?
    }
    else
    {
        setvar(v_ERROR, 11);
    }
}

#endif

