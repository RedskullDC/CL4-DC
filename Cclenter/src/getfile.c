#ifndef GETFILE_C
#define GETFILE_C

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int tgetfile(TDesc *TTptr)
{
    char	*v4;
    int		result;

    result = rdbget(TTptr->DBnumber);	// Ensure that corresponding DB is already open
    if ( result >= 0 )					// result == DBno
    {
        result = tdopen(result, TTptr, ((unsigned int)TTptr->TDlocked >> 6) & 1);	// 0100 0000   bit 7 = lock required flag?
        if ( result < 0 )
        {
            v4 = TTptr->TableName;
            if ( result == -10 )     // -10 == LOCKED
                eprint("table locked - %s\n", v4);
            else
            {
                if ( !cmpbuf(v4, "domains", 6) && !cmpbuf(v4, "elements", 8))	// don't bomb if can't find. At least ONE MUST exist
					eprint("can't identify - %s (err=%d)\n", v4, result);
            }
        }
    }
    return result;	// <0 == ERROR code, >= 0 == TDno (success)
}

int tdopen(int DBno, TDesc *TTptr, int Lock)
{
    TDef	*tdef;
    TDef	*v16;
    FLDdesc *fld;

    unsigned int result;
    int		v10;
    int		ErrCode;
    int		TDno;
    int		RecSize;
    int		NumFields;
    short	v5;
    short	CRC;
    char	FieldName[32];
    ELEMENTREC elem;

    //if ( TTptr->TTrtd != -1 )
    //    __assert_fail("tt->runtd == -1", "getfile.c", 0x6Cu, "tdopen");
	assert(TTptr->TTrtd == -1);

	if ( Lock ) 
	    result = lidentify(DBno, TTptr->TableName);
	else
		result = identify(DBno, TTptr->TableName);

	//printf("tdopen - TTptr->TableName = %s, result = %d\n",TTptr->TableName, result);
	v5 = result;
    if ( (result & 0x80000000u) == 0 )	// if result >= 0 ?
    {
        v16 = tblrow(result);
        result = -1;
        if ( v16 )
        {
            TTptr->TTrtd = v5;
			//================ do Table CRC calcs ======================
            CRC = 0;
            tdef = v16;		// save tdef* for later
            NumFields = 0;
            for ( RecSize = 0; tdef->TDFentry; tdef++ )
            {
                RecSize += tdef->TDFsize;
                CRC = (unsigned short)pcrc(CRC, tdef, 2, 68u, 3u);
                NumFields++;
            }
            if ( !CRC )
                CRC = 1;
            if ( TTptr->TableCRC && TTptr->TableCRC != CRC)
            {
				eprint("table definition incompatibility (calc=%u, got=%u) - %s\n", CRC, TTptr->TableCRC, TTptr->TableName);
				return -1;
            }
            else
                TTptr->TableCRC = CRC;

			//==========================================================

            TTptr->TDworkarea1	= (char *)mmalloc(RecSize + 1);
            TTptr->TDworkarea2	= (char *)mmalloc(RecSize + 1);
            TTptr->NumFields	= NumFields;
            
			TDno = identify(DBno, "elements");		// DB version 4	
            if ( TDno < 0 )
                TDno = identify(DBno, "domains");	// DB version 3 or less? 

			TTptr->TTfields = (FLDdesc *)mmalloc(sizeof(FLDdesc) * (NumFields + 1));	// sizeof(FLDdesc) == 24
            tdef = v16;
            for ( fld = TTptr->TTfields; tdef->TDFentry; tdef++ )
            {
                fld->FLDtype = gettype(tdef->TDFtype, tdef->TDFsize);			// VarType 

				if ( fld->FLDtype == 'C' )			// String
                {
                    v10 = tdef->TDFsize + 1;		// allow for \0 terminator
                }
                else if ( tdef->TDFsize )			// all other variables default to double
				{
					v10 = sizeof(double);			// 8; 
				}
				else	// 'K' or 'S' records have no size
				{
					v10 = 0;
					FieldName[0] = 0;
					if ( TDno > 0 )		// neither elements or domains can be table ZERO
					{
						itoms(&elem.TDFentry, tdef->TDFentry);
						ErrCode = getr(TDno, (char *)&elem, 0);
						
						if ( ErrCode < 0 )
							dberror(ErrCode, 0, TDno);					// error_exit
						else if (!ErrCode )
							sprintf(FieldName, "%d", tdef->TDFentry);	// no name rec found, use DOMID instead
						else
							cdbcpystr(FieldName, elem.FieldName, 0);		// found record ok, use text name
						v10 = strlen(FieldName) + 1;
					}
                }
				// printf("FieldName = %s, v10 = %d\n",FieldName,v10);
				// would be handy if *all* variable names were available at run-time....... :o)
                
				fld->FLDdata = mmalloc(2 * v10);	// why twice the size??
                if ( fld->FLDtype == 'S' || fld->FLDtype == 'K' )
                    cdbcpystr((char *)fld->FLDdata, FieldName, 0);	// save the name for sub/keys, but no data

				if ( tdef->TDFtype & tdf_KEY )		// Key field
                    fld->FLDstat |= fld_KEY;

                fld->FLDstat	|= fld_ZERO;		// quick zero flag
                fld->FLDlen		= tdef->TDFsize;
                fld->FLDelemID	= tdef->TDFentry;
                fld->TDFtype	= tdef->TDFtype;
                fld++;
            }
            if ( TDno > 0 )
            {
                ErrCode = release(TDno);
                if ( ErrCode < 0 )
                    dberror(ErrCode, 0, TDno);
            }
            TTptr->TDrecsize = RecSize;
            result = 0;
        }
    }
    return result;
}

int rtdget(TDesc *TTptr)
{
	//printf("rtdget(x%08X) [%s]", TTptr, TTptr->TableName);
	assert(ttab <= TTptr && TTptr < &ttab[no_ttabs]);	// Ensure sanity on TTptr, within current bounds

	if ( TTptr->TTrtd == -1 && tgetfile(TTptr) < 0 )	// opens table if necessary
    {
        eprint("can't open table '%s'\n", TTptr->TableName);
        dberror(-98, TTptr->DBnumber, TTptr->TTrtd);
        mexit(-98);
    }
	assert(TTptr->TTrtd >= 0 && TTptr->TTrtd < no_ttabs);	// Ensure sanity on TTptr-TTrtd, within current bounds
	//printf("rtdget : returning %d\n",TTptr->TTrtd);
    return TTptr->TTrtd;
}

int getfile(char *DBname, char *TableName)	// only called from reffile::replace()
{
    TDesc	*TTptr;
	char	*Alias;
	short	DBno;
	
//printf("getfile  DBname = %s, TableName = %s\n", DBname, TableName);

	for (DBno = 0; no_dtabs > DBno ; DBno++)
	{
		if ( !strcmp(DBname, dtab[DBno].FullDBname))
			break;
    }
	if ( no_dtabs == DBno )				// we got to the end of list with no match. Shouldn't be possible in clenter?
    {
        newdtab();						// bump no_dtabs
		assert(DBno < no_dtabs);
    }
    if ( !dtab[DBno].FullDBname )
    {
        dtab[DBno].DBno			= -1;
        dtab[DBno].FullDBname	= mstrcpy(DBname, 0);
    }
    for ( TTptr = ttab + 3; TTptr < &ttab[no_ttabs]; TTptr++ )	// start at ttab[3] == first user TD
    {
        if ( !TTptr->TableName[0] )
            break;
        if ( !strcmp(TTptr->TableName, TableName) )
            break;
    }
    if ( TTptr == &ttab[no_ttabs] )		// shouldn't be possible in clenter. All tables pre-allocated?
    {
        newttab();						// bumps no_ttabs
        TTptr = &ttab[no_ttabs - 1];
    }
    if ( !TTptr->TableName[0] )
    {
        Alias = TableName;
		while (*Alias)
		{
			if ( *Alias == ',' )
			{
				*Alias++ = 0;	// terminate TableName, and bump Alias to next char
				break;
			}
			Alias++;
		}
        cdbcpystr(TTptr->TableAlias, Alias, 0);		// If there was no Alias specified, Alias points at a \0
        cdbcpystr(TTptr->TableName, TableName, 0);
        TTptr->DBnumber = DBno;
        TTptr->TTrtd	= -1;
    }
    return rtdget(TTptr);
}

int rdbget(int DBno)
{
    DBase	*DTptr;
    char	*DBpath;
    int		PctFree;

    DTptr = &dtab[DBno];
    if ( DTptr->DBno >= 0 )	// This DB is already open
        return DTptr->DBno;
    
    DBpath	= mstrcpy(chkpath(DTptr->FullDBname, 0, "CLDPATH", 0, 0x180u), 0);	// Go searching on the path for the DB , 0x180u [384] == permissions checks
	DBno	= cldbopen(DBpath, 0);												// DBpath is full path to file

	if ( DBno < 0 )
    {
        eprint("can't open database - %s\n", DTptr->FullDBname);
    }
	else
	{
		DTptr->FullDBname	= DBpath;
		DTptr->DBno			= DBno;
		PctFree				= 100 * dbspace(DBno) / dbsize(DBno);
		
		if ( PctFree < 10 )			// DBase has less than 10% space remaining
		{
			if ( isCGI )
				eprint("%s is %d%% full\n", DTptr->FullDBname, 100 - PctFree);		// no BELL for web
			else
			{
				eprint("\a%s is %d%% full\n", DTptr->FullDBname, 100 - PctFree);	// \a = DING!! and wait for 5 secs so they see it!
				sleep(5u);
			}
		}
	}
	return DBno;	// >= 0 == DBno, < 0 == ERROR code
}
#endif
