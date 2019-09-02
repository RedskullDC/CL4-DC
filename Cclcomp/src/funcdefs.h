#ifndef CL4HDR
#define CL4HDR

FILE	*MainFile;
int		preprocess;
INCLUDE	*FileTree;
LTAB	**ltab;			// LABELS in program code
LTAB	**lt;
short	no_ltabs;
ITAB	*itab;
ITAB	*it;
short	res_v_max;
short	last_var;
//FLDdesc	lvars[63];
VARTREE	*troot;
VARTREE	*rtroot;
short	last_fnd;
short	cur_block;

PTAB_ArrPtr		ptarr;					// PTAB*
RDTAB_ArrPtr	rdarr;					// RDTAB
XTAB_ArrPtr		xtarr;					// XTAB*	Accept fields 
ENTAB_ArrPtr	enarr;					// ENTAB	Expressions
RTAB_ArrPtr		rtarr;					// RTAB		Record refs
PRTAB_ArrPtr	prarr;					// PRTAB	Print statements
XFTAB_ArrPtr	xfarr;					// XFTAB	Copy tables/fields function
SFTAB_ArrPtr	sfarr;					// SFTAB
RATAB_ArrPtr	raarr;					// RATAB	Range records
KXTAB_ArrPtr	kxarr;					// KXTAB	Create Temp table records
CLStrArray		strarr;					//			Store misc strings here 

unsigned short	curptarr  = 0;
unsigned short	curxtarr  = 0;
unsigned short	currtarr  = 0;
unsigned short	curenarr  = 0;
unsigned short	curstrarr = 0;
unsigned short	currdarr  = 0;
unsigned short	curxfarr  = 0;
unsigned short	curprarr  = 0;
unsigned short	cursfarr  = 0;
unsigned short	curraarr  = 0;
unsigned short	curkxarr  = 0;

XTAB	**xtptrs[30];
PTAB	**ptptrs[30];
ENTAB	**enptrs[30];
RDTAB	**rdptrs[30];
XFTAB	**xfptrs[30];
RTAB	**rtptrs[30];
PRTAB	**prptrs[30];
SFTAB	**sfptrs[30];
KXTAB	**kxptrs[30];

//------------termcap vars follow

// these are all control chars:
short	Yarr[] = { 1,2,3,4,5,6,8,9,12,16,18,20,21,22,23,24,26};
char	_tty_bc;
char	*_ya = (char*)&Yarr[0];
char	*_yb = (char*)&Yarr[1];
char	*_yc = (char*)&Yarr[2];
char	*_yd = (char*)&Yarr[3];
char	*_ye = (char*)&Yarr[4];
char	*_yf = (char*)&Yarr[5];
char	*_yh = (char*)&Yarr[6];
char	*_yi = (char*)&Yarr[7];
char	*_yl = (char*)&Yarr[8];
char	*_yp = (char*)&Yarr[9];
char	*_yr = (char*)&Yarr[10];
char	*_yt = (char*)&Yarr[11];
char	*_yu = (char*)&Yarr[12];
char	*_yv = (char*)&Yarr[13];
char	*_yw = (char*)&Yarr[14];
char	*_yx = (char*)&Yarr[15];
char	*_yz = (char*)&Yarr[16];

char	my_null[] = "\0";	// Set following pointers to this blank char array
							// Avoid any uninitialised pointer seg violations.

char	*_vs = my_null;
char	*_vi = my_null;
char	*_ve = my_null;
short	_YT;

char	*_cm = my_null;		// move command
char	*_cd = my_null;
char	*_ce = my_null;
char	*_fs = my_null;
char	*_fe = my_null;
char	*_re = my_null;
char	*_rs = my_null;
char	*_se = my_null;
char	*_so = my_null;
char	*_us = my_null;
char	*_ue = my_null;
char	*_Bs = my_null;
char	*_Be = my_null;
char	*_bo = my_null;
char	*_be = my_null;
char	*__nd;
char	*_te = my_null;
char	*_ti = my_null;

char	*ccbuf;
char	*abuf;

short	_cx;
short	_cy;
short	_sg;
short	_rg;
short	_ug;
short	ox_0;
short	oy_1;
short	align;
short	bypass;

short	_YA;
short	_Yd;
short	_DF;
short	_Y3;
short	_YR;
short	_fg;
short	_Bg;
short	_bg;
short	_Ya = 0x3f;
short	_Ye = 0x3f;
short	_Yh = 0x3f;
short	_Ym = 0x3f;
short	_Yn = 0x3f;
short	_Yo = 0x3f;
short	_Yp = 0x3f;
short	_Yt = 0x3f;
int		_YF;
short	bc = 8;			// backspace char
char	*_bc = (char*)&bc;
char	*_YP = "<CR> to continue";
char	_tc[] = "\0";	
char	*_Yf = _tc;			// fill char

char   TCLArea[1024];          // used by termcap() to store term controls in global mem
//------------------------------

// initilaised variables:
char	alltypes[] = "BCDFGILNPRTX$123456789";	// DC extensions ,'X' == HEX value, 'P' == pointer
short	symbol = -1 ;		// global next symbol found in file   0xFFFF
short	ll = 1;				// last line
short	lla = 1;			// last line

short	Nvars;
short	tnodewasfound;

int		noWarnings;
int		comp_abort;
char	sym[8192];
char	ename[128];
int		syml;
short	while_count;	// used by loadstate()

short	lit_no_0;
short	picol_0;
short	ppcol_1;
short	prow_0;

short	_co = 80;		// lines/columns of terminal
short	_li = 24;

short	comp_errs;
short	no_btabs;
short	lti = 0;
short	block_no_0 = -1;
short	no_itabs;
short	no_files;
bool	got_fdf;		// got first display field in a block
ONESC	*oelist;
bool	esc_char;
//int		inLit;			// unused
int		c;					// returned by nxtch()
int		yflag		= 0;
int		extendDC	= 0;	// Allow DC extensions to CL4. Default NO
int		tpenc		= 0;	// portable enc flag
int		codeb;
int		le_error;
int		count_0;
char	*incl_dir;
FCB		*_files;		//
char	*dbrename;		// used for Database renaming
char	**dbarray;		// ditto
FILE	*dp;			// file ptr for dumpenc
int		isclcomp = 1;	// 
int		notabchks;		// stop CRC checking of Tables? (danger!)

jmp_buf	sjbuf;

PTAB	*PTARR(unsigned short PTABno);	//now a MACRO. see DBdefs.h
void	loadvars(void);
char	*getmem(int MemSize);
void	loaderr(int ErrorNo, char *arg);
bool	alllower(char *a1);
void	alltoupper(char *a1);
short	getsym(void);
int		getnch();
short	getptabp(short a1);
char	*clbasename(char *s);
void	chkoelist(ONESC *a1);
void	gotonl();
int		newbtab();
char	*checkdbrename(char *a1);
char	**camake(char *a1, char Seperator);
short	lehdtxt(void);
short	loadexp(int a1, int a2);
short	getorexp(int a1, int a2);
short	getandexp(int a1, int a2);
short	getaddexp(int a1, int a2);
short	getmultexp(int arg0, int a4);
bool	intexp(short ENTABno);
short	fixbname(char *a1, char *a2);
short	gettf(short ENTno, int *TDno, char *FieldType);
short	gettfexp(short a1);
int		tdopen(int TDno, char *TableName);
int		gettfs(unsigned short ENTABno, int *TTno, char *FLDtype, short *Status);
int		lefresh(PTAB *ptab);
int		lescreen(PTAB *pt);
int		KeyToNumber(char *KeyName);
int		nxtch();
short	dpexp(short ENTno);
short	aliasexp(void);
short	leprint(void);

void	prtree(VARTREE *a1, int Depth);
void	prcolrow(unsigned short a1);
void	prresv(int ReservedMask);
void	prstr(char *a1);
void	prdatefield(unsigned short ENTno, unsigned short XTno, PRTAB *pr);
short	loadsym(char *Symbol);
short	loadresv(char *Symbol);
short	leread(bool isPipe);
int		lerange(short a1);
short	putconst(char *nptr, char FLDtype);
short	getparexp(int a3, int a4);
short	lecreate(void);
int		getxflds(KXTAB *kxtab);
int		getkflds(KXTAB *kxtab);
short	getident(char *FieldName, int *TTno, char *FieldType);
int		numrange(short ENTno, XTAB *xt);
int		isdef(char *StringVal);
void	_define(char *StringVal);
bool	lutree(VARTREE *vartree, char *s1, int StrLen);
void	chkexptype(short ENTno, char *arg, char *ValidTypes, short ErrorNo);
char*	getfuncargtype(unsigned short a1, char **ValidTypes, short *ErrNo, short NumArgs);
int		findtd(char *a1, int a2);
short	leaccept(short ENTno);

// utile routines 
void	DumpPTAB(PTAB* ptb);

void	allptpt(unsigned short PTno, PTAB **a2);
void	dallptpt(PTAB **ptab);
void	moveptptrs(PTAB *a1, PTAB *a2);

void	allxtpt(short a1, XTAB **xtp);
void	dallxtpt(XTAB **xtp);
short	getxtmem(void);
void	movextptrs(XTAB *old, XTAB *New);

void	allenpt(short a1, ENTAB **a2);
void	dallenpt(ENTAB **a1);
short	getenmem(void);
void	moveenptrs(ENTAB *a1, ENTAB *a2);

short	getstrmem(int a1);

short	getrtmem(void);
void	allrtpt(unsigned short a1, RTAB **a2);
void	dallrtpt(RTAB **a1);
void	movertptrs(RTAB *Old, RTAB *New);

short	getxfmem();
void	allxfpt(unsigned short XFno, XFTAB **a2);
void	dallxfpt(XFTAB **xftab);
void	movexfptrs(XFTAB *xft_from, XFTAB *xft_to);

void	allrdpt(unsigned short RDno, RDTAB **a2);
void	dallrdpt(RDTAB **rdtb);
short	getrdmem(void);
void	moverdptrs(RDTAB *a1, RDTAB *a2);

short	getprmem();
void	allprpt(unsigned short a1, PRTAB **a2);
void	dallprpt(PRTAB **a1);
void	moveprptrs(PRTAB *a1, PRTAB *a2);

short	getsfmem();
void	allsfpt(unsigned short a1, SFTAB **a2);
void	dallsfpt(SFTAB **a1);
void	movesfptrs(SFTAB *a1, SFTAB *a2);

short	getramem(void);

short	getkxmem(void);
void	allkxpt(unsigned short a1, KXTAB **a2);
void	dallkxpt(KXTAB **kxtab);
void	movekxptrs(KXTAB *a1, KXTAB *a2);

void	newitab(void);


FLDdesc *getvars(int Varnum);
FLDdesc *getvarmem(void);
VARTREE *gettnode(VARTREE *a1, char *arg, int a3, int a4);

#define col_ANSWER	1
#define col_PROMPT	2
#define col_HEAD    4
XTAB *getcol(XTAB *xt, int col_type);

#define row_ANSWER	1
#define row_PROMPT	2
#define row_HEAD    4
XTAB *getrow(XTAB *xtab, int row_type);

XTAB *getadwid(XTAB *xtab);
XTAB *getprmpt(XTAB *xtab);


/* cl4.h - api for cl4 plugins */

char	*CLgeterror(void);
/*
	returns a string explaining the last error encountered

	if (CLgetfile(ctx, "DEMGL Periods", 0) < 0)
		  printf("%s\n", CLgeterror());
*/

int		CLinit(char *pname);
/*
	Initialises the cl api.
	pname is used when writing to CLELOG.
	returns a cl context required by functions that return a table descriptor.

	ctx = CLinit("test.c");
*/

char	*CLdbschema(int ctx, char *dbname);
/*
	Get a cl database schema (a list of tables).
	returns a str of 'tablename\n......'

	if (!CLdbschema(ctx, "DEMGL"))
		  printf("%s\n", CLgeterror());
*/

char	*CLtdschema(int ctx, int dbno, char *tdname);
/*
	Get a cl database table schema.
	returns a str of 'fname:ftype\n......'

	if (!CLschema(ctx, "DEMGL Periods"))
		  printf("%s\n", CLgeterror());
*/

int		CLgetfile(int ctx, char *str, int verbose);
/*
	Open a cl database table(s).
	If tables = 'all', all tables are opened.
	If no tables specified, no tables are opened.
	returns 0 or < 0 on error.

	if (CLgetfile(ctx, "DEMGL Periods", 0) < 0)
		  printf("%s\n", CLgeterror());
*/

int		CLselect(int ctx, int db, char *str);
/*
	Returns a table descriptor based on the table where expression.
	ctx is the context returned by CLinit().
	On error, returns -1.

	if ((pd = CLselect(ctx, db, "Periods where Period < 100")) < 0)
		printf("%s\n", CLgeterror());

*/

int		CLcreate(int ctx, char *str);
/*
	Create a temporary table, returning a table descriptor.

	if (CLgetfile(ctx, "DEMGL Trans,t", 0) < 0)
		puts(CLgeterror());
	if ((tt = CLcreate(ctx, "tt t.Trans_Date t.Account t.Period t.Unique with t where t.Account = 10")) <= 0)
		printf("%s\n", CLgeterror());
	nflds  = CLnflds(tt);
	while (CLgetnext(tt, 0) > 0) {
		for (fno = 1; fno <= nflds; ++fno) {
			fldname = CLfldname(tt, fno);
			printf("%s ", CLgetfnamefval(tt, fldname, ""));
		}
		printf("\n");
	}

*/

int		CLntables();
/*
	Returns the number of tables currently open.
	The first 3 are reserved so a value of 5 reflects 2 tables open.
*/

int		CLnflds(int td);
/*
	Returns the number of fields in the table (including 'S' and 'K' type fields).
*/

int		CLgetnext(int td, int lock);
/*
	Read the next record in table td.
	Returns the number of records read.

	while (CLgetnext(tt, 0) > 0)
		....
*/

long	CLskipnext(int td, long skip);
/*
	Skip the next skip records in table td.
	Returns the number of records skipped.

	rs = CLskipnext(tt, 100)
		....
*/

int		CLgetprev(int td, int lock);
/*
	Read the previous record in table td.
	Returns the number of records read.

	while (CLgetprev(tt, 0) > 0)
		....
*/

long	CLskipprev(int td, long skip);
/*
	Skip the prev skip records in table td.
	Returns the number of records skipped.

	rs = CLskipprev(tt, 100)
		....
*/

int		CLget(int td, int lock);
/*
	Read the table using the keys loaded into the record area.

	// set field 1 to 8303
	CLsetfnonumval(pd, 1, 8303.0);
	// get the record
	CLget(pd, 0);
	// print the record
	for (nflds = CLnflds(pd), fno = 1; fno <= nflds; ++fno)
		printf("%s ", CLgetfnofval(pd, fno, ""));
	printf("\n");
*/

int		CLput(int td, int lock);
/*
	Write the record currently in the record area.

	while (CLgetnext(pd, 1) > 0) {
		// update a field, yf_fno holds the field no of 'Year Flag'
		CLsetfnonumval(pd, yf_fno, CLgetfnonumval(pd, yf_fno) + 1);
		if (CLput(pd, 0) < 0 )
			printf("%s\n", CLgeterror());
	}
*/

int		CLclear(int td);
/*
	Clear the current record area.

	CLclear(pd);
	CLsetfnonumval(pd, 1, 8304.0);
	CLput(pd, 0);
*/

int		CLcopy(int dtd, int std);
/*
	Copy the std record area into the dtd record area.

	CLcopy(pd1, pd);
	CLget(pd1, 0);
*/

int		CLunlock(int td);
/*
	unlock a table that was previously locked.
*/

int		CLdelete(int td);
/*
	delete the current record.

	if (CLdelete(td) < 0)
		printf("delete failed: %s\n", CLgeterror());
*/

int		CLclose(int td);
/*
	close a table.
*/

int		CLshutdown(int ctx);
/*
	close down the api - opposite of CLinit().
*/
int		CLvalidateexpr(int ctx, int td, char *expr);
/*
	validate a where expr for table td
*/

/**********************************************
**	For examples of the following functions, **
**		see clfields.c in the dem directory  **
***********************************************/

char	*CLgetfnamefval(int td, char *fname, char *fmt);
char	*CLgetfnofval(int td, int fno, char *fmt);
double	CLgetfnamenumval(int td, char *fname);
double	CLgetfnonumval(int td, int fno);
char	*CLgetfnametextval(int td, char *fname);
char	*CLgetfnotextval(int td, int fno);

int		CLsetfnameval(int td, char *fname, void *value);
int		CLsetfnoval(int td, int fno, void *value);
int		CLsetfnamenumval(int td, char *fname, double value);
int		CLsetfnonumval(int td, int fno, double value);
int		CLsetfnametextval(int td, char *fname, char *value);
int		CLsetfnotextval(int td, int fno, char *value);

/*
	The general form of the get and set functions is:

	CL{get|set}{fname|fno}{numval|textval|fval}()

		get - to retrieve a value
		set - to set a value

		fname - pass a (char *)"Field Name" as argument
		fno   - pass a (int)FieldNo as argument

		numval  - a floating point number
		textval - a string value
		fval    - a formatted string value
*/


int		CLtypeistext(char ftype);
/*
	Returns 1 if ftype in "ACDTSK" else returns 0.
*/

int		CLprecision(char ftype);
int		CLfldiskey(int td, int fno);
char	CLfldtype(int td, int fno);
int		CLfno(int td, char *fname);
char	*CLfldname(int td, int fno);
int		CLfldlen(int td, int fno);
char	*CLqfldname(int td, int fno);
int		CLqfldlen(int td, int fno);

char	*CLdatabasename(int td);
int		CLdatabaseno(int td);
char	*CLtablename(int td);
char	*CLaliasname(int td);
int		CLtable(int ctx, char *tname, char *alias, int db);
char	*CLversion(void);
int		CLsutimes(char *str, int startstop);

//	Returns a string with leading and trailing  spaces removed.
char	*trim(char *str);


/*		*** UNDOCUMENTED CL4 lib functions follow
		*** USE AT OWN RISK ***

*/


/* Dump Table statistics - *VERY USEFUL*/
void	dump_tstat(int TTno);

//TDinfo	*_chktd(int TT_no);			// defined in DBdefs.h
DBinfo*	_chkdb(unsigned int DB_no);			
//int		rtdget(void* TT_ptr);		//*NOT* thread safe!
int		keysize(int TT_no);
short	dbstat(char* DBfullpathname, int *UsedPages, int *a2, int *MaxPages);
//char	_tblspace(int TD_no, int *a1);
void*	nfree(void*,void*);
bool	_readpg(DBinfo *DBI_ptr, char *Buffer,int PageNo);
TDef*	tblrow(unsigned int TD_no);
int		_rowsize(TDef* TDef);
int		_rowmin(TDef* TDef);
int		_keysize(TDef* TDefPtr);
int		_keymin(TDef* TDefPtr);
int		recsize(int TD_no);
int		keysize(int TD_no);
bool	cmpstr(char *a1, char *a2);
int		dbsize(int DBno);
int		dbpgsize(int DB_no, short *NumTables, short *MaxTables);
bool	_usage(DBinfo *DBI_ptr, int *UsedPages, int *a3);
int		dbspace(int DB_no);
int		check_bup_sem(DBinfo *DBI_ptr);
//char	_tblspace(int TT_no, int *a2);
void*	_datapg(TDinfo *TDI_ptr, unsigned int PageNo);
char*	_getpg(DBinfo *DBI_ptr); 
PAGE*	_indexpg(TDinfo *TDptr, unsigned int PageNo);
int		_keysize(TDef *FldDesc);
int		_keymin(TDef *FldDesc);
char*	getclver(void);
EXPR*	newexpr(short Type, short a2, char *ExprData, size_t ExprSize);
int		_getpath(int *PageList,TDinfo *TDptr);
void*	ealloc(int size,void* a2);
int		_ulockr(int TDno, char *WorkArea,POS *Pos);
short	cdbrindex(POS *a1, short TDFentry);
void	tuptor(char *dest,POS *Pos, char *src,TDef *TDefptr);
short	rtotup(char *dest, char *src, POS *a3, TDef *a4);
short	_fwidth(char*i, short TDFsize, short TDFtype);
short	_rhead( DBinfo *DBptr, short IndexOffset, int *PageNo);
bool	_scanpg(PAGE *PagePtr, TDinfo *TDptr, short *N2idx, short ArgType);
TDef*	_getshape(DBinfo *DBptr,short TDno);
void	relseq(TDinfo *);
EXPR*	_bound(EXPR* Expr,short Operator,TDef* TDFp);
short	cdbrestrict(int TDno,EXPR *Expr);
NODE_1*	_stepfwd(TDinfo* TDptr);
NODE_1* _stepbak(TDinfo* TDptr);
void*	mmalloc(unsigned int a1);
//void*	mfree_0(void *ptr);
//TDef*	_chkindex(short TDno, DBinfo *DBptr);
void	rectott(int TTno, int a2);
void	tttorec(int TTno, char *WorkArea, int a3);
void	rdkey(int TDno, int a2);
int		tdtotd(int TDno, int TTno, char a3, int a4);
bool	_chkpage(TDinfo *a1, PAGE *PageBuffer);
void	newseq(TDinfo *TDptr);
void	relseq(TDinfo *TDptr);
short	_itosz(PAGE *PageBuffer, short N1_2idx);
char*	_itoptr(PAGE *PageBuffer,short N1_2idx);
void	fix_buf(int TTno, char *a2);
int		typedp(char a1);
short	_picknxt(TDinfo *TDptr, char *WorkArea, short SkipRecs,POS *Pos);
short	_pickprev(TDinfo *TDptr, char *WorkArea, short SkipRecs,POS *Pos);
bool	_update(TDinfo *TDptr, char *WorkArea, POS *Pos, short a4, short *a5);
int		addpage(NODE *Node, short N1_2idx, char *RecBuffer, int RecSize);
int		delpage(NODE *NODEptr, short N1_2idx);
int		rtokey(char *a1, char *Src, POS *a3, TDef *TDefP);
void	_addtuple(char *Buffer, char *a2,TDef *TDefP);
EXPR*	bldexpr(int TTno, signed int a2, int a3);
NODE*	getnode(TDinfo *TDptr, int PageNo, char NodeType);
NODE*	freshnode(TDinfo *TDptr, short NodeType);
TDinfo* _chktd(unsigned int TDno);
char*	_dbname(int DBno);
double	clround(double v13,char FLDtype);
int		getwpos(char *Buffer, short Size);
short	_rdhdr(short fd, DBinfo *DBptr);
void	_addlist(int *PageList, int PageNo);
char*	cdbcpystr(char *DEST, char *SOURCE,...);		// variable args
char*	zap(char *a1, size_t NumBytes);
char*	ftrim(char *a1);
short	getr(int TDno, char *WorkArea, POS *Pos);
DBinfo* _getattrib(int DBno, char *TableName, TD0REC *WorkArea);
int		fill(char *Buffer, int count, char value);
bool	_writepg(DBinfo *DBptr, char *Buffer, unsigned int PageNo);
int		moveleft(NODE *NodeDEST, NODE *NodeSRC, short NumEntries);
int		moveright(NODE *NodeDEST, NODE *NodeSRC, short NumEntries);
short	_promote(char *a1, NODE *Node, NODE *a3, short a4, TDinfo *TDptr);
char*	chkpath(char *a1, char *include, char *Name, int a4, unsigned short perms);
void*	_getfldno(int TTno, int FieldNo, const char *src);
void	cdbfreepg(void);
DBinfo*	_rmdb(int DBno);
//void*	nalloc(size_t BytesRequired, void *);
void*	nalloc(unsigned int BytesRequired, void *);
int		_lockr(DBinfo *DBptr, short IndexOff, char *KeyDefs, unsigned int KeySize, short a5);
void	_mkfree(DBinfo *DBptr, int *PageList);
unsigned int readfull(short fd, char *buf, unsigned int NumBytes);
void	_rmlock(DBinfo *DBptr, int *PageList);
void	_ulock(DBinfo *DBptr, int PID);
void	ulockall(void);
void	_setshut(void);
void*	cdbonexit(void *a1);
int		delr(int TDno, char *WorkArea, POS *Pos);
int		putr(int TDno, char *WorkArea, POS *Pos);
int		lputr(int TDno, char *WorkArea, POS *Pos);
int		newr(int TDno, char *WorkArea, POS *Pos);
int		lnewr(int TDno, char *WorkArea, POS *Pos);
int		lmodr(int TDno, char *WorkArea, short *a3);
int		modr(int TDno, char *WorkArea, short *a3);
int		replace(int TDno, char *WorkArea);
char*	getevar(char*);
char*	getenv(const char*);
Elist*	elist_Create(void);
void	_whead(DBinfo *DBptr, short IndexOff, int PageNo, short Depth);
void	eprint(const char *format, ...);
void	print(const char *format, ...);
void	oprint(FILE *s, const char *format, ...);
void	syserror(const char *format, ...);
void*	mfree_0(void *ptr);
void*	mrealloc(void *ptr, int a2, size_t size);
short*	itoms(short* Targetvar, short a2);
short	_cmpattrib(char *src, short TDFsize, char *Buffer, short TDFsize2, short TDFtype);
short	numsz(char *a1, unsigned int a2);
void	putstr(int fd, char *a2, ...);
void*	fretre(void *ptr, void *a2);	// hacky!
void	relnode(NODE *ptr);
double	_addexp(double, int);
double	mstoxl(int*);
double	mstod(double*);
double _addexp(double a1, int a2);
int*	ltoms(int* a1, int a2);
char	*mstrcpy(char *Src, ...);
void	derror(short ErrorCode, DBinfo *DBptr, TDinfo *TDptr);
void	cdberror(int a1, int DBno, int TTno);
void	dberror(int ErrorCode, int DBno, signed int TDno);
void	setavar(int, char*, int);
void	setvar(int,int);
int		owildmat(char *a1, char *a2, short TDFtype);
void	_relshare(DBinfo *DBptr);
int		xtodom(char *Dest, short FLDlen, unsigned short FLDtype, char *Src);
short	domtox(char *Dest, char *Src, short FLDlen, short FLDtype);
int		_diffattrib(char *a1, char *a2, unsigned int a3, short TDFtype);
char	*getdirname(char *a1);
char	*gelogin(void);
bool	_sparetree(TDinfo *TDptr, int PageNo, short Depth);
void	cllogerr(int ErrorCode, char *ProgName, char *DBASEname, char *TABLEname);
long long *xltoms(long long *a1, double a2);
int		*xitoms(int *a1, int a2);
double	*dtoms(double *a1, double a2);
short	_ifetch(char *Dest, DPOS *Dpos, TDinfo *TDptr);
short	_dfetch(char *Dest, DPOS *Dpos, TDinfo *TDptr);
EXPR*	newjunc(EXPR *a1,short Operator,EXPR *a3);
double	gettime(const char *src);		//*** Not finished ***
void	negate(unsigned char *a1, short TDFsize);
void	newdtab(void);
void	newttab(void);
void*	frelst(void *ptr, void *a2);
int		findfno(int TDno, char *FieldName);
int		_lockmap(DBinfo *DBptr, short LockMode);
int		*__errno_location(void);
int		_beyond(EXPR *SelectExpr, short Operator, char *a3,TDef *TDefptr);
void	_imerge(RLARR *RAl, TDinfo *TDptr);
void	_dmerge(RLARR *a1, TDinfo *TDptr);
bool	putnode(TDinfo *TDptr, NODE *Node);
int		_split(TDinfo *TDptr, NODE *Node, NODE *Node2, short N1_2idx2, char **a5);
bool	_spread(TDinfo *TDptr, NODE *a2, NODE *a3,short N1_2idx, char **a5);
int		_redistrib(NODE *a1, NODE *a2, short PageSize);
short	_oshare(char *a1,char *a2);
void	modpage(NODE *Node, short N1_2idx, char *RecBuffer, short NewRecSize);
int		_mktable(int *TTnos, short NumTTs, short *KeyIDs, short *DataIDs, short *a5);
int		mktable(int *TTnos, short NumTTs, short *KeyIDs, short *DataIDs, short *a5);
int		_lock(DBinfo *DBptr, short IndexOff, short LockMode);
bool	_match(char *src, EXPR *Expr, TDef *TDefPtr);

void	termcap();
short	gettcent(char *a1);
void	gettccl(char **a2);
void	getclatt(char *AttrName, char **a2);
void	tcerror(char *format, char *a2);
void	qosat(int a1);
void	qatt(char a1, int a2);
void	qat(int X, int Y);
char	*sysdate(char*);
char	*fillbuf(char *Buff,size_t NumBytes, char value);
unsigned short acrc(unsigned short CRC, char *buff, unsigned int len);
unsigned short pcrc(unsigned short CRC, TDef *tdef, int TDFsize, short TDFtype, unsigned int a5);

short	leselect(void);
bool	leulck(int *a1);
bool	lemsg(PTAB *pt);
bool	lepause(PTAB *pt);
int		lefresh(PTAB *ptab);
int		lescreen(PTAB *pt);
int		lerange(short PTno);
short	leopen(bool isPipe);
int		lejoin(short PTno, int join);
int		lerfile(int *a1, short Symbol);
short	lecopy(void);
bool	leif(int *a1);
bool	lewhile(int *a1);
int		leredisp();
int		leeop(int a1);
bool	leformat(int *a3, bool no_brk);
int		ledefine(bool local);
void	leblock(int *LineNo);
void	prtfid(unsigned short ENTABno);
int		closedb(int a1);
void	tdlocks();
void	prvars(bool ShowTree);
void	prtds();
void	prrt(unsigned short RTno);
void	prpf(unsigned short PRno);
void	prrd(unsigned short RDno);
void	prxt(XTAB *xtab, bool isHDTXT);
void	prkx(KXTAB *kxtab);
void	prcalc(ENTAB *ent);
void	prpipe(SFTAB *sftab);
void	prset(PTAB *ptb);
void	ProgDump();
short	getrelexp(int inCondExp, int chkINT);
bool	lesrand(PTAB *ptab);
bool	lesleep(PTAB *ptab);
int		cpybuf(void *DEST, void *SOURCE, int Size);
void	mexit(int ErrorCode);
bool	cmpbuf(char *a1, char *a2, int a3);
int		cldbopen(char *DBname, ...);
char	gettype(unsigned short TDFtype, short TDFsize);
unsigned short clgetmode(char *path);
TDef*	_chkindex(short TableIdx, DBinfo *DBptr);
short	_cmpkey(char *a1, char *a2, TDef *tdef);
int		emark(DBinfo *DBptr, short IndexOff);
int		ewake(DBinfo *DBptr, short IndexOff);
void	ewait(DBinfo *DBptr, short IndexOff);
bool	_balance(TDinfo *TDptr, NODE *Node, int *PageList, short Depth);
void*	alloc(unsigned int size, void* vector);
int		cdbfclear(char *Buff, short TDFsize, short TDFtype);
int		_locktbl(DBinfo *DBptr, short IndexOff, int Lock);
void	_ipos(DPOS *dpos, int PageNo, short Depth, TDinfo *TDptr);
short	scnbuf(char *a1, short TDFsize, char Value);
void	_chkitems(NODE *NodeP, short RequiredEntries);
char	*extstr(char *src, int Seperator, char **a3);
FLDdesc	*getvarname(char *VariableName);
short	retrieve(int TDno, char *WorkArea1, char *WorkArea2);
bool	leincl(void);
short	lehdtxt(void);
bool	leopenlib(int *LineNo);
void	prentab();
bool	loadstate(int *LineNo);
bool	lemaint(BTAB *btb);


#endif
