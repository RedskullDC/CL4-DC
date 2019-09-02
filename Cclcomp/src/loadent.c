#ifndef LOADENT_C
#define LOADENT_C

#include <stdio.h>
#include <stdarg.h>			// for var args stuff
#include <ctype.h>          // for islower() and toupper() functions
#include <string.h>
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"		// for flag bits

void chkoelist(ONESC *escp)
{
	BTAB	*bt;
	PTAB	*ptab;
	XTAB	*xtab;

	size_t	MaxLen;
	short	PTno;
	short	CurrBTno;

	for ( ; escp; escp = escp->NextESC )
	{
		MaxLen = lenstr(escp->BlockName) + 1;	// include the \0 in the compare below
		bt = btab;
		CurrBTno = 0;
		if ( no_btabs > 0 )						// global
		{
			//printf("chkoelist #28 - escp->BlockName = %s, bt->BlockName = %s\n",escp->BlockName, bt->BlockName);
			while ( !cmpbuf(escp->BlockName, bt->BlockName, MaxLen) )
			{
				++CurrBTno;
				++bt;
				if ( no_btabs <= CurrBTno )
					 goto LABEL_12;
			}
			//printf("chkoelist #36 - escp->BlockName = %s, bt->BlockName = %s\n",escp->BlockName, bt->BlockName);
			PTno = bt->StartLine;
			while ( (ptab = PTARR(getptabp(PTno)), ptab->OpCode != 950) )	// continue till a break instruction found
			{
				//printf("chkoelist #40 - ptab->OpCode = %d\n",ptab->OpCode);
				if ( ptab->OpCode == 1 && ptab->TABno )	// data accept field
				{
					xtab = &xtarr.TableAddr[ptab->TABno - 1];
					if ( xtab->Flags & xtb_FDF )	// 0x0008u	// FDF First Display Field for a block
					{
						xtab->Flags &= ~xtb_FDF;	// 0xFFF7u;
						break;
					}
				}
				PTno++;
			}
			escp->BlockNo = CurrBTno;
		}
LABEL_12:
		if ( no_btabs == CurrBTno )
			loaderr(74, escp->BlockName);	// 'escape' block name not found
	}
}

int loadent(char *FullPathName)
{
	SCR		**v53;
	PTAB	*ptb;
	PTAB	*ptb2;
	BTAB	*btb;
	BTAB	*btb2;
	char	*IDname;
	char	*v1;
	
	int		PTno;
	size_t	v23;
	short	LineNo;
	short	v35;
	short	NumScreens;
	short	NumFormats;
	short	count;
	short	CurrBTno;
	
	char	FileName[168];
	
	cdbcpystr(FileName, FullPathName, ".ent", 0);
	
	v1 = &FileName[lenstr(FileName) - 1];	// start at end of path
	while (v1 > FileName && *v1 != '/')
		v1--;
	if ( v1 != FileName )					// we hit a '/'
		v1++;								// position after the slash

	// deleted the pre-processing stuff.

	MainFile = fopen64(FileName, "r");
	if ( lenstr(v1) > 140u || !MainFile )
	{
		eprint("ent file not found - %s\n", FileName);
		exit(1);
	}
	FileTree = (INCLUDE *)mmalloc(sizeof(INCLUDE));
	FileTree->FileName = mmalloc(lenstr(FileName) + 1);
	cdbcpystr(FileTree->FileName, FileName, 0);
	FileTree->filePTR = MainFile;
	ltab	= mmalloc(0);
	itab	= mmalloc(0);
	newltab();
	lt		= ltab;		// program label tables
	it		= itab;		// if/while tables
	PTno	= 0;		// PTno is used to count NumPTABs : program steps

	loadvars();			// initialise system vars

	symbol = getsym();
	while ( symbol && !comp_abort )
	{
		while ( symbol && symbol != 820 && !comp_abort )	//820 = "{"  start of block token
		{
			if ( symbol == 660 )		// include directive. Only valid item outside of a block construct.
				leincl();				// leincl() returns pointing at last <CR> after the include statement
			if ( symbol != 930 )		// <CR>
				loaderr(33, sym);		// unexpected symbol
			symbol = getsym();
		}
		if ( symbol && !comp_abort)
		{
			leblock(&PTno);				// returns with symbol pointing at the "}" at the end of the block (token == 1360)
			while ( symbol && symbol != 930 )
				symbol = getsym();
		}
	}
	fclose(MainFile);

	// ENT file has now been read with no syntax errors, or we struck an error (or 2...)
	// Lets go ahead and check it for logical errors, and link all the tables etc.

	PTARR(getptabp(PTno))->OpCode = 950;	// add two 950's to signify end of file
	PTno++;									// doesn't work properly if the ++ is inclued in the macro [expands to *2* ++'s]!! 
	PTARR(getptabp(PTno))->OpCode = 950;

	// fix all links do do{blockname} and goto LABEL: commands
	if ( !comp_errs )
	{
		PTno = 0;
		while( (ptb = PTARR(getptabp(PTno)), ptb->OpCode))
		{
			if ( ptb->OpCode == 720 && ptb->TABno )		// 720 = do block_name
			{
				IDname	= STRARR(ptb->TABno);			// Identifier name. Block or Label name
				v23		= lenstr(IDname) + 1;
				for (btb = btab, count = 0; count < no_btabs ; count++, btb++)
				{
					if ( cmpbuf(IDname, btb->BlockName, v23) )
					{
						ptb->Operand = count;
						break;
					}
				}
				if ( count == no_btabs )		// got to end of btab array and no match
					loaderr(68,IDname);			// 'do/format' block name not found"
			}
			else if ( ptb->OpCode == 700 && ptb->TABno )	// 700 goto handler. 
			{
				IDname = STRARR(ptb->TABno);
				for ( lt = ltab; *lt; lt++ )	// Program Label Tables
				{
					if ( cmpbuf(IDname, (*lt)->LabelName, (*lt)->NameLen ))
					{
						ptb->Operand = (*lt)->PTno;
						ptb->TABno = 0;
						if ( *lt )			// should always be true here!
							goto LABEL_91;	// We found a match before end of the list
						break;
					}
				}
				cdbcpystr(sym, IDname, 0);
				loaderr(11, sym);			// "label not found"
			}
			else if ( ptb->OpCode == 1 && ptb->TABno )	// 1 = accept field, look for a field specific escape
			{
				chkoelist(XTARR(ptb->TABno)->onesc);
			}
LABEL_91:
			PTno++;
		}

		chkoelist(oelist);					// check for global block escapes 

		btb		 = btab;					// lastly, check for block specific stuff: On_exit, On_entry, On_excep, On_delete
		CurrBTno = 0;
		if ( no_btabs <= CurrBTno )
			return comp_errs;
		while ( 1 )
		{
			if ( btb->BlockName[0] )
			{
				if ( btb->StartLine || btb->EndLine )
				{
					v53				= &btb->Screens;
					btb->Screens	= (SCR*)mmalloc(sizeof(SCR));	//(8u);
					LineNo			= btb->StartLine;
					NumScreens		= 0;
					NumFormats		= 0;

					while ( PTARR(getptabp(LineNo))->OpCode != 999 )	// 999 == end
					{
						if ( !*v53 )
							*v53 = (SCR*)mmalloc(8u);
						(*v53)->PT_start = LineNo;
						
						while ( (ptb2 = PTARR(getptabp(LineNo)), ptb2->OpCode != 950)) // break;
						{
							if ( ptb2->OpCode == 600 ) // format
								NumFormats++;
							LineNo++;
						}
						(*v53)->PT_end = LineNo;
						NumScreens++;
						v53 = &(*v53)->NextSCR;
						LineNo++;
					}
					btb->Scrs = NumScreens;
					btb->fmts = NumFormats;

					if ( btb->On_delete )
					{
						for (btb2 = btab,v35 = 0; v35 < no_btabs ; v35++, btb2++)
						{
							if ( btb2->BlockName[0] && cmpbuf(btb2->BlockName, btb->On_delete->BlockName, lenstr(btb2->BlockName) + 1))
							{
								btb->On_delete->BlockNo = v35;
								break;
							}
						}
						if ( no_btabs == v35 )
							loaderr(75, btb->On_delete->BlockName);	// 'on_delete' block name not found
					}
					
					if ( btb->On_exit )
					{
						for (btb2 = btab,v35 = 0; v35 < no_btabs ; v35++, btb2++)
						{
							if ( btb2->BlockName[0] && cmpbuf(btb2->BlockName, btb->On_exit->BlockName, lenstr(btb2->BlockName) + 1))
							{
								btb->On_exit->BlockNo = v35;
								break;
							}
						}
						if ( no_btabs == v35 )
							loaderr(79, btb->On_exit->BlockName);	// 'on_exit' block name not found
					}
					
					if ( btb->On_excep )
					{
						for (btb2 = btab,v35 = 0; v35 < no_btabs ; v35++, btb2++)
						{
							if ( btb2->BlockName[0] && cmpbuf(btb2->BlockName, btb->On_excep->BlockName, lenstr(btb2->BlockName) + 1))
							{
								btb->On_excep->BlockNo = v35;
								break;
							}
						}
						if ( no_btabs == v35 )
							loaderr(80, btb->On_excep->BlockName); //'on_exception' block name not found
					}

					if ( btb->On_entry )
					{
						for (btb2 = btab,v35 = 0; v35 < no_btabs ; v35++, btb2++)
						{
							if ( btb2->BlockName[0] && cmpbuf(btb2->BlockName, btb->On_entry->BlockName, lenstr(btb2->BlockName) + 1))
							{
								btb->On_entry->BlockNo = v35;
								break;
							}
						}
						if ( no_btabs == v35 )
							loaderr(80, btb->On_entry->BlockName); //'on_exception' block name not found  ***error in real CL4 *** should be 'on_entry'
					}
				}
				else
					loaderr(68, btb->BlockName);	// 'do/format' block name not found
			}
			CurrBTno++;
			btb++;
			if ( no_btabs <= CurrBTno )
				return comp_errs;
		}
	}	
	if ( symbol )
	{
		fwrite("too many errors - aborting\n", 1u, 0x1Bu, stderr);
		fflush(stderr);
	}
	return comp_errs;
}

#endif
