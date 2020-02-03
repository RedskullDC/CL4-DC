//#define __USE_LARGEFILE64
//#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE 1
//#define _LARGE_FILE_API

//#define NDEBUG		// uncomment to remove asserts from code
#define clenter			// used for some conditional compilation

#include <stdio.h>		// printf etc.
#include <stdlib.h>		// getenv, putenv
#include <fcntl.h>
//#include <curses.h>		// for terminal handling
#include <unistd.h>
#include <stdint.h>
#include <string.h>		// for void *memset(void *s, int c, size_t n);
#include <stdbool.h>
#include <pthread.h>
#include <locale.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h> /* for mode_t */
#include <ctype.h>
#include <time.h>       // for struct tm
#include <sys/times.h>  // for times()
#include <signal.h>		// for signal() calls
#include <sys/signal.h>  // for sighandler()
#include <setjmp.h>     /* jmp_buf, setjmp, longjmp */
#include <dlfcn.h>		// dynamic libs		

#include "UTIL_FUNCTIONS.h"
// Test some undocumented CL4 lib routines
#include "DBdefs.h"
#include "lvarnames.h"
#include "funcdefs.h"
#include "alloc.c"
#include "derror.c"

#include "b64.c"			// base64 encoding from http://base64.sourceforge.net, but modified.
#include "blowfish.c"		// includes blowfish.h
#include "encrypt.c"		// includes encrypt.h
#include "cpyseq.c"
#include "getdirname.c"
#include "getevar.c"
#include "getftf.c"
#include "chkpath.c"		//needs work
#include "getresv.c"
#include "pcreate.c"
#include "rcheck.c"
#include "ppause.c"
#include "gettdfno.c"
#include "getexpr.c"
#include "getyx.c"
#include "buftott.c"
#include "getetype.c"
#include "psleep.c"
#include "psrand.c"
#include "setvar.c"
#include "pset.c"
#include "crc.c"
#include "gnname.c"
#include "fldtobuf.c"
#include "getdwid.c"
#include "calculate.c"
#include "redisp.c"
#include "prfield.c"
#include "rdnxt.c"
#include "rdprev.c"
#include "tdclose.c"
#include "pedit.c"
#include "pfile.c"
#include "prdebug.c"
#include "chkesc.c"
#include "toscreen.c"
#include "termcap.c"
#include "clintr.c"
#include "clhintr.c"
#include "jcheck.c"
#include "modfld.c"
#include "dispfld.c"
#include "disprec.c"
#include "ploop.c"
#include "dscreen.c"
#include "isalist.c"
#include "pcount.c"	// not called in clenter
#include "argenv.c"
#include "reexec.c"
#include "recomp.c"
#include "pattern.c"
#include "oformat.c"
#include "coord.c"
#include "getpt.c"

#include "clfork.c"
#include "flagcbd.c"	//used by pfork
#include "chklt.c"
#include "pfork.c"	// Needs work on the byte division
#include "cafuncs.c"
#include "extstr.c"

#include "msquee.c"
#include "append.c"
#include "mvref.c"
#include "mvheap.c"
#include "merge.c"
#include "loadr.c"
#include "join.c"
#include "sift.c"
#include "pflush.c"
#include "hflush.c"
#include "rlist.c"
#include "fldxfe.c"
#include "sameke.c"
#include "testr.c"
#include "commit.c"
#include "sflush.c"
#include "salloc.c"
#include "compose.c"


#include "guname.c"
#include "newexp.c"
#include "newjun.c"
#include "identi.c"
#include "keysiz.c"
#include "rowsiz.c"
#include "getatt.c"
#include "getsha.c"
#include "delhier.c"
#include "gettype.c"
#include "bldexpr.c"
#include "rectott.c"	//needs more work around clround()
#include "tttorec.c" 
#include "owildm.c"
#include "repkey.c"
#include "rdkey.c"
#include "reprec.c"
#include "ealloc.c"
#include "putstr.c"
#include "findtd.c"
#include "seq.c"
#include "cmpstr.c"
#include "dbglob.c"
#include "getr.c"
#include "rtokey.c"
#include "lockr.c"
#include "fwidth.c"
#include "fill.c"
#include "fretre.c"
#include "frelst.c"
#include "cldelput.c"	// log structures different to libcl4. needs more work.
#include "dbsize.c"
#include "dbspac.c"
#include "newdtab.c"
#include "newttab.c"
#include "isnumber.c"
#include "openscr.c"
#include "getosbuf.c"
#include "relnod.c"
#include "clrtab.c"			// _sparetree()
#include "usage.c"
#include "closedb.c"
#include "rdrec.c"
#include "tdtotd.c"
#include "ulock.c"
#include "ulockr.c"
#include "chkind.c"
#include "cpytuc.c"
#include "bupsem.c"
#include "nextr.c"
#include "getpat.c"
#include "indexp.c"
#include "getpg.c"
#include "datapg.c"
#include "chkpag.c"		// requires more testing
#include "itoptr.c"
#include "scanpg.c"
#include "itosz.c"
#include "readpg.c"
#include "scnbuf.c"
#include "readf.c"
#include "rhead.c"
#include "cmpatt.c"
#include "numsz.c"
#include "rindex.c"
#include "prevr.c"
#include "tuptor.c"
#include "rtotup.c"
#include "echknum.c"
#include "reply.c"
#include "getwid.c"
#include "pfresh.c"
#include "getnod.c"
#include "reffile.c"
#include "mkfree.c"
#include "lock.c"
#include "picknx.c"		// testing
#include "pickpr.c"		// testing
#include "match.c"
#include "beyond.c"
#include "fclear.c"
#include "cmpkey.c"
#include "bound.c"
#include "ifetch.c"
#include "dfetch.c"
#include "ipos.c"
#include "dpos.c"
#include "dmerge.c"
#include "imerge.c"
#include "addtup.c"
#include "update.c"
#include "freshn.c"
#include "sync.c"
#include "addpag.c"
#include "delpag.c"
#include "uptupl.c"
#include "modpag.c"
#include "chkite.c"
#include "captur.c"
#include "balanc.c"
#include "rmlock.c"
#include "dbclos.c"
#include "stepba.c"
#include "stepfw.c"
#include "putnod.c"   //** Need to work out rdata/roffset use of NODE_1 structure
#include "whead.c"
#include "writep.c"
#include "repos.c"
#include "spread.c"
#include "split.c"
#include "relsha.c"
#include "promot.c"
#include "redist.c"
#include "movele.c"
#include "moveri.c"
#include "diffat.c"
#include "diffke.c"			//untested, needs work

#include "elist.c"
#include "delr.c"
#include "putr.c"
#include "cllogerr.c"
#include "error.c"
#include "dbopen.c"
#include "oshare.c"
#include "onexit.c"
#include "shutdb.c"
#include "rdhdr.c"
#include "mktmp.c"
#include "mktabl.c"
#include "newstab.c"
#include "prerr.c"


// conversion routines follow
#include "domtox.c"			// Needs more work
#include "xtodom.c"			// Needs more work
#include "mstoi.c"
#include "mstol.c"
#include "mstod.c"
#include "mstoxi.c"
#include "mstoxl.c"
#include "xitoms.c"
#include "xltoms.c"
#include "itoms.c"
#include "ltoms.c"
#include "dtoms.c"
#include "addexp.c"
#include "negate.c"


//#include "zap.c"
#include "sutimes.c"
#include "round.c"		// OK to 7 decimal places, then gets stuffed up!
#include "print.c"
#include "lenstr.c"
#include "cpybuf.c"
#include "compare.c"
#include "rfree.c"
#include "recsiz.c"
#include "trim.c"
#include "power.c"
#include "typedp.c"
#include "restri.c"
#include "releas.c"   // need to understand sallocbuf better
#include "getclver.c"
#include "times.c"
#include "tty.c"
#include "keydebug.c"
#include "cllock.c"
#include "getattr.c"

#include "mstrings.c"
#include "getflags.c"
#include "cpystr.c"
#include "cmpbuf.c"
#include "key.c"
#include "keyname.c"
#include "process.c"
#include "bind.c"
#include "basename.c"
#include "lex.c"
#include "tblglo.c"
#include "loadenc.c"
#include "pblock.c"
#include "eafr.c"
#include "abcd.c"		//needs more work
#include "exptobuf.c"	// still needs more work.

#include "cgi.c"		// cgi/web functionality 
#include "isfname.c"
#include "funs.c"
#include "evaliex.c"
#include "dates.c"
#include "prscreen.c"
#include "linput.c"
#include "convstr.c"
#include "cnvstr.c"			// handler for sconv()
#include "getfmode.c"
#include "linpesc.c"
#include "getfile.c"
#include "ulckrec.c"

// DC extension
#include "popenlib.c"
#include "plibcall.c"

void setCL_ERROR(int ErrorCode)
{
  sprintf(buf_0, "CL_ERROR=%d", ErrorCode);
  putenv(buf_0);
}


int main(int argc, char **argv, char** a3)
{
	TDesc		*TTptr;
	FLDdesc		*fld;
	char		*v28;						// spare char ptr, multi-use
	char		*arg = 0;					// cl program name
	
	int			v30;
	int			count;
	int			v41;
	int			errcount;
	short		TimeOut;
	
	bool		child_log	= false;		// pass process ID to child for logging purposes
	char		FillChar	= 0;			// assume no override of termcap or CLINFO file
	
	TimeOut		= -1;						// default to no timeout
	arg			= 0;
	op			= stdout;
	fullpname	= *argv;
	_pname		= clbasename(*argv);		// extract program name from cmd line
	getcwd(cwd, PATH_MAX);					// runs twice! see below
	setupEnv();
//---------------------------------------------------------------------------------
//	clcgi stuff
	//if (!strncmp(_pname,"clcgi",5) || !strncmp(_pname,"indexs.cgi",10) || !strncmp(_pname,"index.cgi",9))
	if (!strncmp(_pname,"cle.cgi",7) || !strncmp(_pname,"indexs.cgi",10) || !strncmp(_pname,"index.cgi",9))
	{
		isCGI++;		// 1
		if (!strncmp(_pname,"clcgi",5))
			isCGI++;	// 2 when invoked as clcgi?

		putenv("CLSETUP=./CLSETUP");			// ensure sane environment
		putenv("TERM=vt100");
		arg = cgiGetEname();					// default CGI program name to run
		if ( !arg )
		{
			if ( isCGI == 1 )
				arg = "htstart";				// Didn't find a default!, run htstart.enc
			else
			{
				setCL_ERROR(9);
				arg = "clerror";				// Ooops! Something wrong here, exit via clerror
			}
		}
	}
//---------------------------------------------------------------------------------
	if ( !getflags(
          &argc,
          &argv,
          "D,I*,c,d#,f?,k,L##,n,N,p,P,r*,s,t?,T##",
          &lddeb,                               // DEBUG mode
          &incl_dir,                            // Include file directory
          &check,                               // validate checksums
          &TimeOut,                             // timeout delay in seconds
          &FillChar,                            // fill character
          &Key_Debug,                           // debug keydefs and macros
          &lpid,                                // Log File process ID
          &noWarnings,                          // don't print Warnings
          &noLog,                               // do not Log transactions
          &pr_scr,                              // allow screen dumps
          &npr_scr,                             // allow screen dumps with line/col numbers
          &dbrename,                            // dbase rename
          &tpenc,                               // compile to portable enc file 
          &TAB,                                 // TAB char
          &cl_tx,                               // LOGFILE transaction number
          0)
    || !argc && !Key_Debug && !isCGI)
	{
  		printf("build: %s (%s)\n\n", _pname, getclver());
		eprint("usage: %s [options] ent_file [var...]\n", _pname);
		
		eprint("options:\n");
		eprint("  -c               validate checksums on tables\n");
		eprint("  -d {secs}        set timeout delay in seconds\n");
		eprint("  -f {char}        set fill character to {char}\n");
		eprint("  -k               debug keydefs and macros\n");
		eprint("  -L {lpid}        log file process id\n");
		eprint("  -N               do Not log transactions\n");
		eprint("  -p               allow screen dumps\n");
		eprint("  -P               allow screen dumps with line and column numbers\n");
		eprint("  -r {db1=db2,..}  process enc file with database db2 in place of db1\n");
		eprint("  -t {char}        set tab character to {char}\n");
		eprint("  -T {tran}        log file transaction no\n");
		eprint("\n");
		eprint("The following options are passed to the compiler:\n");
		eprint("  -D               debug mode\n");
		eprint("  -I {dir}         find include files in directory {dir}\n");
		eprint("  -n               don't print warnings\n");
		eprint("  -s               compile to portable enc file\n");
		exit(1);	// err_exit
	}
	//eprint("sizeof(BTAB) = %d\n",sizeof(BTAB));
	getcwd(cwd, PATH_MAX);
	if (!isCGI)
		arg	= *argv;			// 'program[.ent/.enc]'
	v28		= getenv("LANG");
	if ( !v28 || !*v28 )
	{
		putenv("LANG=en_AU");
		v28 = "en_AU";
	}
	setlocale(LC_ALL, v28);
	termcap();
	++cldepth;
	if ( Key_Debug )			// not running a program, just testing keydefs and macros
	{
		//v30 = 0;
		v30 = _setjmp(sjbuf);
		if ( !v30 )
		{
			dateCheck(0);
			Key_Debugem();
		}
	}
	else
	{
		if ( TimeOut >= 0 )
			_Yd = TimeOut;
		if ( FillChar )
			*_Yf = FillChar;	// override fill character specified in termcap or clinfo file.
		
		v41 = lenstr(arg);		// strip '.enc' or '.ent' from filename if found
		if ( v41 > 3 )
		{
			v28 = &arg[v41 - 4];
			if ( cmpbuf(v28, ".enc", 4) || cmpbuf(v28, ".ent", 4) )
				*v28 = 0;
		}
//---------------------------------------------------------------------
		cdbcpystr(pname, arg, 0);
		for ( v28 = &pname[lenstr(pname) - 1]; v28 > pname && *v28 != '/'; --v28 )
			;
		if ( v28 != pname )
			++v28;
		cdbcpystr(pname, v28, 0);

		if ( lpid || cl_tx )
			child_log = true;

		if ( !lpid )
			lpid = getpid();
		if ( cl_tx )
		{
			if ( cl_tx < 0 )
				cl_tx = -cl_tx;
		}
		else
			cl_tx = 1;

		if ( noLog )
			setNoLogging();
		stab = (STAB **)mmalloc(0);	// "screen" table holders
		Key_Construct();

//======== load enc file here ==========

		while ( 1 )
		{
			v30 = _setjmp(sjbuf);
			if ( !v30 )
				v30 = loadenc(arg);
			if ( !isCGI || !v30 || !strcmp(arg, "clerror"))
				break;
			setCL_ERROR(v30);			// error! go around again and run the clerror program if exists
			arg = "clerror";
		}

		if ( v30 )						// error, setjmp came back with non-zero value
		{
			closedb(0);
			closetlog(v30);
			if ( isCGI )
				cgiprint(v30);
		}
		else
		{
			if ( check ) // not actually running the program. Just check we can access all Tables
			{
				TTptr = ttab;
				count = 0;
				for ( errcount = 0; count < no_ttabs; TTptr++ )
				{
					if ( TTptr->TTrtd < 0 && !(TTptr->TDlocked & 0x0200) )		// !ttl_CREATE	not a table *we* create
					{
						//v36 = tgetfile(TTptr);
						//printf("check - count = %d, v36 = %d\n",count,v36);
						//v37 = errcount + 1;
						//if ( v36 > -1 )
						//	v37 = errcount;
						//errcount = v37;
						if (tgetfile(TTptr) < 0)
							errcount++;					// bump count of tables we *couldn't* open
					}
					count++;
				}
				closedb(0);
				exit(errcount);
			}
			dateCheck(1);	// force a _check_YD()
			if ( child_log )
			{
				btab->Scrs |= 0x4000u;
				prev_tx = cl_tx;
			}
			argvect = argv;
			envvect = a3;

			// look for any command line params, and copy them into V1-V9			
			if ( argc )
			{
				++argv;
				count = 0;
				fld = &ttab->TTfields[v_V1];					// fld now points at "V1"
				--argc;
				while ( argc && count <= 8 )						// stop at V9
				{
					v41 = lenstr(*argv);
					if (v41 > 127)
						v41 = 128;								// MaxLen of 128 bytes on each cmdline variable
					cpybuf((char *)fld->FLDdata, *argv, v41);	// copy command line vars into V1-V9
					//printf("V%d set to %s\n",count + 1,(char *)fld->FLDdata);
					if ( **argv )
						fld->FLDstat = (fld->FLDstat | fld_DATA_AVAIL) & ~fld_ZERO;	// 0x0010) & 0xFFFB;	// set quick zero, data avail flags
					++fld;
					++count;
					++argv;
					--argc;
				}	
			}
		
			setvar(v_SLINES, _li);								// "SLINES"
			setvar(v_SCOLS, _co);								// "SCOLS"
			setvar(v_MLINE, _li - 1);							// "MLINE"
			setvar(v_MCOL, _sg ? _sg + 1 : 1);					// "MCOL"			_sg = # of standout glitch chars
			setvar(v_DATEFORM, _DF);							// "DATEFORM"
			if ( *_YL )
				rawdisplay = 1;
			else
				rawdisplay = _Y3;
			setvar(v_RAWDISPLAY, (unsigned short)rawdisplay);	// "RAWDISPLAY"
			setvar(v_RAWPRINT, (unsigned short)rawprint);		// "RAWPRINT"

			loadMonthArrays(1);
			//printf("_YD set to %s\n",_YD);
			v30 = _setjmp(sjbuf);
			if ( !v30 )	// v30 is non-zero if an exception has occured while running
			{
				openscr();
				pblock(btab, 0);			// go execute the program!!!
				closelibs();				// DC extension, close any opened dynamic libraries
			}
			signal(SIGTERM, SIG_IGN);
			signal(SIGINT, SIG_IGN);
			signal(SIGHUP, SIG_IGN);
			signal(SIGPIPE, SIG_IGN);
			if ( v30 == 1 )
				v30 = 0;
			closedb(0);		// close all open databases
			closefp();		// close all open pipes/files
			//if ( v30 >= 0 )
			//	v30 = (signed int)*(double *)ttab->TTfields[6].FLDdata;	"EXIT"
			//==
			closetlog(v30);
			clunlock();
			//if ( v30 < 0 )
			//	cllogerr(-v30, pname, 0, 0);
			closescr();
			unloadMonthArrays();	// free memassociated with DateString vars
			//remove_xml_filename();
			Key_Destruct();			// ditto the key macros if loaded
			if ( timing )
			{
				v28 = mstrcpy(arg,".ent",0);
				sutimes(v28, false);			// turns timing off, and print report
				mfree_0(v28);
			}
		}
	}
	exit(v30);	// exit_success if v30 == 0
}

void closelibs(void)
{
	FLDdesc	*fld;
	int		count = 0;

	fld = ttab[0].TTfields;
	while(fld->TDFtype)
	{
		if (fld->FLDstat & fld_LIB)		// flag this variable as holding a library handle see [popenlib()] 
		{
			//printf("lib handle Varno = %d\n", count);
			if (fld->FLDdata)
			{
				//printf("HandleVar = x%08X\n",*(void**)fld->FLDdata);
				dlclose(*(void**)fld->FLDdata);
			}
		}				
		fld++;
		count++;
	}

}