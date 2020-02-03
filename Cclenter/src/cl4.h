#ifndef CL4HDR
#define CL4HDR

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
short	_cmpkey(char *a1, char *a2, TDef *a3);
bool	cmpstr(char *a1, char *a2);
int		dbsize(int DBno);
int		dbpgsize(int DB_no, short *NumTables, short *MaxTables);
bool	_usage(DBinfo *DBI_ptr, int *UsedPages, int *a3);
int		dbspace(int DB_no);
int		check_bup_sem(DBinfo *DBI_ptr);
PAGE*	_datapg(TDinfo *TDI_ptr, unsigned int PageNo);
PAGE*	_getpg(DBinfo *DBI_ptr); 
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
void*	mmalloc(size_t reqsize);
//void*	mfree_0(void *ptr);
//TDef*	_chkindex(short TDno, DBinfo *DBptr);
void	rdkey(int TDno, int a2);
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
char*	chkpath(char *a1, char *a2, char *Name, int a4, unsigned short perms);
void*	_getfldno(int TTno, int FieldNo, const char *src);
void	cdbfreepg(void);
DBinfo*	_rmdb(int DBno);
void*	nalloc(size_t BytesRequired, void *);
//void*	nalloc(unsigned int BytesRequired, void *);
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
void	cgiprint(int ErrorCode);
void*	mfree_0(void *ptr);
void*	mrealloc(void *ptr, int a2, size_t size);
short*	itoms(short* Targetvar, short a2);
short	_cmpattrib(char *src, short TDFsize, char *Buffer, short TDFsize2, short TDFtype);
short	numsz(char *a1, unsigned int a2);
void	putstr(int fd, char *a2, ...);
EXPR*	fretre(EXPR *expr, EXPR *a2);
void	relnode(NODE *ptr);
double	_addexp(double, int);
int		mstol(int *a1);
double	mstoxl(int*);
int		mstoxi(int *a1);
double	mstod(double*);
double _addexp(double a1, int a2);
int*	ltoms(int* a1, int a2);
char	*mstrcpy(char *Src, ...);
void	derror(short ErrorCode, DBinfo *DBptr, TDinfo *TDptr);
void	cdberror(int a1, int DBno, int TTno);
void	dberror(int ErrorCode, int DBno, signed int TDno);
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



#endif
