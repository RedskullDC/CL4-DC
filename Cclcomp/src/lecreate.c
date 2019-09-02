#ifndef LECREATE_C
#define LECREATE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

// careful not to confuse with tdopen()!!

int _tdopen(int DBno, int TTno)
{
	TDesc	*TTptr;
	TDef	*v18;
	TDef	*tdef;
	FLDdesc *fld;
	
    unsigned int v7;
    signed int result;
    size_t	v9;
    short	NumFields;
    short	v6;
    short	v8;
    int		TDno;
    int		ErrorNo;
    unsigned short	CRC;
    char	arg[112];
    char	v23[32];
    char	FLDtype;
    ELEMENTREC elem;

    if ( no_ttabs - 1 <= TTno )
    {
          newttab();
		  assert(TTno < no_ttabs);
    }
    TTptr	= &ttab[TTno];
    v18		= tblrow(TTno);
    result = 0;
    if ( v18 )
    {
		tdef = v18;
        CRC = 0;
        for ( NumFields = 0; tdef->TDFentry; tdef++ )
        {
			CRC = pcrc(CRC, tdef, 2, 0x44u, 3u);
            NumFields++;
		}
        if ( !CRC )
			CRC = 1;
		TTptr->TableCRC = CRC;
        
		TTptr->NumFields = NumFields;
        TTptr->TTfields = (FLDdesc *)mmalloc(sizeof(FLDdesc) * (NumFields + 1));		// sizeof(FLDdesc) == 24. zero terminated array
        TDno = identify(DBno, "elements");
        if ( TDno < 0 )
			TDno = identify(DBno, "domains");
		
		tdef = v18;
        for ( fld = TTptr->TTfields; tdef->TDFentry; tdef++ )
        {
			v23[0] = 0;
            if ( TDno > 0 )
            {
				itoms(&elem.TDFentry, tdef->TDFentry);
				v6 = getr(TDno,(char*)&elem, 0);
                ErrorNo = v6;
                if ( v6 <= 0 )
                {
					sprintf(arg, "domid %d not in %s.elements", tdef->TDFentry, dtab[DBno].FullDBname);
                    loaderr(82, arg);
                    v23[0] = 0;
                    if ( (signed short)ErrorNo < 0 )
						dberror(ErrorNo, 0, TDno);// dead end exit
				}
                else
                {
					elem.FLDlen = 0;	// ensure that FieldName is terminated
                    cdbcpystr(v23, elem.FieldName, 0);
				}
                v7 = strlen(v23) + 1;
                v8 = v7 - 1;
                if ( v7 != 1 )
                {
					v9 = v8 + 1;
                    fld->FLDname = (char *)mmalloc(v8 + 1);
                    cpybuf(fld->FLDname, v23, v9);
				}
			}
            FLDtype = gettype(tdef->TDFtype, tdef->TDFsize);
            fld->FLDtype = FLDtype;
            
			if ( FLDtype == 'C' )
				fld->FLDdata = mmalloc(tdef->TDFsize + 1);				// string 
			else if ( fld->FLDtype != 'S' && fld->FLDtype != 'K' )
				fld->FLDdata = mmalloc(sizeof(double));					// all others stored as doubles 8

			fld->FLDstat |= fld_ZERO;		// Quick ZERO flag
              
			if ( tdef->TDFtype & tdf_KEY )	// 0x0001 = Key Field flag
				fld->FLDstat |= fld_KEY;	// 0x8000;
			else
				fld->FLDstat &= ~fld_KEY;	// 0x7FFF;

			fld->FLDelemID	= tdef->TDFentry;
            fld->FLDlen		= tdef->TDFsize;
            fld->TDFtype	= tdef->TDFtype;
            fld++;
		}
        v6 = release(TDno);
        if ( v6 < 0 )
			dberror(v6, 0, TDno);
		result = 1;
	}
    return result;
}

short lecreate(void)
{
	
    TDesc	*TTptr;
    KXTAB	*kxtab;
    int		*v16;
    short	*v10;

    int		TDno;
    int		TTno;
	short	count;
    short	v11;
	short	KXno;

    char	NewTableName[144];

    short	KeyElemIDs[100];
    short	KeyTypes[100];
    short	DataElemIDs[200];
    int		TTnoList[20];

	KXno = getkxmem();
	allkxpt(KXno, &kxtab);

    kxtab->TTlist		 = TTnoList;
    kxtab->KeyElemIDs	 = KeyElemIDs;
    kxtab->TkeyTy		 = KeyTypes;
    kxtab->DataElemIDs	 = DataElemIDs;
    kxtab->RecSize		 = 0;
    kxtab->KeySize		 = 0;
    kxtab->NumDataFields = 0;
    kxtab->NumKeyFields	 = 0;
    kxtab->NumTTs		 = 0;
    
	// wipe the the arrays
	for (count = 0; count <= 99 ; count++ )		
	{
		KeyElemIDs[count] = 0;						// 100 Key Elements (should be overkill)
        KeyTypes[count] = 0;
    }

	for (count = 0; count <= 199 ; count++)			// 200 Data elements
		DataElemIDs[count] = 0;

	for (count = 0; count <= 19 ; count++)			// Table can have fields from up to 20 TT's
		TTnoList[count] = 0;

	symbol = getsym();
    if ( symbol == 1670 )							// unique
    {
		kxtab->KXFlags |= kxf_UNIQUE;				// unique flag
        symbol = getsym();
    }
    
	if ( symbol == 2230 )							// Variable/Table name
	{
		cdbcpystr(NewTableName, sym, 0);			// next symbol is the new table name
		if ( findtd(sym, -1) < 0 )					// see if that name is already used. -1 == not found
		{
			symbol = getsym();								// next symbol should be start of key fields
			if ( getkflds(kxtab) >= 0 )						// -1 == error. getkflds will load error code. 0 = success
			{
				if ( kxtab->NumKeyFields )					// no key fields? Error!!
				{

				// --- load non-key fields
					if ( symbol == 1740 )						// with
					{
						symbol = getsym();
						if ( getxflds(kxtab) < 0 )				// no extra fields?
						{										// should this load an error??
							dallkxpt(&kxtab);
							return 0;
						}
					}

				// --- look for optional specifiers
					if ( symbol == 380 )						// empty
					{
						if ( !(kxtab->KXFlags & kxf_UNIQUE) )	// Empty without unique specifier is an error
						{
							loaderr(65, NewTableName);			// "must be 'create unique' to be empty"
							dallkxpt(&kxtab);
							return 0;
						}
						kxtab->KXFlags |= kxf_EMPTY;			// Empty flag 0x1000
						symbol = getsym();
					}
				   
					else if ( symbol == 70 || symbol == 930 )   // all or newline. all is default behaviour if not specified
						symbol = getsym();						// OK, just continue

					else if ( symbol == 1720 )					// where ( conditional expression )
					{
						symbol = getsym();
						v11 = loadexp(1, 1);
						if ( !v11 )
						{
							dallkxpt(&kxtab);
							return 0;
						}
						kxtab->WhereEXP = v11;
					}

				//--- collected all the info required, do range checks etc.

					if ( kxtab->KeySize <= 128 )
					{
						if ( kxtab->RecSize <= 1016 )
						{
							TDno = mktable(TTnoList, kxtab->NumTTs, KeyElemIDs, DataElemIDs, KeyTypes);	// Try and create the temp table!
							TTno = TDno;
							if ( TDno >= 0 )	// Table was created ok, now try to open it normally.
							{
								if ( _tdopen(ttab[TTnoList[0]].DBnumber, TDno) )		// Table was created in DB pointed to by first TTno?
								{
									TTptr = &ttab[TTno];
									cdbcpystr(TTptr->TableName, NewTableName, 0);		// possible buffer overrun on the alias copy!
									cdbcpystr(TTptr->TableAlias, NewTableName, 0);
									
									// update ttab record in memory for future instructions which may reference it :o)
									TTptr->TTrtd = TTno;
									TTptr->DBnumber = ttab[TTnoList[0]].DBnumber;	// Hmmmmm.....
									TTptr->TDlocked |= ttl_CREATE;					// temp table flag 0x0200
									if ( kxtab->KXFlags & kxf_UNIQUE )				// unique flag bit
										TTptr->TDlocked |= ttl_UNIQUE;				// 0x400;
									
									// All ok, create the kxtab record
									cdbcpystr(kxtab->TableName, NewTableName, 0);
									kxtab->TTno = TTno;
									kxtab->KeyElemIDs	= (short *)mmalloc(sizeof(short) * (kxtab->NumKeyFields + 1));	// +1 as all null terminated lists
									kxtab->TkeyTy		= (short *)mmalloc(sizeof(short) * (kxtab->NumKeyFields + 1));
									kxtab->DataElemIDs	= (short *)mmalloc(sizeof(short) * (kxtab->NumDataFields + 1));
									kxtab->TTlist		= (int *)mmalloc(sizeof(int) * (kxtab->NumTTs + 1));

									//  ----------------------------
									//  Copy into the kxtab structure

									v10 = kxtab->KeyElemIDs;
									v11 = 0;
									while ( kxtab->NumKeyFields >= v11 )
										*v10++ = KeyElemIDs[v11++];
									
									v10 = kxtab->TkeyTy;
									v11 = 0;
									while ( kxtab->NumKeyFields >= v11 )
										*v10++ = KeyTypes[v11++];
									
									v10 = kxtab->DataElemIDs;
									v11 = 0;
									while ( kxtab->NumDataFields >= v11 )
										*v10++ = DataElemIDs[v11++];
									
									v16 = kxtab->TTlist;
									v11 = 0;
									while ( kxtab->NumTTs >= v11 )
										*v16++ = TTnoList[v11++];

									dallkxpt(&kxtab);
									return KXno;				// *** exit_success ***
								}
								else
									loaderr(14, NewTableName);	// "table not open"
							}
							else
								loaderr(45, NewTableName);		// "can't create table"
						}
						else
							loaderr(49, sym);					// "record length too big"
					}
					else
						loaderr(48, sym);						// "key length too big"
				}
				else
					loaderr(47, sym);							// "no key fields defined"
			}
			// getkflds(kxtab) will load error code if required
		}
		else
			loaderr(46, sym);									// "table/alias redefined"
	}
	else	// no tablename specified
    {
        if ( symbol == 930 )
			loaderr(40, sym);					//"missing table/alias"
        else
			loaderr(39, sym);					//"invalid table/alias"
    }

	dallkxpt(&kxtab);
	return 0;
}

#endif
