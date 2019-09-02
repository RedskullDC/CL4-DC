//#define __USE_LARGEFILE64
//#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE 1
//#define _LARGE_FILE_API

#include <stdio.h>		// printf etc.
#include <stdlib.h>		// getenv, putenv
#include <fcntl.h>
//#include <curses.h>		// for terminal handling
#include <termcap.h>
#include <unistd.h>
#include <stdint.h>
#include <float.h>		// for FLT_MIN, FLT_MAX
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h> /* for mode_t */
#include <ctype.h>
#include <time.h>       // for struct tm
#include <sys/times.h>  // for times()
#include <setjmp.h>     /* jmp_buf, setjmp, longjmp */

//#define DEBUG_DUMP		// allow dump routines to send tables to stdio
//#define DEBUG_OUTPUT		// allow dump routines to send various stuff stdio
//#define CL4_STRICT		// Only allow canonical CL4 functionality, no extensions
//							   ** see also - extendDC command line flag.

// Test some undocumented CL4 lib routines
#include "DBdefs.h"
#include "funcdefs.h"
#include "lvarnames.h"
#include "tblglo.c"
#include "cl4.h"

#ifdef DEBUG_DUMP
#include "UTIL_FUNCTIONS.h"
#endif

// source files in same order as clcomp
#include "loadent.c"
#include "dump_routines.c"
#include "dumpenc.c"
#include "leblock.c"
#include "legetf.c"
#include "lemaint.c"
#include "leformat.c"
#include "leopenlib.c"		// DC extension
#include "lelibcall.c"		// DC extension
#include "loadstate.c"
#include "leaccept.c"
#include "lecopy.c"
#include "lecreate.c"
#include "ledefine.c"
#include "lehdtxt.c"
#include "leif.c"
#include "leincl.c"
#include "lejoin.c"
#include "leopen.c"
#include "leread.c"
#include "leredisp.c"
#include "leprint.c"
#include "lerange.c"
#include "lerfile.c"
#include "leselect.c"
#include "leset.c"
#include "lewhile.c"
#include "getadwid.c"
#include "getcol.c"
#include "getprmpt.c"
#include "getrow.c"
#include "loadexp.c"
#include "gettfexp.c"
#include "prdebug.c"
#include "getpt.c"
#include "putconst.c"
#include "loaderr.c"
#include "getsym.c"
#include "loadsym.c"
#include "loadvars.c"
#include "gettf.c"
#include "intexp.c"
#include "getkflds.c"
#include "getxflds.c"
#include "getident.c"
#include "lefork.c"
#include "getmem.c"		// allocate all structures
#include "gettnode.c"
#include "lepause.c"
#include "leeop.c"
#include "leescape.c"
#include "loadresv.c"
#include "leulck.c"
#include "dpexp.c"
#include "lefresh.c"
#include "prpipe.c"
#include "fixbname.c"
#include "lemsg.c"
#include "lesleep.c"
#include "lesrand.c"
#include "newdtab.c"
#include "newbtab.c"
#include "newitab.c"
#include "newltab.c"
#include "newttab.c"
#include "isdef.c"
#include "findfno.c"
#include "findtd.c"
#include "gettype.c"
#include "termcap.c"
#include "dates.c"
#include "times.c"
#include "round.c"		// OK to 7 decimal places, then gets stuffed up!
#include "compare.c"
#include "typedp.c"
#include "zap.c"
#include "getflags.c"
#include "closedb.c"
#include "cmpbuf.c"
#include "getdwid.c"
#include "getclver.c"
#include "getosbuf.c"
#include "trim.c"
#include "chkpath.c"
#include "crc.c"
#include "cllogerr.c"
#include "getevar.c"
#include "getfmode.c"
// clcomp specific includes
#include "getfile.c"
#include "setvar.c"
#include "keyname.c"
#include "basename.c"
#include "cafuncs.c"		// only required if DBrename option is specified
#include "extstr.c"			// only required if DBrename option is specified
#include "getdirname.c"
#include "guname.c"
#include "mstrings.c"
#include "error.c"
#include "print.c"
#include "dbopen.c"
#include "shutdb.c"
#include "dbclos.c"
#include "dbspac.c"
#include "dbsize.c"
#include "usage.c"
#include "identi.c"
#include "releas.c"   // need to understand sallocbuf better
#include "mktabl.c"
#include "rdhdr.c"
#include "getatt.c"
#include "getsha.c"
#include "chkind.c"
#include "relsha.c"
#include "oshare.c"
#include "dbglob.c"
#include "getr.c"
#include "keysiz.c"
#include "lockr.c"
#include "update.c"
#include "ulock.c"
#include "uptupl.c"
#include "newexp.c"
#include "nextr.c"
#include "seq.c"
#include "picknx.c"		// testing
#include "restri.c"
#include "rowsiz.c"
#include "rtokey.c"
#include "rtotup.c"
#include "imerge.c"
#include "stepfw.c"
#include "getpat.c"
#include "tuptor.c"
#include "domtox.c"			// Needs more work
#include "mstoxl.c"
#include "mstod.c"
#include "xtodom.c"			// Needs more work
#include "sync.c"
#include "balanc.c"
#include "beyond.c"
#include "bound.c"
#include "cpyseq.c"
#include "cpytuc.c"
#include "fclear.c"
#include "fretre.c"
#include "ltoms.c"
#include "match.c"
#include "mstol.c"
#include "mstoxi.c"
#include "negate.c"
#include "rindex.c"
#include "rmlock.c"
#include "scanpg.c"
#include "split.c"
#include "spread.c"
#include "xitoms.c"
#include "dtoms.c"
#include "xltoms.c"
#include "addpag.c"
#include "clrtab.c"			// _sparetree()
#include "cmpkey.c"
#include "freshn.c"
#include "ifetch.c"
#include "ipos.c"
#include "mkfree.c"
#include "modpag.c"
#include "promot.c"
#include "redist.c"
#include "relnod.c"
#include "repos.c"
#include "rhead.c"
#include "whead.c"
#include "addtup.c"
#include "captur.c"
#include "chkite.c"
#include "cmpatt.c"
#include "delpag.c"
#include "diffke.c"			//untested, needs work
#include "fwidth.c"
#include "getnod.c"
#include "movele.c"
#include "moveri.c"
#include "numsz.c"
#include "putnod.c"   //** Need to work out rdata/roffset use of NODE_1 structure
#include "writep.c"
#include "lock.c"
#include "diffat.c"
#include "datapg.c"
#include "readpg.c"
#include "chkpag.c"		// requires more testing
#include "indexp.c"
#include "getpg.c"
#include "itoptr.c"
#include "itosz.c"
#include "itoms.c"
#include "ealloc.c"
#include "mktmp.c"
#include "derror.c"
#include "addexp.c"
#include "frelst.c"
#include "alloc.c"
#include "cpybuf.c"
#include "cpystr.c"
#include "fill.c"
#include "readf.c"
#include "scnbuf.c"
#include "onexit.c"
#include "putstr.c"
#include "lenstr.c"
#include "cmpstr.c"
#include "owildm.c"
#include "bupsem.c"

// below not called in clcomp???

// files in generic source location
//#include "gettype.c"
//#include "pickpr.c"		// testing
//#include "prevr.c"
//#include "newjun.c"
//#include "putr.c"
//#include "delr.c"
//#include "stepba.c"
//#include "dfetch.c"
//#include "dpos.c"
//#include "dmerge.c"
//#include "mstoi.c"
//#include "ulockr.c"


//#include "util.c"			// debug routines 

char	ename[128];
//char	_ebuf[8192];
//char	_obuf[8192];

main(int argc, char **argv)
{
	FLDdesc	*fld;
	char	v55[144];
	char	v56[132];
	char	*v7;

	int		v6;
	int		NumErrors;
	int		NumVars;
	int		jmp_error;
	int		Verbose = 0;

	//printf("sizeof(reswords) = %d\n",sizeof(reswords)/sizeof(ReservedWord));

	//setbuf(stderr, _ebuf);	// stderr defaults to *no* buffering
	//setbuf(stdout, _obuf);	// stdout default to line buffering
	_pname = (char *)*argv;
	if (!getflags(&argc, &argv,	"D,y,I*,n,v,C,r*,s,x,E", &codeb, &yflag, &incl_dir, &noWarnings, &Verbose, &notabchks, &dbrename, &tpenc, &exact, &extendDC, 0) || !argc)
	{
		printf("usage: %s (%s) [options] ent_file[.ent]\n", clbasename(_pname), getclver());
        printf("options:\n");
        printf("  -D               debug mode\n");
        printf("  -I {dir}         find include files in directory {dir}\n");
        printf("  -n               suppress warning messages\n");
        printf("  -r {db1=db2,..}  process enc file with database db2 in place of db1\n");
        printf("  -s               compile to portable enc file\n");
        printf("  -v               verbose\n");
        printf("  -x               exact match on field names\n");
        printf("  -y               display widths of 'D' type input/output fields\n");
        printf("  ===================================================================\n");
        printf("  -C               ignore CRC of tables. **Undocumented**\n");
        printf("  -E               enable DC extensions to CL4. **Gefahr!!**\n");
        exit(1);
	}
	//getflags() advances argc and argv past the flags 
	
	//printf("extendDC = %d\n",extendDC);

	//printf("Hello %d\t %s\n",argc, argv[argc-1]);

	v7 = getenv("LANG");
	if (!v7 || !*v7)
		putenv("LANG=en_AU");	// default to en_AU if no locale info found
	getcwd(cwd, 128);
	termcap();
    loadMonthArrays(0);			// load months, but don't init CLvars
    dateCheck(0);

	v6 = strlen(*argv);
//	if ( v6 > 3 && cmpbuf((char *)&(*argv)[v6 - 4], ".ent", 4u) )
//		(*argv)[v6 - 4] = 0;

	v7 = &(*argv)[v6 - 4];
	if ( v6 > 3 && cmpbuf(v7, ".ent", 4u) )
		*v7 = 0;

	cdbcpystr(v55, (char *)*argv, ".ent", 0);     // ensure filename passed has ".ent" on the end
	v7 = chkpath(v55, 0, "CLEPATH", 0, 256);
	cdbcpystr(ename, v7, 0);
	ename[strlen(ename) - 4] = 0;

	if ( !yflag )	// display width of 'D' fields
		eprint("cl (%s) compiling %s:\n", getclver(), *ename ? ename : *argv);

	// ensure all mandatory table pointers are initialised.
	ttab = (TDesc *)mmalloc(0);
	while ( no_ttabs < 3 )				// 3 system tables 0-2		"tables","fields","locks"
		newttab();
	
	dtab = (DBase *)mmalloc(0);
	btab = (BTAB *)mmalloc(0);
	_files = (FCB *)mmalloc(0);

	jmp_error = setjmp(sjbuf);			// 0 on first run though. Non-Zero if we had a seg-violation causing crash!
	if (!jmp_error)
	{	
		NumErrors = loadent(ename);		// Load in the ENT file and compile program
		if (!NumErrors)					// Compilation was successful!
		{	
			if (!yflag)					// only checking date fields
			{
				NumVars = 0;
				while((fld = getvars(NumVars), fld) )
				{
					if ( !noWarnings && !(fld->FLDstat & fld_VAR_USED) )	// 0x0010  used to indicate variable accessed in clcomp
						eprint("warning: variable [%s] not used\n", fld->FLDname);
					//if ( fldvar->field_15 )
					//	LOWORD(NumVars) = NumVars + fldvar->field_15;	// Nothing appears to set this field?
					NumVars++;
				}
				tdlocks();							// Show any possible table conflicts/issues
				fflush(stderr);
				cdbcpystr(v56, ename, ".enc", 0);
				NumErrors = dumpenc(v56);			// create the enc file, may return file related errors
			}
		}
		if (NumErrors)	// possible to get errors when dumping the file
		{
			eprint("n_errs = %d\n", NumErrors);
			Verbose = 0;
		}
	}
	else
		Verbose = 0;
//--------------------
	if ( codeb )
		prvars(1);							// show variable names as tree structure

	if (Verbose)
	{
		print("No. of variables = %d\n", NumVars);
        prvars(0);							// show variable names normally
		ProgDump();							// Dump program details
		prentab();
		prtds();                            // Show Block details
	}

	closedb(1);
	
	if ( jmp_error < 0 )	// program crashed out through a longjmp()
		cllogerr(-jmp_error, ename, 0, 0);
	
	if ( !jmp_error )
		return NumErrors;	// exit success
	else
		return 99;
}

VARTREE *chknode(VARTREE *a1, char *TableName, short MaxLen)
{
	VARTREE *v3;
    int v4;

    if ( !a1 || (v3 = chknode(a1->VarPrev, TableName, MaxLen)) == 0	&& (v4 = compare(TableName, a1->VarName), v3 = a1, v4) && (v3 = chknode(a1->VarNext, TableName, MaxLen)) == 0 )
		v3 = 0;
	return v3;
}

void prentab()
{
	ENTAB	*entab;
	short	count;
	short	i;

	count = enarr.NextElement;
	entab = enarr.TableAddr;
	if (count)
	{
		print("ENode Table\n");
		print("ENT# Type     Opcd  Rec   <-   ->\n");
		for ( i = 0; i < count; i++,entab++ )
		{
			print("%4d %4d ", i+1, entab->entype);
			switch (entab->entype)
			{
			case 1:
			case 2:
				print("   x%04X %4d ",(unsigned short)entab->TTno, entab->RecNo);
				break;
			case 4:
				print ("   [%8f] ",*(float *)&entab->TTno);		// DC extension!
				break;
			case 8:
				print ("   [%8d] ",*(int *)&entab->TTno);
				break;
			case 16:
				print ("   [%8s] ",(char *)&entab->TTno);
				break;
			default:
				print("              ");
				break;
			
			}
			print("%4d %4d\n",entab->enleft, entab->enright);
		}
		fflush(stdout);
	}

}

void tdlocks()
{
	TDesc	*TTptr;
	TDesc	*LastTTptr;
	TDesc	*TTptr2;
	FLDdesc *fld;
	VARTREE	*v8; 
	char	*Alias;

	short	v0;
	short	v10;
	bool	v14;

    LastTTptr = &ttab[no_ttabs - 1];
	for ( TTptr = ttab; TTptr <= LastTTptr; TTptr++ )
    {
		v0 = TTptr->TDlocked;
        if ( v0 & ttl_LOCK )
        {
			// lock on table, but no "put" or "delete" commands on the table
			if ( !(v0 & ttl_PUT) && !(v0 & ttl_DEL) )
				eprint("warning: possible redundant lock on %s %s '%s'\n", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);
			
			// look for any tablename contention which have locks as well!
			for ( TTptr2 = ttab; TTptr2 <= LastTTptr; TTptr2++ )
            {	
				if ( TTptr2->TDlocked & ttl_LOCK && TTptr != TTptr2 && TTptr->DBnumber == TTptr2->DBnumber && !compare(TTptr->TableName, TTptr2->TableName) )
					eprint("warning: possible lock contention - %s %s '%s' and '%s'\n", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias, TTptr2->TableAlias);
			}
		}
		// no lock on the table, we *didn't* create the table, and there are "put" or "delete" commands on the table, flag it!
        else if ( !(v0 & ttl_CREATE) && (v0 & ttl_PUT || v0 & ttl_DEL) )
			eprint("WARNING: no lock when updating %s %s '%s'\n", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);

//==================================================================
        if ( TTptr->TableName[0] )
        {
			for ( fld = TTptr->TTfields; fld->FLDelemID; ++fld )
            {
				if ( fld->FLDtype == 'S' || fld->FLDtype == 'K' )
                {
					TTptr2 = ttab;
                    v14 = false;
                    while ( TTptr2 <= LastTTptr )
                    {
						if ( TTptr2->TableName[0] && TTptr->DBnumber == TTptr2->DBnumber )
							v14 = (compare(fld->FLDname, TTptr2->TableName) == 0);

						if ( v14 )
							break;
						TTptr2++;
					}
					if ( !v14 && (TTptr->TDlocked & ttl_PUT && fld->FLDtype == 'K' || TTptr->TDlocked & ttl_DEL) )
						eprint("warning: %s %s %s table '%s' not open\n", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, (fld->FLDtype == 'S') ? "set" : "key" , fld->FLDname);
				}
			}
			// check for variable name/tablename conflicts
			v8 = chknode(troot, TTptr->TableName, strlen(TTptr->TableName) + 1);
			if ( v8 )
				eprint("warning: table and variable conflict [%s,%s]\n", TTptr->TableName, v8->VarName);
		}
		// ditto for any Alias/variable name conflicts
        if ( TTptr->TableAlias[0] )
        {
			v8 = chknode(troot, TTptr->TableAlias, strlen(TTptr->TableAlias) + 1);
            if ( v8 )
				eprint("warning: alias and variable conflict [%s,%s]\n", TTptr->TableAlias, v8->VarName);
		}
//=================================================================
		Alias = TTptr->TableAlias;
        v10 = strlen(Alias);
        TTptr2 = TTptr + 1;
        while ( v10 && TTptr2 <= LastTTptr )
        {
			if ( cmpbuf(Alias, TTptr2->TableAlias, v10 + 1) )	// also compares the trailing \0 byte to ensure no partial matches
				eprint("warning: duplicate alias [%s] in %s %s and %s %s\n", Alias, dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, dtab[TTptr2->DBnumber].FullDBname, TTptr2->TableName);
			TTptr2++;
		}
	}
}

void ProgDump()
{
	TDesc	*TTptr;
	PTAB	*ptab;
	ENTAB	*entb;
	RTAB	*rtab;
	XFTAB	*xftb;
	SFTAB	*sftab;
	XTAB	*xtab;
	
	int		OpCode;
	short	PTno = 0;

	//PTno Line OpCd\tInstruction\n"
	//   0   10  200	testn[N]  (=) ~56756767~
	//   1   12  200	testn2[N]  (=) testn[N]  ([7]%) ~1000000~

	print("PTno SrcL OpCd\tInstruction\n");
	while ((ptab = PTARR(getptabp(PTno)),ptab->OpCode))
    {
		print("%4d %4d %4d\t", PTno, ptab->SrcLineNo, ptab->OpCode);
        fflush(stdout);
		OpCode = ptab->OpCode;
		switch (OpCode)
		{

		case 500 ... 598:							// All file operations
			rtab = RTARR(ptab->TABno);
			TTptr = &ttab[rtab->TTno];
			if ( rtab->OpType == rtb_SELECT )		// 0x0080
				print("select %s %s '%s'", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);

			else if ( rtab->OpType & rtb_GET )		// 0x0001
				print("get %s %s '%s'", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);

			else if ( rtab->OpType & rtb_GETNEXT )	// 0x0002
				print("getnext %s %s '%s'", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);

			// not in real clcomp:
			else if ( rtab->OpType & rtb_GETPREV )	// 0x0004
				print("getprev %s %s '%s'", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);

			else if ( rtab->OpType & rtb_PUT )		// 0x0008
				print("put %s.%s '%s'", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);

			else if ( rtab->OpType & rtb_DELETE )	// 0x0010
				print("delete %s %s '%s'", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);
				
			else if ( rtab->OpType & rtb_CLEAR )	// 0x0020
				print("clear %s %s '%s'", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);
				
			else if ( rtab->OpType & rtb_REPLACE )	// 0x0800		// the *EVIL* replace command......
				print("replace '%s'", TTptr->TableAlias);

			//----------------
			if ( rtab->LockType & rtb_LOCK )		// 0x0200
				print(" lock");
			else if ( rtab->LockType & rtb_LOCKR )	// 0x0400
				print(" lockr");
						 
			if ( rtab->WhereEXP )
			{
				print(" where ");
				prtfid(rtab->WhereEXP);
			}
			
			else if ( rtab->OpType & rtb_REPLACE )	// 0x0800 Mutually exclusive with "where" and "all" 
				prrt(rtab->NextRT);

			else if ( rtab->OpType & rtb_SEL_ALL )	// 0x0100
				print(" all");
			break;

		case 720:
			print("do (block %d) ", ptab->Operand);
			if ( ptab->TABno )
				print("%s", &strarr.StringTable[ptab->TABno - 1]);
			break;

		case 880:
			print("sleep ");
			if ( ptab->TABno )
				prtfid(ptab->TABno);
			break;

		case 882:
			print("exit_block");
			break;

		case 884:
			print("exit_do");
			break;
		
		case 886:
			print("exit_process");
			break;

		case 888:
			print("restart_block");
			break;

		case 910:
			print("unlock ");
			if ( ptab->Operand )
			{
				TTptr = &ttab[ptab->Operand];
				print("%s %s,%s", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);
			}
			else
				print("all");
			break;

		case 950:
			print("break");
			break;

		case 999:
			print("end");
			break;

		case 900:
			print("print ");
			prpf(ptab->TABno);
			break;
		
		case 730:
			print("redisplay ");
			prrd(ptab->TABno);
			break;
		
		case 750:
			print("fork ");
			if ( ptab->TABno )
				prtfid(ptab->TABno);
			break;

		case 760:
			print("srandom(");
			if ( ptab->TABno )
				prtfid(ptab->TABno);
			print(")");
			break;

		case 800:
			xftb = XFARR(ptab->TABno);
			if ( xftb )
			{
				TTptr = &ttab[xftb->TTno_to];
				print("copy %s %s '%s' from ", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);
				TTptr = &ttab[xftb->TTno_from];
				print("%s %s '%s'", dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);
			}
			break;

		case 850:
		case 860:
			sftab = SFARR(ptab->TABno);
			prpipe(sftab);
			break;

		case 400:						// not sure this is actually reachable?
			print("no-op");
			break;

		case 410:								// *** DC Extension ***
			print("openlib( ");
			prtfid(ptab->TABno);
			print(")");
			break;

		case 420:								// *** DC Extension ***
			print("libcall( ");
			prtfid(ptab->TABno);
			print(")");
			break;

		case 320:
			print("continue (%d)", ptab->Operand);
			break;

		case 330:
			print("end_loop (%d)", ptab->Operand);
			break;

		case 300:
			print("if ");
			prtfid(ptab->TABno);
			print(" (goto %d)", ptab->Operand);	// go to this PTAB if condition false
			break;

		case 310:
			print("while ");
			prtfid(ptab->TABno);
			print(" (goto %d)", ptab->Operand);	// go to this PTAB if condition false
			break;

		case 265:
			print("no_form");
			break;

		case 140:
			print("clear_eos");
			break;

		case 130:
			print("clear_eol");
			break;

		case 200:		// OpCode 200 is assignment.  ie.  X = Y 
			prcalc(ENARR(ptab->TABno));
			break;

		case 250:
			print("refresh ");			// no 'screen' name is legal
			if ( ptab->TABno )
				print("%s", &strarr.StringTable[ptab->TABno - 1]);
			break;

		case 260:
			print("screen ");			// no 'screen' name is legal
			if ( ptab->TABno )
				print("%s", &strarr.StringTable[ptab->TABno - 1]);
			break;

		case 600:
			print("format %s ", btab[ptab->Operand].BlockName);
			prtfid(ptab->TABno);
			break;

		case 700:
			print("goto %d", ptab->Operand);
			break;

		case 450:
			print("create: ");
			prkx(KXARR(ptab->TABno));
			break;

		case 1:		// 1 = "accept" field
		case 3:		// 3 = "head" 
		case 4:		// 4 = "text"
			xtab = XTARR(ptab->TABno);
			switch (OpCode)
			{
			case 1:
				prxt(xtab, false);
				break;
			case 3:
				print("head ");
				prxt(xtab, true);
				break;
			case 4:
				print("text ");
				prxt(xtab, true);
				break;
			}
			break;

		case 870:
			print("pause ");
			if ( ptab->TABno )					// pause with no variable following is legal
			{
				entb = ENARR(ptab->TABno);
				if ( entb->enleft )				// Pause with two variables following
				{
					prtfid(entb->enleft);
					print(" ");
					prtfid(entb->enright);
				}
				else							// pause with one variable following
				{
					prtfid(ptab->TABno);
				}
			}
			break;

		case 270:
			print("message ");
			if ( ptab->TABno )
			{
				entb = ENARR(ptab->TABno);
				prtfid(entb->enleft);
				print(" ");
				prtfid(entb->enright);
			}
			break;

		case 630 ... 698:
			prset(ptab);						// code in real clcomp is incorrect. set csv 630 ,dos 640 are unreachable!
			break;

		default:
			//if ( (unsigned short)(ptab->OpCode - 650) <= 48u )	// code in real clcomp is incorrect. set csv 630 ,dos 640 are unreachable!
			//if ( ptab->OpCode >= 630 && ptab->OpCode <= 698 )
			//	prset(ptab);
			break;
		}

        print("\n");
        PTno++;
	}
}

