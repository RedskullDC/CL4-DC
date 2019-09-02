#ifndef LEOPEN_C
#define LEOPEN_C

#include <stdio.h>
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

void newfile()
{
    int v0;
	v0 = no_files++;
    _files = (FCB *)mrealloc(_files, sizeof(FCB) * v0, sizeof(FCB) * no_files);		// sizeof(FCB) == 28
}

short leopen(bool isPipe)
{
	char	*StrVal;
	FCB		*fcb;
	SFTAB	*sft;
	
	short	fpno;
    short	SFno;
	short	cmd;
	short	entno;
	int		TDno;
	char	FLDtype;

	//printf("leopen(%s), command = %d\n",isPipe ? "true" : "false", symbol);
	SFno	= getsfmem();
	sft		= SFARR(SFno);
	
	cmd		= symbol;								// Save previous symbol which is the command type
    symbol	= getsym();
    if ( symbol != 2230 && symbol != 2220 )			// string literal or variable name
    {
		if ( symbol == 930 )
			loaderr(50, sym);                       // "missing file/pipe name"
		else
			loaderr(51, sym);                       // "invalid file/pipe name"
		return 0;
    }
    
	StrVal = (char *)mmalloc(syml + 1);
    cdbcpystr(StrVal, sym, 0);
    if ( symbol == 2230 && nxtch() != '.' )			// *** should really test for conflict with variable name ***
		isdef(StrVal);
	
	entno = loadexp(0, 1);
    if ( entno )
    {
		gettf(entno, &TDno, &FLDtype);
        if ( FLDtype != 'C' )
        {
			loaderr(51, StrVal);					// "invalid file/pipe name"
            return 0;
		}
        mfree_0(StrVal);
        
		fcb		= _files;
        fpno	= 0;
		while ( no_files > fpno && fcb->FCBFlags )	// find end of FCB tables, or first unused FCB entry
		{
			fpno++;
            fcb++;
		}
		if ( fpno == no_files )				// true if beyond last FCB
		{
			newfile();						// create space for new FCB, bump 'no_files'
			assert(fpno < no_files);		// sanity check
			fcb = &_files[fpno];
		}
        
		sft->AliasEnt = entno;

		switch (cmd)
		{
		case 430:							// openapp
			fcb->FCBFlags	= 0x0004u;
            sft->cmd_flags	= 0x0014u;
			break;
		case 1120:							// popenio
			fcb->FCBFlags	= 0x0003u;
            sft->cmd_flags	= 0x0013u;
			break;
		case 530:							// openout
		case 1150:							// popenout
			fcb->FCBFlags	= 0x0002u;
			sft->cmd_flags	= 0x0012u;
			break;
		case 460:							// openin
		case 1110:							// popenin
			fcb->FCBFlags	= 0x0001u;
            sft->cmd_flags	= 0x0011u;
			break;
		}

		if ( isPipe )						// If isPipe == 1, this is a Pipe type command: popenin, popenio, popenout
        {
			fcb->FCBFlags	|= 0x0040u;
            sft->cmd_flags	|= 0x0040u;
		}
		entno = loadexp(0, 1);				// get filename descriptor. Can be a string variable, or string literal 
        if ( entno )
        {
			gettf(entno, &TDno, &FLDtype);
            if ( FLDtype == 'C' )
            {
				sft->FileNameEnt = entno;
                return SFno;				// exit_success
			}
			loaderr(55, sym);				// "file name / pipe command must be charac"..
		}
	}
    return 0;
}

#endif
