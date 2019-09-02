#ifndef DUMPENC_C
#define DUMPENC_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"                 // for bit field values

int *__errno_location(void);

int dumpenc(char *EncFileName)
{
	TDesc		*TTptr;
	FLDdesc		*fld;
	PTAB		*ptb;
	BTAB		*btb;
	KXTAB		*kx;
	ONESC		*onesc;

	int			*v2;
	char		*ptr;

	int			v13;
	int			j;
	size_t		v19;
	
	unsigned short i;
	short		OpCode;
	short		NumEscs;
	short		PTABno;
	short		NumTables;
	short		TempTables;
	short		count;
	
    if ( !btab->BlockName[0] )
	{
        eprint("This is an empty ent file - nothing to compile!\n");
		return 1;
    }
    dp = fopen64(EncFileName, "w");
    if ( !dp )
	{
        v2 = __errno_location();
        eprint("can't open %s for writing - %s\n", EncFileName, strerror(*v2));
		return -*v2;
    }
    putucount(0x9DD3u);							// 0x9DD3 = 40403, 0x9DD4 = 40404 = version number
	// *** NON-STANDARD ***
	putcount((short)(extendDC << 1 | tpenc));   // Signify if this is a Portable enc and if Extended DC features are present!!!
												// If tpenc and extendDC both set, will produce a file unreadable by real clenter!! 
    
//-----------------------------------
//	Set flag bits appropriately for any open tables in TTptr->TDlocked
	TempTables	= 0;
    NumTables	= 0;
    TTptr		= ttab;
    count		= 0;
    while ( no_ttabs > count )
	{
		if ( TTptr->TableName[0] )
        {
			NumTables++;
            if ( TTptr->TDlocked & ttl_CREATE)						// 0x0200 // Temporary table flag
				TempTables++;
			fld = TTptr->TTfields;
            if ( isclcomp )
            {
				while ( fld->FLDelemID )
				{
					if ( fld->FLDtype == 'S' )
						TTptr->TDlocked |= ttl_HASSUB;				// 0x4000 // Sub Records flag bit = 0x4000
					else if ( fld->FLDtype == 'K' && findtd(fld->FLDname, -1) > 0)
					{
						TTptr->TDlocked |= ttl_HASK;				// Key Tables flag bit = 0x0010
						if (TTptr->TDlocked & (ttl_PUT | ttl_GET))	// 0x0006  if there is a put or get on the kay table..
							TTptr->TDlocked |= ttl_GETK;			// 0x0008	// set the getk flag
					}
					fld++;
				}
			}
		}
		count++;
        TTptr++;
    }

//-----------------------------------
//	Dump Defs for any tables which are opened
	if ( isclcomp )
    {
		eprint("%5d open tables\n", NumTables - TempTables);
		if ( TempTables )
            eprint("%5d temp tables\n", TempTables);
    }

	putcount(NumTables);		// Open Tables
    TTptr = ttab;
    count = 0;
    while ( no_ttabs > count )
	{
		if ( TTptr->TableName[0] )
        {
			puttext(dtab[TTptr->DBnumber].FullDBname, 0);
            if ( TTptr->TDlocked & ttl_FLOCK)	// 0x0040u		// File locking flag: getfile XYZ lock
				puttext("locking", 0);
            ptr = mstrcpy(TTptr->TableName, "," ,TTptr->TableAlias, 0);
            puttext(ptr, 0);
            mfree_0(ptr);
            putbits(TTptr->TDlocked);
            if ( notabchks )
				TTptr->TableCRC = 0;	// Ignore CRC of tables. dangerous!!
			putcount(TTptr->TableCRC);
		}
        count++;
        TTptr++;
	}

//-----------------------------------
//	Dump Global Escapes next
    onesc = oelist;
    for ( NumEscs = 0; onesc; NumEscs++ )
		onesc = onesc->NextESC;

	putcount(NumEscs);
    if ( NumEscs )
		dumpoe(oelist);

//-----------------------------------
//	Dump Variable defs, followed by any init values
	v13 = last_var;
    if ( isclcomp )
		eprint("%5d variables\n", last_var);
    putcount(v13);
    putftabs(ttab->TTfields, v13);			// **** puts one too many to disk. Null terminated list ****
    
	fld = ttab->TTfields;					// Variables hang off Table 0 as regular fields
    count = v13 - 1;
    while ( count >= 0 )
	{
		if ( !(fld->FLDstat & fld_ZERO) )	// check quick zero flag. If not ZERO, save value in enc file
        {
			if ( fld->FLDtype == 'C' )
				puttext((char *)fld->FLDdata, fld->FLDlen);			// text value
			else
				putdouble(*(double *)fld->FLDdata, fld->TDFtype);	// all others stored as doubles
		}
        fld++;
        count--;
	}
//-------------------------------------------
// Next dump number of program steps
    count = 0;
	i = getptabp(count); 
	while((PTARR(i))->OpCode )
		i = getptabp(++count);

    if ( isclcomp )
		eprint("%5d steps\n", count);

//-------------------------------------------
// Next dump program nodes
	putptabs(&ptarr);						// program steps array
    if ( isclcomp )
		eprint("%5d nodes\n", enarr.NextElement);

	putenodes(&enarr);						// enodes = expressions
    putxtabs(&xtarr);                       // Accept field defs
    putrtabs(&rtarr);                       // Redisplay defs
    putratabs(&raarr);                      // Range Defs
    putxftabs(&xfarr);                      // XFER defs.  Copy command defs
    putsftabs(&sfarr);                      // FCBs:  Files we open/read/write
    putprtabs(&prarr);                      // print statements
    putrdtabs(&rdarr);                      // read statements
    putkxtabs(&kxarr);                      // Create statement defs
	
	// create statements parameters
	for (count = 0; count < kxarr.NextElement; count++)		// kx = Create statements
    {
		kx = &kxarr.TableAddr[count];
        putcount(kx->NumKeyFields);
        putarrcount(kx->KeyElemIDs, kx->NumKeyFields);
        putarrcount(kx->TkeyTy, kx->NumKeyFields);
        putcount(kx->NumDataFields);
        putarrcount(kx->DataElemIDs, kx->NumDataFields);
        putcount(kx->NumTTs);
        putarrtds(kx->TTlist, kx->NumTTs);
	}

//-------------------------------------------
//	dump string values
    if ( isclcomp )
		eprint("%5ld strings\n", strarr.StrTableSize);

    putLong(strarr.StrTableSize);              // 4 bytes of string table size
    ptr = strarr.StringTable;
    for ( j = strarr.StrTableSize; j; j -= v19 )
    {
		v19 = 15000;                          // Dump 15000 bytes at a time
        if ( j < 15001 )
			v19 = j;
		putbuf(ptr, v19);
        ptr += v19;
	}

	count = 0;
    for ( PTABno = (unsigned short)getptabp(0); ; PTABno = (unsigned short)getptabp(count) )
    {
		ptb = PTARR(PTABno);
		if ( !ptb->OpCode )		// end of instruction steps, break out
			break;
		/*OpCode = ptb->OpCode;
		switch (OpCode)
		{
			case 1 ... 49:
				printf("dumpenc#245 OpCode = %4d\n",OpCode);
				break;
			default:
				break;		
		}*/
		if ( (unsigned short)(ptb->OpCode - 1) > 48u )		// [1 ... 49]
        {
			if (ptb->TABno)
			{
				OpCode = ptb->OpCode;
				if ( OpCode == 250 || OpCode == 260 )		// 250 = refresh, 260 = screen
				{
					puttext(STRARR(ptb->TABno),0);			// Save a "screen_name" here
				}
				else if ( (unsigned short)(ptb->OpCode - 650) <= 48u)	// Not sure this is reachable in V4?  [650 ... 698]
				{
					printf("dumpenc OpCode = %4d, pt->TABno = %d\n",OpCode,ptb->TABno);
					putbuf(STRARR(ptb->TABno), 20u);		// Put a block name here? (max 20chars)
				}
			}
		}
        else	// Screen display type opcodes
        {
			dumpoe(XTARR(ptb->TABno)->onesc);			// Dump On-Escape
		}
		count++;
	}
    
//-------------------------------------------
//	dump program blocks, followed by screens, On_Escapes
	btb = btab;
    for ( count = 0; btb->BlockName[0]; btb++ )
		count++;
	if ( isclcomp )
		eprint("%5d process blocks\n", count);
	putbtabs(count);
    for ( btb = btab; btb->BlockName[0]; btb++ )
    {
		dumpscr(btb->Screens);                   // screens
        dumpoe(btb->On_delete);                 // global escapes follow for this block
        dumpoe(btb->On_exit);
        dumpoe(btb->On_excep);
        dumpoe(btb->On_entry);
	}
	if (extendDC)
	{
		eprint("*****************************************\n");
		eprint("*** Extended DC functionality enabled ***\n");
		eprint("*****************************************\n");
	}
    fclose(dp);
    return 0;
}

#endif
