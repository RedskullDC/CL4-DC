#ifndef REFFILE_C
#define REFFILE_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short reffile(RTAB *rtab)
{
	DBase	*DTptr;
	TDesc	*TTptr;
	RTAB	*rtbr;				// rtab chain for "replace" command
	ENTAB	*ent;
	FLDdesc *fld;
	
	bool	v18;
	int		DBno;
	short	OpCode;
	short	v9;
	short	v17;
	short	v14;

	int		TDrp;	// TD to be replaced
	int		TDno;
	int		Lock;

	short	ErrorCode;

	// used by replace command
	char	Aliasname[1008];
	char	TableName[1008];
	char	DBname[1008];

	ErrorCode = 0;	// default success
	TDno = rtab->TTno;
	TTptr = &ttab[TDno];
    if ( !(TTptr->TDlocked & ttl_OPEN))			//	0x0800u Indicates table has been opened with a getfile statement
    {											// clcomp should trap all these errors.
        setvar(v_ERROR, 12);		// "ERROR" 1
        return 12;
    }
    
	OpCode	= rtab->OpType;			// Specifies the operation type.
    Lock	= rtab->LockType;		// Lock type. May be a lockr
	//printf("reffile OpCode = x%04X, Lock = x%04X\n",OpCode,Lock);

	if (OpCode & rtb_SELECT)		// select command	0x0080u
	{
		TTptr->TTselect = (EXPR *)fretre(TTptr->TTselect, 0);		// get rid of previous expression
		TTptr->TTselect = getexpr(ENARR(rtab->WhereEXP), TDno);
		if ( cdbrestrict(rtdget(TTptr), TTptr->TTselect) < 0 && !(TTptr->TDlocked & ttl_CREATE))	// ttl_CREATE = 0x0200
			ErrorCode = 1;
	}
    else if ( OpCode & rtb_GET) // 0x0001 ) GET command (also NOJOIN) 
    {
		while ( 1 )
        {
			v9 = rdrec(TDno, Lock);
            if ( v9 != -10 || Lock == 1024 )	// -10 return code == locked by another user, Lock == 1024 means called with lockr (Lock Request)
				break;
			sleep(1u);			// Deadlock until another process releases this record
		}
        if ( v9 > 0 )			// exit_success
			goto LABEL_102;
		else if ( v9 == -10 )	// record was locked, but requested record with "lockr"
			ErrorCode = 4;
		else
		{
			rfree(TDno, 0);
			ErrorCode = 1;
		}
	}
    else if ( OpCode & rtb_JOIN)				// 0x0040 JOIN
    {
		if (OpCode & 0x0002)	// direction bit
		{
			if ((short) rdnxt(TDno, TTptr->TTselect, Lock, 1) > 0 )
				goto LABEL_102;
		}
		else
		{
			if ((short) rdprev(TDno, TTptr->TTselect, Lock, 1) > 0 )
				goto LABEL_102;
		}
	    ErrorCode = 2;	// no more data
	}
	else if ( OpCode & (rtb_GETNEXT|rtb_GETPREV))	// 0x0006 )	// checks 2 bits  0000-0000-0000-0110    0002 is getnext, 0004 is getprev
    {
		//printf("reffile : OpCode & (rtb_GETNEXT|rtb_GETPREV)\n");
		ErrorCode = 0;	// default success
        if ( OpCode & rtb_SEL_ALL || rtab->WhereEXP )			// rtab->field_2 & 0x100 == "all" flag
        {
			TTptr->TTselect = (EXPR *)fretre(TTptr->TTselect, 0);
            ent = 0;
            if ( !(OpCode & rtb_SEL_ALL) )						// if "all" not specified, get the "where" expression
            {
				ent = ENARR(rtab->WhereEXP);
			}
            TTptr->TTselect = getexpr(ent, TDno);
            v14 = cdbrestrict(rtdget(TTptr), TTptr->TTselect);
			if ( v14 < 0 )
				dberror(v14, 0, rtdget(TTptr));		// dead end
		}
			
		if (OpCode & rtb_GETNEXT)	// 0x0002)		// check direction 
			v9 = rdnxt(TDno, TTptr->TTselect, Lock, 0);	// rdnxt clobbers KeyTablenames :o(
		else
			v9 = rdprev(TDno, TTptr->TTselect, Lock, 0);
                
		if ( v9 > 0 )			// exit_success
			goto LABEL_102;
		else if ( v9 == -10 )	
			ErrorCode = 4;		// locked
		else
		{
			rfree(TDno, 0);
			ErrorCode = 2;		// no more data
		}
	}
	else if ( OpCode & rtb_PUT)		// 0x0008  put command
    {
		repkey(TDno, 0);								// ensure key fields are updated in record area
        if ( reprec(TDno, Lock == 0) < 0 )				// Error if we couldn't put the record back to disk
			ErrorCode = 3;
	}
	else if ( OpCode & rtb_DELETE )	// 0x0010  delete command
    {
        v17 = TTptr->TDlocked;
        if ( v17 & ttl_HASK )		//	0x0010u	 Has Key Tables
        {
            //if ( (v17 & 0x0021) == 0x0021 && cmpbuf(TTptr->TDworkarea1, TTptr->TDworkarea2, TTptr->TDrecsize) )
            v18 = ( (v17 & (ttl_AVAIL|ttl_LOCK)) == (ttl_AVAIL|ttl_LOCK) && cmpbuf(TTptr->TDworkarea1, TTptr->TDworkarea2, TTptr->TDrecsize)) ? true : false;
			
			if ( !v18 && (short)rdrec(TDno, 0) <= 0 )
            {
				rfree(TDno, 0);
                ErrorCode = 1;	// error_exit
                ulckrec(TDno);
			}
		}
        //if ( ErrorCode )
		//	goto LABEL_102;
		//if ( delhier(TDno, 1) == 0 )
		//	ErrorCode = 1;
        if ( !ErrorCode && !delhier(TDno, 1))
			ErrorCode = 1;
	}
    else if ( OpCode & rtb_CLEAR )		// 0x0020 clear record
    {
		rfree(TDno, 0);												// bit clunky.  rfree(TD,0) frees all Fields, but also clears fld_DATA_AVAIL bits
		for ( fld = TTptr->TTfields; fld->FLDelemID; fld++ )		// so we have to go and turn them all back on again. :o(
			fld->FLDstat |= fld_DATA_AVAIL;	// 0x0010u; *** Seems like the wrong flag to set ?? ***
	}
    else if ( OpCode & rtb_REPLACE )			// replace command. ** Undocumented in CL4 manuals **  GEFAHR!
    {
        DBname[0]		= 0;
        TableName[0]	= 0;
        Aliasname[0]	= 0;

        TDrp = rtab->TTno;				// TTno to be replaced
        rtbr = RTARR(rtab->NextRT);		// RTAB chain of table details to replace TTno
        if ( rtbr )
        {
            exptobuf(DBname, ENARR(rtbr->enright), 0);					// DBname
            rtbr = RTARR(rtbr->NextRT);
            if ( rtbr )
            {
                exptobuf(TableName, ENARR(rtbr->enright), 0);			// TableName
                rtbr = RTARR(rtbr->NextRT);
                if ( rtbr )
					exptobuf(Aliasname, ENARR(rtbr->enright), 0);		// Aliasname
			}
		}

        DTptr	= dtab;
        DBno	= 0;
        while ( no_dtabs > DBno && *DTptr->FullDBname )
        {
			if ( !compare(DBname, clbasename(DTptr->FullDBname)) )		// break when a match found
				break;
			DTptr++;
            DBno++;
		}
        if ( DBno == no_dtabs )			// reached end of dtabs with no name match!
		{
			ErrorCode = 1;				// error_exit
		}
		else							// found match on DBname, look for TableName match
		{
			TTptr	= ttab;
			TDno	= 0;
			if ( no_ttabs > 0 )
			{
				// look for a match on DBname and TableName. Match on Alias is optional
				while ( TTptr->DBnumber != DBno || compare(TableName, TTptr->TableName) || Aliasname[0] && compare(Aliasname, TTptr->TableAlias) )
				{
					++TDno;
					++TTptr;
					if ( TDno >= no_ttabs )// error_exit
					{
						ErrorCode = 2;	// no more data
						goto LABEL_102;
					}
				}
				if ( TDno < no_ttabs )
				{
					printf("replace 235\n");
					if ( ttab[TDno].TTrtd < 0 )	// make sure replacing Table is actually open!
						getfile(DBname, TableName);

					cpybuf((char *)&ttab[TDrp], (char *)&ttab[TDno], sizeof(TDesc));	// 80  copy TDesc record over the top of another - bit hacky :o)
					*(short *)&ttab[TDrp].TableAlias[0] = 0x003E;						// '>'	tell closedb() not to close DB associated with this TT (avoid double close) 
					goto LABEL_102;
				}
			}
			ErrorCode = 2;	// no more data error_exit
		}
	}
	else
	{
		printf("reffile : Unknown command!!!\n");	// should be unreachable
        ErrorCode = 9;
	}

//=======================================
LABEL_102:
    if ( Lock == 1024 )					// Lock request -> lockr  0x400
    {
        if ( ErrorCode == 4 )
        {
            setvar(v_LOCKED, 1);		// "LOCKED"
            ErrorCode = 0;
        }
        else
            setvar(v_LOCKED, 0);		// "LOCKED"
    }
    setvar(v_ERROR, ErrorCode);			// "ERROR"

	//printf("reffile exiting. returned ErrorCode = %d\n",ErrorCode);
    return ErrorCode;
}

#endif
