#ifndef DBDEFS
#define DBDEFS

#include <pthread.h>	// for pthread specific stuff below
#include <time.h>       // for struct tm
#include <sys/times.h>  // for times()
#include "structures.h"		// common file with structure definitions

//*************************************************************
// comment out to use our own local versions of these variables
//#define USE_LIB_CL4
//
//*************************************************************
// uncomment to send debug output to stdout
//#define DEBUG_CL4 
//*************************************************************
// Quick MACROs to swap bytes around

#define SWAP_SHORT(x)(*((unsigned char*)(x)+1) | (unsigned short)(*x << 8))
#define MSHORT(x) ((unsigned short)(x << 8) | (unsigned short)((x >> 8)& 0xFF))
#define M_INT(x) ((unsigned int)(x << 24) | (unsigned int)((x >> 8) & 0x0000FF00 )| (unsigned int)((x << 8)& 0x00FF0000)| (unsigned int)((x >> 24)& 0xFF))

// macros for accessing CL4 tables easily
#define PTARR(x) (x ? &ptarr.TableAddr[x - 1] : 0)	// is a *function* in REAL clenter
#define ENARR(x) (x ? &enarr.TableAddr[x - 1] : 0)
#define XTARR(x) (x ? &xtarr.TableAddr[x - 1] : 0)
#define PRARR(x) (x ? &prarr.TableAddr[x - 1] : 0)
#define KXARR(x) (x ? &kxarr.TableAddr[x - 1] : 0)
#define XFARR(x) (x ? &xfarr.TableAddr[x - 1] : 0)
#define SFARR(x) (x ? &sfarr.TableAddr[x - 1] : 0)	// SFTAB file/pipe operations
#define RDARR(x) (x ? &rdarr.TableAddr[x - 1] : 0)	// RDTAB re-display
#define RTARR(x) (x ? &rtarr.TableAddr[x - 1] : 0)	// range specifier
#define STRARR(x) (x ? &strarr.StringTable[x - 1] : 0)	// String table



typedef struct 
{
	short	cmd_flags;		// command flag bits
	short	field_2;
	int		child_PID;		// child PID for pipes
	int		field_8;
	int		field_C;
	FILE*	RD_FILE;
	FILE*	WR_FILE;
	char*	ALIAS;
}FCB;

/*
typedef struct 
{
	short	RDTFlags;
	short	field_2;
	int		TTno;
	short	NextRD;
	//short	field_A;
}RDTAB;
*/


// forward reference for Elist_Elem* members:
typedef struct Elist_Elem Elist_Elem;
struct Elist_Elem
{
	int			field_0;
	void		*UserData;
	Elist_Elem	*Prev;
	Elist_Elem	*Next;
};

// forward reference for Elist* members:
typedef struct Elist Elist;
struct Elist
{
	int			Size;		//x00	0
	Elist_Elem	*Current;	//x04	4
	Elist_Elem	*Prev;		//x08	8
	Elist_Elem	*Next;		//x0C	12
};	// sizeof(Elist) = 16

/*typedef struct 
{
	int			lx_KeyType;	// x00	0
	int			lx_CRstart;	// x04	4
	int			lx_CRend;	// x08	8
	Elist*		lx_elist;	// x0C	12
	int			lx_keyno;	// x10	16
}LEX_elem ;	// (sizeof=0x14) 20
*/

typedef struct 
{
	Elist	*field_0;
	Elist	*field_4;
	Elist	*field_8;
	char	*FileName;
	int		(*RdFunc)(void);
}KEY_Elist;

typedef struct 
{
	int			lx_KeyType;	// x00	0
	int			lx_CRstart;	// x04	4
	int			lx_CRend;	// x08	8
	Elist*		lx_elist;	// x0C	12
	int			lx_keyno;	// x10	16
}KEY_Elem;


typedef union
{
	int		(*Func)(KEY_Elist*);		// *** Need to check ***
	char	*Name;
	int		Count;
	Elist*	List;
} KEYPTR_1;

typedef struct 
{
	char*	KeyFnName;
	int		(*Key_Fn)(KEY_Elist*);		// *** Need to check ***
	char*	KeyFnDesc;
}KEYFUNC;

typedef struct 
{
	KEYPTR_1	u_Ptr;
	char		Type;
	//char		pad[3];
} KEYPTR;


// Used by cgi functions
typedef struct form_entry form_entry; // forward ref
struct form_entry
{
  char *name;
  char *val;
  form_entry *next;
};

typedef struct 
{
  char *name;
  char *val;
} CGI_VAR;


int		no_dtabs;
int		no_ttabs;
TDesc	*ttab;						// pointer to array of TDesc structures. Doesn't look right, but works...
DBase	*dtab;						// pointer to array of DBase structures
BTAB	*btab;						// used by clcomp

short	_nspare = 20;				// default number of spare pages to "capture"
short	_psize = 0x1000;			// default page size for temp files: 0x1000
int		_fsize = 0x7A120;			// default file size for temp files: 0x7A120  [ 500000 ]

POS		_tpos[] = {107, 0, 0, 106, 0, 22, 108, 0, 28, 109, 0, 24, 110, 0, 26, 0, 0, 0};	// reposition data for "tables" : TD0
POS		_fpos[] = {106, 0, 0, 101, 0, 2, 102, 0, 4, 103, 0, 6, 104, 0, 8, 0, 0, 0};	// reposition data for "fields" : TD1

char			*_pname;				// points to "cldblib" [syserror()]
int				time_stamp;			// used by syserror()
pthread_mutex_t	mutex;
pthread_key_t	ctxKey;				// used by CLgetError()
pthread_t		threads[256];
char			*_pnameArr[256];	// used by CLinit(), CLdeinit()
char			*pnameArr[256];		// used by CLinit(), CLdeinit()
int				contexts[256];		// Array of Context values. Context = PID + 10000

//int			tx_started[256];
int				tx_started;			// single int in clenter

int				cl_tx;				// transaction count?					
__pid_t			lpid;				

//int				lfd[256];			// Array of Log file descriptors. Indexed by Context
int				lfd	 = -1;				// single int in clenter

int		cldblibInit = 0;			// set to "1" when CLinit() finished
int		exact		= 0;			// Exact match on field names	default = 0
int		isClDemo	= 0;			// Is this a demo version of libcl4? 1 = yes
short	trim_it		= 0;			// Used to temporarily override trim on/off value

// used by cllogerr() and clcgi processing
int			isCGI;
int			isSoap;
form_entry	*ck;		// cookies linked list
form_entry	*fe;		// form entry variables linked list
form_entry	*pck_1;
form_entry	*pfe_4;
form_entry	*pfe_7;
form_entry	*pfe_10;
char		*nameval_2;
char		*nameval_5;
char		*svName_6;
char		*svName_9;
int			cleanSSI;
int			started_0;
int			started_3;
int			lenSeen_8;
char		scriptName[100];

CGI_VAR	ce[] = { 
	"CONTENT_LENGTH",0,
	"CONTENT_TYPE",0,
	"REQUEST_METHOD",0,
	"QUERY_STRING",0,
	"SERVER_NAME",0,
	"RPC_TYPE",0,
	"AUTH_TYPE",0,
	"DOCUMENT_ROOT",0,
	"GATEWAY_INTERFACE",0,
	"HTTP_ACCEPT",0,
	"HTTP_ACCEPT_LANGUAGE",0,
	"HTTP_CONNECTION",0,
	"HTTP_COOKIE",0,
	"HTTP_HOST",0,
	"HTTP_REFERER",0,
	"HTTP_USER_AGENT",0,
	"HTTP_UA_CPU",0,
	"HTTP_UA_DISP",0,
	"HTTP_UA_OS",0,
	"PATH_INFO",0,
	"PATH_TRANSLATED",0,
	"REMOTE_ADDR",0,
	"REMOTE_HOST",0,
	"REMOTE_IDENT",0,
	"REMOTE_USER",0,
	"SCRIPT_FILENAME",0,
	"SCRIPT_NAME",0,
	"SERVER_ADMIN",0,
	"SERVER_PORT",0,
	"SERVER_PROTOCOL",0,
	"SERVER_SOFTWARE",0,
	"PATH",0,
	"TERM",0,
	"TZ",0,
	"CLSETUP",0,
	0,0					// NULL terminated list
};

double	_dbias[] = {1 , 100, 10000, 1e+06, 1e+08, 1e+10, 1e+12};	// used by xtodom()

// Max_nos data in memory:
/*int	MAX_nos[]	= {
0x0,0x0,0x0A2000000,0x426D1A94,0x0A2000000,0x426D1A94,0x20000000,0x4202A05F,
0x20000000,0x4202A05F,0x0,0x4197D784,0x0,0x4197D784,0x0,0x412E8480,0x0,
0x412E8480,0x0,0x40C38800};
*/

//called by echecknum.
// translates to:
double MAX_nos[] = {0.000000,
					1000000000000.000000,
					1000000000000.000000,
					10000000000.000000,
					10000000000.000000,
					100000000.000000,
					100000000.000000,
					1000000.000000,
					1000000.000000,
					10000.000000};

// Remap td_tables (_tshape) to TD0REC structure. (fields re-ordered only) 
POS My_pos[] = {107, 0 , 0,
				106, 0, 22,
				108, 0, 28,
				109, 0, 24,
				110, 0, 26,
				0  , 0,  0};

// Map td_fields shape to "FIELDS" structure.
// Different size on x86 vs. x64
POS My_pos_0[] = {102, 0, offsetof(struct FIELD, TDFentry),
				  104, 0, offsetof(struct FIELD, TDFsize),
				  103, 0, offsetof(struct FIELD, TDFtype),
				  0, 0,	0};


TDef	_tshape[] =  { 107, 20, 0x0B, 108,   2, 0x200, 109, 2, 0x40, 110, 2,  0x40, 106, 1, 0x40, 0, 0, 0};			// tables
TDef	_fshape[] =  { 106,  1, 0x41, 101,   1,  0x41, 102, 2, 0x40, 103, 2, 0x200, 104, 2, 0x40, 0, 0, 0 };			// fields
TDef	_lshape[] =  { 106,  1, 0x41, 116, 128,     1, 115, 4, 0x40, 0, 0, 0};											// locks 
TDef	_xfshape[] = { 106,  1, 0x41, 101,   1,  0x41, 102, 1, 0x40, 103, 2, 0x200, 104, 2, 0x40, 105, 1, 8, 0,0,0};

TDef	*def_0[] = {_tshape,_fshape,_lshape,_xfshape};

//int		sz_shape[]	= {36,36,24,42};
int		sz_shape[]	= {sizeof(_tshape), sizeof(_fshape), sizeof(_fshape), sizeof(_xfshape)};

char	tname[]="tables";
char	fname[]="fields";
char	lname[]="locks";
char	*sname_0[] = {tname,fname,lname};

char	tkey[20];	// 'tables' key area
char	fkey[2];	// 'fields' "	"
char	lkey[129];	// 'locks'	"	"

short	lmap[] = {0,1,2,0};	// used by _lock()

TDinfo	tables_def = {0x0CD,0,0,0x100,0x212,_tshape,0, 27,  8, 20,  1,0,0,0,0,1,0,0,1,tkey,0};
TDinfo	fields_def = {0x00D,0,1,0x100,0x208,_fshape,0,  8,  8,  2,  2,0,0,0,0,1,0,0,1,fkey,0};
TDinfo	locks_def  = {0x029,0,2,0x100,0x200,_lshape,0,133,133,129,129,0,0,0,0,1,0,0,1,lkey,0};

TDinfo	*def_1[] = {&tables_def,&fields_def,&locks_def};

TDinfo	*_tbl[3005]	= {&tables_def,&fields_def,&locks_def};

#define DB_MAX 255			// max size of open databases array
DBinfo	*_dbase[DB_MAX];

char	str_0[8192];		// used by getevar() and others?

char	buf_0[3072];		// used by CLdbschema and others?
char	buf_2[6144];		// used by CLtdschema and others?
char	buffer_0[120];		// used by _getfldno()
PAGE*	pbuf = 0;			// PageBuffer pointer used by getpg(), cdbfreepg()
int		psize = 0;			// current page size in bytes
char	TCLArea[1024];		// used by termcap() to store term controls in global mem

int		fcount;				// Index fetch count : 

TRANSLOG	tl;				// used by cldel/put functions. Transaction Log structure
//bool	logging = true;		// ditto, default value = logging is *ON*
bool	logging = false;

bool	have_forked_0;		// Used by pfork()

int		before_logging;		// ditto, used to store int value of evar: "CLTBEFORELOG"
char	l_tty[20];			// Logging TTY id
char*	nlDbases_1;			//
int		tdArr_0[3005];		//

char	cdbtmpdir[100];		// used by _mktmp()
char	defcdbtdir[] = "/tmp";

FILE*	fp_1;				// used by eprint()

// see dates.c
char	*_YD = "1990-2050:10";		// Default width = 10: dd-mm-yyyy	old: "1955-2022:10"
//char	*_YL = "\n\n\0";
char	*_YL = "\0";
//int		_DF = 0; 					// Date Format 0,1, or 2   [0 = dd-mm-yy]  
int		defDateWidth = 10;			// Default date width = 10 (0x0A)
char	THOU_SEP[10] = ",";			// used by loadMonthArrays
char	RADIX_CHAR[10] = ".";		// used by loadMonthArrays
int		dateDone = 0;				// flag to indicate default date settings have been processed
int		fromYear = 1990;
int		toYear = 2050;	
char	**wday;
char	**fmth;
char	**mth;
int		darr[10];					// array of 10 ints, used by dfmt()

//                      J  F  M  A  M  J  J  A  S  O  N  D
short	day_tab[] = {0,31,28,31,30,31,30,31,31,30,31,30,31,
	                 0,31,29,31,30,31,30,31,31,30,31,30,31};

// mstrings.c

char*	mbuf;				// used by mmalloc_init(), mmalloc()

pthread_once_t	once_block;

DOMAINR	domsfp[] = {0x7533, 0,  0, 0,			// domains table defs
					0x7535, 0,  2, 0,
					0x753D, 0, 22, 0,
					0x7537, 0, 24, 0,
					0x753B, 0, 54, 0,
					0x753C, 0, 74, 0,
					   0, 0,  0, 0};

DOMAINR	domsfp_0[] = {0x7533, 0,  0, 0,			// elements table defs
					0x7535, 0,  2, 0,
					0x753D, 0, 22, 0,
					0x7537, 0, 24, 0,
					0x753B, 0, 54, 0,
					0x753C, 0, 74, 0,
					   0, 0,  0, 0};

TDef domsflds_0[] = {0x7533,  2, 0x41,			// elements field defs
					 0x7535, 20, 0x0A,
					 0x753D,  2, 0x40,
					 0x7537, 30, 0x0A,
					 0x753B, 20, 0x0A,
					 0x753C,  1, 0x0A,
						0,  0, 0};

/*
This is the defs for the fixed "elements" table: 
table < elements >
 No. Type  Description
[ 1]  *I   30003			TDFentry
[ 2]   A   30005 [20]		Element Name
[ 3]   I   30013			Default Width
[ 4]   A   30007 [30]		Heading
[ 5]   A   30011 [20]		Range
[ 6]   A   30012 [1]		Required? (Y/N)

Example:
-> l elements w 2 = rec_no
[ 1]  *I  30003                : 29963
[ 2]   A  30005                : Rec_No
[ 3]   I  30013                : 10
[ 4]   A  30007                : Rec No
[ 5]   A  30011                :
[ 6]   A  30012                : n

*/

#endif

/*

Limits with CL4:

Database limits 2GB for a database
The page limit in a database is determined by the size of the pages.
For example, a limit of 1,000,000 pages at 2k each, or 500,000 pages at 4k each.
256 tables in a database
508 fields in a table
65535 field identifiers in a database
Records in a table limited only by available space in the database
508 bytes per record
64 fields in a combined key
64 bytes total combined key length
6 levels in an index tree
6 tables can be joined at one time

Version 4.2.21 onwards:
By default, databases are now created with 500,000 4k pages and 255 tables.
Databases can now have up to 8M 8k pages (64GB).

---

Hidden table names in each database:
First three are at fixed Table index positions, and fixed TD numbers.
They also have fixed Shape tables etc.

COURDB1old  tables		: TD0
COURDB1old  elements	: TD1
COURDB1old  locks		: TD2
COURDB1old  domain
COURDB1old  _Clfields
COURDB1old  _Cltables

Definitions:

-> l tables
[ 1]  *A  107                  : _Clfields		- Table name
[ 2]   I  108                  : 192
[ 3]   I  109                  : 2048
[ 4]   I  110                  : 4140
[ 5]   B  106                  : 5

-----------

# elements can be part of records:

-> l elements
[ 1]  *I  30003                : 29015
[ 2]   A  30005                : Direction
[ 3]   I  30013                : 1
[ 4]   A  30007                :
[ 5]   A  30011                :
[ 6]   A  30012                : n

[ 1]  *I  30003                : 29999
[ 2]   A  30005                : Account		- Field Name
[ 3]   I  30013                : 6				- Field Size in Bytes
[ 4]   A  30007                : Ac Num
[ 5]   A  30011                :
[ 6]   A  30012                : n

-> d elements
 No. Type  Description
[ 1]  *I   30003
[ 2]   A   30005 [20]
[ 3]   I   30013
[ 4]   A   30007 [30]
[ 5]   A   30011 [20]
[ 6]   A   30012 [1]
key = [ 2] record = [75]

--------------------

-> l domain
[ 1]  *A  107                  : AAE_adjust		- Field name
[ 2]   I  102                  : 29051			- Pointer to entry in elements table
[ 3]   I  104                  : 1				- Size in Bytes?
[ 4]   C  103                  : 032			- ???

[ 1]  *A  107                  : Direction
[ 2]   I  102                  : 29015
[ 3]   I  104                  : 1
[ 4]   C  103                  : 032

[ 1]  *A  107                  : Account
[ 2]   I  102                  : 29999			- Pointer to entry in elements table
[ 3]   I  104                  : 6				- Field Size in bytes
[ 4]   C  103                  : 032

-> d domain
 No. Type  Description
[ 1]  *A   107 [20]
[ 2]   I   102
[ 3]   I   104
[ 4]   C   103 [6]
key = [20] record = [30]

--------------

-> l _clfields
[ 1]  *C  30016                : Box_Rates		- Table Name
[ 2]  *I  30006                : 10				- Field No. (as in DBDEF)
[ 3]   C  30005                : Account		- Field Name in Table
[ 4]   C  30008                : y				- Key Field?

-> d _clfields
 No. Type  Description
[ 1]  *C   30016 [20]
[ 2]  *I   30006
[ 3]   C   30005 [20]
[ 4]   C   30008 [1]
key = [22] record = [43]

--------------

-> l _cltables
[ 1]  *C  30016			: Box_Rates				- Field Name
[ 2]   C  30014			: box					- Alias
[ 3]   C  30015			: Box rates by client	- Description

-> d _cltables
 No. Type  Description
[ 1]  *C   30016 [20]
[ 2]   C   30014 [5]
[ 3]   C   30015 [30]
key = [20] record = [55]

*/

