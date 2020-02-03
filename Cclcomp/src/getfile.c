#ifndef GETFILE_C
#define GETFILE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

short tdopen_0(int DBno, TDesc *TTptr, int Lock);

// not public
int tgetfile(TDesc *TTptr)
{
	int		result;
	int		v3;
	short	Error;
	
	result = rdbget(TTptr->DBnumber);
	if ( result >= 0 )
	{
		Error = tdopen_0(result, TTptr, ((unsigned int)TTptr->TDlocked >> 6) & 1);
		v3 = Error;
		if ( Error < 0 )
		{
			if ( Error == -10 )
				eprint("table locked - %s\n", TTptr->TableName);
			else
			{
				if ( !cmpbuf(TTptr->TableName, "domains", 6u) && !cmpbuf(TTptr->TableName, "elements", 8u) )
					eprint("can't identify - %s (err=%d)\n", TTptr->TableName, v3);
			}
		}
        result = v3;
	}
	return result;
}

short tdopen_0(int DBno, TDesc *TTptr, int Lock)
{
	FLDdesc	*fld;
	FLDdesc	*j;
	TDef	*tdef;
	TDef	*v17;

	int		v10;
	int		v11;
	int		v12;
	int		TDno;
	int		RecSize;

	unsigned short	CRC;
	short	NumFields;
	short	v4;
	short	v5;

	char	FLDtype;
	char	v20[32];
	ELEMENTREC elem;
	 
	 assert(TTptr->TTrtd == -1);

	if ( Lock )
		v4 = lidentify(DBno, TTptr->TableName);
	else
		v4 = identify(DBno, TTptr->TableName);

     v5 = v4;
     if ( v4 >= 0 )
     {
          v17 = tblrow(v4);
          v4 = -1;
          if ( v17 )
          {
               TTptr->TTrtd = v5;
               CRC	= 0;
               tdef = v17;
               NumFields = 0;
               for ( RecSize = 0; tdef->TDFentry; tdef++ )
               {
                    RecSize += tdef->TDFsize;
                    CRC = pcrc(CRC, tdef, 2, 0x44u, 3u);	// allow unsigned value
                    NumFields++;
               }
               if ( !CRC )
                    CRC = 1;
               if ( TTptr->TableCRC )
               {
                    if ( TTptr->TableCRC != CRC )
                    {
                         eprint("table definition incompatibility (calc=%u, got=%u) - %s\n", CRC, TTptr->TableCRC, TTptr->TableName);
                         return -1;
                    }
               }
               else
                    TTptr->TableCRC = CRC;

               TTptr->TDworkarea1 = (char *)mmalloc(RecSize + 1);
               TTptr->TDworkarea2 = (char *)mmalloc(RecSize + 1);
               TTptr->NumFields = NumFields;
               TDno = identify(DBno, "elements");
               if ( TDno < 0 )
                    TDno = identify(DBno, "domains");

               fld = (FLDdesc *)mmalloc(sizeof(FLDdesc) * (NumFields + 1));
               TTptr->TTfields = fld;
               tdef = v17;
               for ( j = fld; tdef->TDFentry; tdef++ )
               {
                    FLDtype = gettype(tdef->TDFtype, tdef->TDFsize);
                    j->FLDtype = FLDtype;

					if ( FLDtype == 'C' )
                         v10 = tdef->TDFsize + 1;
                    else
                    {
                         if ( tdef->TDFsize )
                              v10 = 8;
                         else
                         {
                              v10 = 0;
                              v20[0] = 0;
                              if ( TDno > 0 )	// Read ELEMENTREC directly
                              {
                                   itoms(&elem.TDFentry, tdef->TDFentry);
                                   v11 = (signed short)getr(TDno, (char*)&elem, 0);

                                   if ( (signed short)v11 <= 0 )
                                   {
                                        if ( v11 < 0 )
                                             dberror(v11, 0, TDno);				// deadend
                                        sprintf(v20, "%d", tdef->TDFentry);		// v11 == 0, not found. use TDFentry directly
                                   }
                                   else
                                        cdbcpystr(v20, elem.FieldName, 0);		// found a match, use FieldName instead.

								   v10 = strlen(v20) + 1;
                              }
                         }
                    }
                    j->FLDdata = mmalloc(2 * v10);	// twice as much space as is required??
                    if ( j->FLDtype == 'S' || j->FLDtype == 'K' )
                         cdbcpystr((char *)j->FLDdata, v20, 0);
                    
					if ( tdef->TDFtype & tdf_KEY)	// 0x0001
                         j->FLDstat |= fld_KEY;		// 0x8000 KeyField
                    
					j->FLDstat	|= fld_ZERO;		// Quick ZERO flag
                    j->FLDlen	 = tdef->TDFsize;
                    j->FLDelemID = tdef->TDFentry;
                    j->TDFtype   = tdef->TDFtype;
                    ++j;
               }
               if ( TDno > 0 )
               {
                    v12 = release(TDno);
                    if ( v12 < 0 )
                         dberror(v12, 0, TDno);
               }
               TTptr->TDrecsize = RecSize;
               v4 = 0;
          }
     }
     return v4;
}


// different functionality to clenter/libcl4
int rtdget(TDesc *tt)
{

	assert(ttab <= tt && tt < &ttab[no_ttabs]);

	if ( tt->TTrtd == -1 && tgetfile(tt) < 0 )
    {
        eprint("can't open table '%s'\n", tt->TableName);
        dberror(-98, tt->DBnumber, tt->TTrtd);
    }
	
	assert(tt->TTrtd >= 0 && tt->TTrtd < no_ttabs);

    return tt->TTrtd;
}

// Not called in clcomp!
/*
int getfile(char *DBname, char *TableName)			
{
    TDesc *TTptr; // esi@9
    char *Alias; // eax@16
    short DBno; // [sp+18h] [bp-10h]@1

printf("getfile  DBname = %s, TableName = %s\n", DBname, TableName);

    DBno = 0;
    while ( no_dtabs > DBno )
    {
		if ( !strcmp(DBname, dtab[DBno].FullDBname))	// We found a match!
			break;
		DBno++;
    }
    if ( no_dtabs == DBno )		// No match. This DBase isn't currently open.
    {
        newdtab();				// Create a new DBase slot.
        if ( no_dtabs <= DBno )
            __assert_fail("dbii < no_dtabs", "getfile.c", 213u, "getfile");
    }
    if ( !dtab[DBno].FullDBname )	// Wasn't a match above, copy DBname incto structure
    {
        dtab[DBno].DBno = -1;
        dtab[DBno].FullDBname = mstrcpy(DBname, 0);
    }

    for ( TTptr = ttab + 3; TTptr < &ttab[no_ttabs]; ++TTptr )	// TD0 -> TD2 == system tables 
    {
        if ( !TTptr->TableName[0] )		// got to end of ttab's with no match on TableName
            break;
        if ( !strcmp(TTptr->TableName, TableName) )	// We found a match!
            break;
    }
    if ( TTptr == &ttab[no_ttabs] )		// True if we got to end of ttab's with no match
    {
        newttab();						// Create a new ttab
        TTptr = &ttab[no_ttabs - 1];
    }
    if ( !TTptr->TableName[0] )			// true if this is a new ttab
    {
//        Alias = TableName;
//        if ( *TableName )
//        {
//			while (*Alias )
//			{
//				if (*Alias == ',')
//				{
//					*Alias++ = 0;
//					break;
//				}
//			}
//		}
//		printf("getfile: %s,%s",TableName, Alias);
		
        Alias = TableName;
        if ( *TableName )
        {

            if ( *TableName == ',' )
            {
LABEL_20:
                if ( *Alias )
                    *Alias++ = 0;
            }
            else
            {
                while ( 1 )
                {
                    ++Alias;
                    if ( !*Alias )
                        break;
                    if ( *Alias == ',' )
                        goto LABEL_20;
                }
            }
        }
		printf("getfile: %s,%s",TableName, Alias);
        cdbcpystr(TTptr->TableAlias, Alias, 0);
        cdbcpystr(TTptr->TableName, TableName, 0);
        TTptr->DBnumber = DBno;
        TTptr->TTrtd = -1;
    }
    return rtdget(TTptr);
}
*/
int rdbget(int DBno)
{
     DBase	*DTptr;
	 char	*DBname;
	 int	PctFree;
	 int	DBnoa;
	 
     DTptr = &dtab[DBno];
     if ( DTptr->DBno >= 0 )	// This DBase already opened?
          return DTptr->DBno;
     
	 DBname = mstrcpy(chkpath(DTptr->FullDBname, 0, "CLDPATH", 0, 0x180), 0);	// 0x180 mode read_write  permission bits '-rw-------'
     DBnoa = cldbopen(DBname, 0);
     if ( DBnoa < 0 )
     {
          eprint("can't open database - %s\n", DTptr->FullDBname);
          return DBnoa;
     }
     //v4 = checkForDemo(DBnoa, DTptr->FullDBname);
     //v4 = 1;
	 //ErrorCode = -1;
     //if ( v4 )
     //{
          DTptr->FullDBname = DBname;
          DTptr->DBno = DBnoa;
          PctFree = (dbspace(DBnoa) * 100) / dbsize(DBnoa);
          if ( PctFree < 10 )	// Less than 10% free space left!!
          {
               /*if ( isCGI )
               {
                    eprint("%s is %d%% full\n", DTptr->FullDBname, 100 - PctFree);
               }
               else
               {*/
                    eprint("\a%s is %d%% full\n", DTptr->FullDBname, 100 - PctFree);
                    sleep(5u);
               //}
          }
          return DBnoa;
     //}
     //return ErrorCode;
}
#endif
