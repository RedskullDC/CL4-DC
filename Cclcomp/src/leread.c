#ifndef LEREAD_C
#define LEREAD_C

short leread(bool isPipe)
{
    ENTAB *entb;
	PRTAB *v14;
	PRTAB *prtab;
    SFTAB *sftab;
    FLDdesc *fld;
	
    int cmd;
    int TDno;
    int FLDno;

	register short PRno;
    short expno;
	short ENTno;
    short SFno;
    bool cls_tbls;

    cmd = symbol;								// save command token for below
    SFno = getsfmem();
    allsfpt(SFno, &sftab);
    symbol = getsym();							//    2230            2220
    if ( symbol != 2230 && symbol != 2220 )		// variable name or string literal 
    {
		if ( symbol == 930 )
			loaderr(50, sym);					// "missing file/pipe name"
		else
			loaderr(51, sym);					// "invalid file/pipe name"
        dallsfpt(&sftab);
        return 0;
	}
	if (symbol == 2220 && !strncmp(sym,"tables",7))	// look for the 'tables' specifier in case this 
	{												// is a "CLOSE" instruction
		cls_tbls = true;
	}
	else
	{
		cls_tbls = false;
        expno = loadexp(0, 1);
        if ( !expno )
			return 0;			// error_exit		*** doeesn't dallsfpt(&sftab); ***
		sftab->AliasEnt = expno;
	}

// =============================

	if ( cmd == 540 )                       // read
    {
		if ( symbol == 930 )				// <CR>  shouldn't be reachable. caught above : "missing file/pipe name"
        {
			loaderr(7, sym);                // "unexpected end of line"
			return 0;
		}
		sftab->PRTno = getprmem();
		allprpt(sftab->PRTno, &prtab);
		if ( symbol != 930 )				// should always be true?
		{
			while ( symbol == 2230 )		// while table/field name
			{
				if ( nxtch() == '.' || (TDno = findtd(sym, -1), TDno <= 0) ) // read single field or variable
				{
					ENTno = getenmem();
					if ( !gettfexp(ENTno) )
					{                    
						dallsfpt(&sftab);
						dallprpt(&prtab);
						return 0;
					}
					prtab->VAR_ENTno = ENTno;
				}
				else									// *all* fields in a table
				{
					fld = ttab[TDno].TTfields;
					for ( FLDno = 0; fld->FLDelemID; fld++ )
					{
						ENTno			 = getenmem();
						entb			 = ENARR(ENTno);
						entb->TTno		 = TDno;
						entb->RecNo		 = FLDno;
						entb->entype	 = 1;
						prtab->VAR_ENTno = ENTno;
						if ( fld[1].FLDelemID )			// NULL when we get to end of fields
						{
							PRno = getprmem();			//*** getprmem() can change prtab! ***
							prtab->NextPR = PRno;
							v14 = PRARR(prtab->NextPR);
							prtab = v14;
						}
						FLDno++;
					}
					symbol = getsym();
				}
				if ( symbol == 930 )					// <CR> terminates the command
				{
					dallprpt(&prtab);
					sftab->cmd_flags = 1;
					goto LABEL_56;
				}
				else // ( symbol != 930 )
				{
					PRno = getprmem();					//*** getprmem() can change prtab! ***
					prtab->NextPR = PRno;
					v14 = PRARR(prtab->NextPR);
					prtab = v14;
				}
			}
			loaderr(56, sym);				// "invalid in read statement"
			dallsfpt(&sftab);
			dallprpt(&prtab);
			return 0;
		}
		dallprpt(&prtab);
		sftab->cmd_flags = 1;
	}
	else if ( cmd == 440 )					// "close"
    {
        if ( cls_tbls )
			sftab->cmd_flags = 0x20u;		// close 'tables'
		else
			sftab->cmd_flags = 0x08u;		// close file|pipe descriptor
        symbol = getsym();
	}
    else if ( cmd == 560 || cmd == 1230 )	// 560 = "write", 1230 = ???
    {
		if ( symbol == 930 )
        {
			loaderr(7, sym);				// <CR>  shouldn't be reachable. caught above : "missing file/pipe name"
			return 0;
		}
		sftab->cmd_flags = 0x02u;
        expno = leprint();					// presumably calls getsym() prior to exit
        if ( !expno )
        {
			dallsfpt(&sftab);				// 
            return 0;
		}
        sftab->PRTno = expno;
	}

LABEL_56:
    if ( isPipe )							// commands are to pipes : popenin popenout popenio
		sftab->cmd_flags |= 0x0040u;
	dallsfpt(&sftab);
    return SFno;
}

#endif
