#ifndef LEGETF_C
#define LEGETF_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

char *checkdbrename(char *DBname)
{
    const char **v1;
	size_t len;
	char *NewDBname;
	const char *Key;
	
    v1 = (const char **)dbarray;
    if ( v1 )
    {
        Key = mstrcpy(DBname, "=", 0);
        len = strlen(Key);
        NewDBname = 0;
		while (*v1)
		{
			if (!strncmp(Key, *v1, len))						// found a match
			{
	            NewDBname = mstrcpy((char *)&(*v1)[len], 0);	// copy string after the '=' sign
				break;
			}
			v1++;
		}
        mfree_0((void *)Key);
        if ( NewDBname )
        {
            //mfree_0(DBname);	// replaces memory pointed to by *DBname  **** different to clenter!! ***
            DBname = NewDBname;
        }
    }
    return DBname;
}

int tdopen(int DBno, char *TableName)
{
	DOMAINR *dom;
	TDesc	*TTptr;
	TDef	*tdef;
	TDef	*v22;
	TDef	*v9;
	Qlen	*qlen;
	FLDdesc *fld;
	char	*j;
	char	*v6;
	
	int		TDno;
	int		TDno2;
	size_t	v13;

	unsigned int v12;
	unsigned short	CRC;
	short	NumFields;
	short	RetCode;
	
	ELEMENTREC elem1;
	ELEMENTREC elem2;
	
	//printf("tdopen() - sizeof(Qlen) = %d\n",sizeof(Qlen));

	TDno = identify(DBno, TableName);
	if ( TDno >= 0 )
	{
		TTptr = &ttab[TDno];
		TTptr->TTrtd = TDno;
		v22 = tblrow(TDno);
		if ( v22 )
		{
			CRC = 0;
			tdef = v22;
			for ( NumFields = 0; tdef->TDFentry; tdef++ )
			{
				CRC = pcrc(CRC, tdef, 2, 0x44u, 3u);	// allow unsigned value
				NumFields++;
			}
			if ( !CRC )
				CRC = 1;

			TTptr->TableCRC		= CRC;
			TTptr->TDworkarea1	= (char *)mmalloc(129u);
			TTptr->NumFields	= NumFields;
			TTptr->TTfields		= (FLDdesc *)mmalloc(sizeof(FLDdesc) * (NumFields + 1));	// Null terminated list, sizeof(FLDdesc) == 24
			
			TDno2 = identify(DBno, "elements");
			if ( TDno2 < 0 )
				TDno2 = identify(DBno, "domains");
			if ( TDno2 > 0 )
				tblrow(TDno2);              // dead code

			tdef = v22;
			for ( fld = TTptr->TTfields; tdef->TDFentry; tdef++ )
			{
				fld->FLDtype = gettype(tdef->TDFtype, tdef->TDFsize);
				if ( TDno2 > 0 )
				{
					v6 = (char *)&elem1;
					zap((char*)&elem1, sizeof(ELEMENTREC));		 // = 76 bytes
					zap((char*)&elem2, sizeof(ELEMENTREC));
					itoms(&elem1.TDFentry, tdef->TDFentry);
					
					RetCode = retrieve(TDno2, (char *)&elem1, (char *)&elem1);
					if ( RetCode <= 0 )
					{
						sprintf(elem2.FieldName, "%d", tdef->TDFentry);
						if ( RetCode < 0 )
							dberror(RetCode, 0, TDno2);
					}
					else
					{
						v9	= tblrow(TDno2);
						dom = domsfp;			// hardcoded Table defs. see "DBdefs.h"
						for ( j = (char *)&elem2; v9->TDFentry; j = (char *)&elem2 + dom->DomOffset )
						{
							domtox(j, v6, v9->TDFsize, v9->TDFtype);
							v6 += v9->TDFsize;
							v9++;
							dom++;
						}
						if ( elem2.FLDlen > 0 )
						{
							qlen = (Qlen *)getmem(sizeof(Qlen));	// sizeof(Qlen) == 16 
							fld->FLDqlen = qlen;
							if ( fld->FLDtype == 'D' )
								qlen->Qdeflen = defDateWidth;
							else
								qlen->Qdeflen = elem2.FLDlen;
						}
					}
					v12 = strlen(elem2.FieldName) + 1;
					v13 = v12 - 1;
					if ( v12 != 1 )
					{
						if ( v13 >= 21 )
							v13 = 20;
						fld->FLDname = (char *)mmalloc(v13 + 1);
						cpybuf(fld->FLDname, elem2.FieldName, v13);
					}
				}
					
				// allocate space for data
				if ( fld->FLDtype == 'C' )
					fld->FLDdata = mmalloc(tdef->TDFsize + 1);
				else if ( fld->FLDtype != 'S' && fld->FLDtype != 'K' )
					fld->FLDdata = mmalloc(sizeof(double));
				//else don't alloc any mem for FLDdata

				// set type flags
				fld->FLDstat |= fld_ZERO;		// set quick zero flag

				if ( tdef->TDFtype & tdf_KEY)	// 0x0001 Key Field
					 fld->FLDstat |= fld_KEY;	// 0x8000;
				else
					 fld->FLDstat &= ~fld_KEY;	// 0x7FFF;

				fld->FLDelemID	= tdef->TDFentry;
				fld->FLDlen		= tdef->TDFsize;
				fld->TDFtype	= tdef->TDFtype;
				fld++;
			}
			if ( TDno2 >= 0 )
			{
				RetCode = release(TDno2);
				if ( RetCode < 0 )
					 dberror(RetCode, 0, TDno2);
			}
			return TDno;	// exit success!
		}
		return -1;	// error tblrow(TDno) failed
	}
    return TDno;	// error TDno < 0
}

int legetf()
{
    DBase	*DTptr;
	TDesc	*TTptr;
	char	*DBname;
	
	int		TDno;
	int		TTno;
	int		DBno;
	char	v20[128];
	bool	FileLock;
	
    DBname = 0;
    symbol = getsym();			// should be DBase name
    if ( symbol == 930 )
	{
		loaderr(7, sym);		// "unexpected end of line"
        return 0;
	}
    if ( dbrename )				// set if command line option "-r {db1=db2,..}" is found
    {
         if ( !dbarray )
              dbarray = camake(dbrename, ',');		// mem never free'd in clcomp!
         DBname = checkdbrename(sym);
    }
    if ( !DBname )
         DBname = sym;

	DTptr = dtab;
    DBno = 0;
    while ( DBno < no_dtabs && !cmpbuf(DBname, DTptr->FullDBname, syml + 1) )
    {
         ++DBno;
         ++DTptr;
    }

	if ( DBno == no_dtabs )		// DB in question is not already open
    {
        cdbcpystr(v20, chkpath(DBname, 0, "CLDPATH", 0, 384), 0);
        DBno = cldbopen(v20, 0);
        if ( DBno < 0 )
        {
             eprint("can't open database - %s\n", DBname);
             return 0;
		}
		// Removed Demo check
        newdtab();				// should bump no_dtabs so that next assert will pass
		assert(DBno < no_dtabs);

		dtab[DBno].FullDBname = mstrcpy(sym, 0);
        dtab[DBno].DBno = DBno;
	}
    
	// DB now opened, look for Tables/Aliases

	FileLock = false;
    TTno = 0;
    
	symbol = getsym();		// Should be next Table name
    if ( symbol == 930 )
    {
		loaderr(7, sym);	// "unexpected end of line"
        return 0;
    }
    while ( symbol != 930 )
    {
		TDno = tdopen(DBno, sym);		// *** Doesn't check if sym is actually a reserved name here!! ***
        if ( TDno < 0 )					// Error condition here
		{
		    fprintf(stderr, "can`t identify - %s err=%d\n", sym, TDno);		// not necessarily a fatal error. DBase may never be accessed.
			fflush(stderr);
			symbol = getsym();
		    if ( symbol != 180 || (symbol = getsym(), symbol != 2230) )		// 180 = ',', 2230 = table/varname
				continue;
		    symbol = getsym();
			continue;
		}
        if ( no_ttabs - 1 <= TDno )
		{
			newttab();
			assert(TDno < no_ttabs);
		}
        cdbcpystr(ttab[TDno].TableName, sym, 0);			// No sanity check on the length of TableName!
        ttab[TDno].DBnumber = DBno;

        if ( !TTno )
			TTno = TDno;				// remember lowest TDno in this DBase in case of File level Locking below

        symbol = getsym();
        if ( symbol == 180 )            // ',' Comma
        {
			symbol = getsym();
            if ( symbol == 2230 )		// String literal, assume this is an Alias! 
            {
				cdbcpystr(ttab[TDno].TableAlias, sym, 0);	// No sanity check on the length
                symbol = getsym();
            }
		}
        if ( symbol == 800 )             // "lock"
        {
			FileLock = true;
			symbol = getsym();
		}
	}

	if ( FileLock )		// if any tables had file locking, flag *all* TDs in this DBase for locking
    {
		for ( TTptr = &ttab[TTno]; TTno <= TDno; TTptr++ )
        {
			TTptr->TDlocked |= ttl_FLOCK;	// 0x0040u;	// bit 6 = FILE level locking indicator
            TTno++;
        }
    }
    return 1;
}

#endif
