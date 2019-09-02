#ifndef LOCALVARN_C
#define LOCALVARN_C

// This file describes :
// - local variable NAME <=> Number conversions
// - flag bits used in TDef's
// - flag bits used in FLDdesc's

// 62 reserved vars: 0 - 61

// Reserved variable names to var#

#define v_NULL			0
#define v_ERROR			1
#define v_LOCKED		2
#define v_MODE			3
#define v_ZERO			4
#define v_REPLY			5
#define v_EXIT			6
#define v_LREPLY		7
#define v_V1			8
#define v_V2			9
#define v_V3			10
#define v_V4			11
#define v_V5			12
#define v_V6			13
#define v_V7			14
#define v_V8			15
#define v_V9			16
#define v_INTERRUPT		17
#define v_MREPLY		18
#define v_LCOUNT		19
#define v_PCOUNT		20
#define v_PLINES		21
#define v_SLINES		22
#define v_SCOLS			23
#define v_MATCH			24
#define v_LMATCH		25
#define v_OUTPUT		26
#define v_GOBACK		27
#define v_ABORT			28
#define v_AAUTO			29
#define v_RAUTO			30
#define v_FILL			31
#define v_ALIGN			32
#define v_BIGE			33
#define v_FNUM			34
#define v_NEXIT			35
#define v_SKIP			36
#define v_TRIM			37
#define v_DEBUG			38
#define v_TAB			39
#define v_JUSTIFY		40
#define v_GOING_BACK	41
#define v_BYPASS		42
#define v_REPEAT		43
#define v_HANGUP		44
#define v_CONVERT		45
#define v_RAWDISPLAY	46
#define v_RAWPRINT		47
#define v_SCONVERR		48
#define v_LSCONV		49
#define v_MLINE			50
#define v_MCOL			51
#define v_XML_DEPTH		52
#define v_DATEFORM		53
#define v_MONTHNAMES	54
#define v_ABMONTHNAMES	55
#define v_DAYNAMES		56
#define v_ABDAYNAMES	57
#define v_PAUSE_PROMPT	58
#define v_XML			59
#define v_DOS			60
#define v_CSV			61

//================================================

// Screen Attribute FieldTypes

#define ft_ACCEPT	0x01u	// ACCEPT fields
#define ft_ERROR	0x02u	// ERROR display fields
#define ft_HEAD		0x04u	// HEAD statements
#define ft_MESSAGE	0x08u	// cl messages and MESSAGE statement
#define ft_PROMPT	0x10u	// PROMPTS on FIELD statements
#define ft_TEXT		0x20u	// TEXT statements
#define ft_PRINT	0x40u	// PRINT statements

//================================================

// Function bit patterns

#define FN(N) ((N << 10))	//  0001-003F ->  0400-FC00

#define f_INTRES    0x0200u	// Integer result flag bit
#define f_NOT		FN(1)	// 0400
#define f_SQRT		FN(2)	// 0800
#define f_LOG		FN(3)	// 0C00
#define f_SIN		FN(4)	// 1000
#define f_COS		FN(5)	// 1400
#define f_TAN		FN(6)	// 1800
#define f_UCASE		FN(7)	// 1C00
#define f_LCASE		FN(8)	// 2000
#define f_SLEN		FN(9)	// 2400
#define f_DAY		FN(10)	// 2800
#define f_GETARG	FN(11)	// 2C00
#define f_ASIZE		FN(12)	// 3000
#define f_ASC		FN(13)	// 3400
#define f_CHR		FN(14)	// 3800
#define f_SRAND		FN(15)	// 3C00
#define f_GETENV	FN(16)	// 4000
#define f_LOG10		FN(17)	// 4400
#define f_ASIN		FN(18)	// 4800
#define f_ACOS		FN(19)	// 4C00
#define f_ATAN		FN(20)	// 5000
#define f_ADD		FN(21)	// 5400		+
#define f_SUB		FN(22)	// 5800		-	multiple meanings for strings
#define f_POW		FN(23)	// 5C00		** power()
#define f_MUL		FN(24)	// 6000		*
#define f_DIV		FN(25)	// 6400		/
#define f_MOD		FN(26)	// 6800		%
#define f_MATCH		FN(27)	// 6C00
				//  FN(28)  // 7000 free?
#define f_SDEL		FN(29)	// 7400
#define f_SMULT		FN(30)	// 7800
#define f_SWORD		FN(31)	// 7C00
#define f_SETENV	FN(32)	// 8000
#define f_SKEY		FN(33)	// 8400
#define f_SFMT		FN(34)	// 8800
#define f_GETXML	FN(35)	// 8C00
#define f_DEGREES	FN(36)	// 9000
#define f_RADIANS	FN(37)	// 9400
#define f_EXP		FN(38)	// 9800
#define f_TRIM		FN(39)	// 9C00
#define f_GETCGI	FN(40)	// A000
#define f_NAME		FN(41)	// A400
#define f_ASSIGN	FN(42)	// A800		'='	Assignment operator 
#define f_ARRSTART	FN(43)	// AC00		'['	Array Subscript
				//  FN(44)  // B000 free?
				//  FN(45)  // B400 free?
#define f_RANDOM	FN(46)	// B800
#define f_KEYREADY	FN(47)	// BC00
				//  FN(48)  // C000 free?
				//  FN(49)  // C400 free?
				//  FN(50)  // C800 free?
#define f_SUBSTR	FN(51)	// CC00
#define f_SREP		FN(52)	// D000
#define f_SCONV		FN(53)	// D400
#define f_ADDEQ		FN(54)	// D800		+=
#define f_SUBEQ		FN(55)	// DC00		-=		multiple meanings for strings
#define f_MULEQ		FN(56)	// E000		*=
#define f_DIVEQ		FN(57)	// E400		/=
#define f_MODEQ		FN(58)	// E800		%=
#define f_POWEQ		FN(59)	// EC00		**=		power()
				//  FN(60)  // F000 free?
#define f_SCAT		FN(61)	// F400
				//  FN(62)  // F800 free?
				//  FN(63)  // FC00 free?

//================================================

// kxtab->KXFlags

//#define kxf_???		0x0001u
//#define kxf_???		0x0002u
//#define kxf_???		0x0004u
//#define kxf_???		0x0008u
//#define kxf_???		0x0010u
//#define kxf_???		0x0020u
//#define kxf_???		0x0040u
//#define kxf_???		0x0080u
//#define kxf_???		0x0100u
//#define kxf_???		0x0200u
#define kxf_UNIQUE		0x0400u		// create unique type table
//#define kxf_???		0x0800u		// set but never read back ??
#define kxf_EMPTY		0x1000u		// create unique *empty* table
//#define kxf_???		0x2000u
//#define kxf_???		0x4000u
//#define kxf_???		0x8000u


// TDptr->TDFlags flag bits

#define tdp_LOCK		0x0001u
//#define tdp_???		0x0002u		// indicates that TDptr->TDSallocBuf points at a RLARR??
//#define tdp_???		0x0004u
//#define tdp_???		0x0008u		// ???? appears in _append()
//#define tdp_???		0x0010u
//#define tdp_???		0x0020u
#define tdp_RecVar		0x0040u		// Record has string entries -> Variable record size (RecSize != RecMin)
#define tdp_KeyVar		0x0080u		// Key has string entries -> Variable Key size (KeyMin != KeySize)
//#define tdp_???		0x0100u
//#define tdp_???		0x0200u
#define tdp_REORDER		0x0400u		// reorder() == duplicates allowed, on-time pass
#define tdp_UNIQUE		0x0800u		// compose() == create unique
#define tdp_RLARR		0x1000u		// indicates TDptr->TDSallocBuf holds an RLARR structure
//#define tdp_???		0x2000u
//#define tdp_???		0x4000u
//#define tdp_???		0x8000u


// TDef->TDFtype flag bits

#define tdf_KEY			0x0001
#define tdf_STRING		0x0002
#define tdf_SIGNED		0x0004		// Allow signed numbers
#define tdf_NOCASE		0x0008		// Allow case-insensitive match
//#define tdf_???		0x0010
//#define tdf_???		0x0020
//#define tdf_???		0x0040
//#define tdf_???		0x0080
//#define tdf_???		0x0100
//#define tdf_???		0x0200
//#define tdf_???		0x0400
//#define tdf_???		0x0800
//#define tdf_???		0x1000
//#define tdf_???		0x2000
//#define tdf_???		0x4000		// ???? reverse comparisons. descend sort??
//#define tdf_???		0x8000


// TTptr->TDlocked defs
#define ttl_LOCK		0x0001u		// lock is called on this TT
#define ttl_PUT			0x0002u		// put is called on this TT
#define ttl_GET			0x0004u		// get is called on this TT
#define ttl_GETK		0x0008u		// indicate to CLENTER that key tables need to be opened
#define ttl_HASK		0x0010u		// Has Key Tables
#define ttl_AVAIL		0x0020u
#define ttl_FLOCK		0x0040u		// File locking flag: getfile XYZ lock
#define ttl_LOCKR		0x0080u		// lockr is called on this TT
#define ttl_GETNP		0x0100u		// Getnext/GetPrev
#define ttl_CREATE		0x0200u
#define ttl_UNIQUE		0x0400u
#define ttl_OPEN		0x0800u		// Indicates table has been opened with a getfile statement
//#define ttl_???		0x1000u
//#define ttl_???		0x2000u
#define ttl_HASSUB		0x4000u		// Has Sub-records
#define ttl_DEL			0x8000u		// delete is called on this TT


// FLDdesc FLDstat flag bits

#define fld_REC_LOADED	0x0001u		// data in TTptr->Workarea for this field
#define fld_ARRAY		0x0002u		// Field points at an Array variable. (Not possible in a table)
#define fld_ZERO		0x0004u		// field currently hold a zero value or null string
#define fld_SUBREC		0x0008u		// This field points to a sub-record table ???
#define fld_DATA_AVAIL	0x0010u		// field has data loaded at *fld->FLDdata
#define fld_VAR_USED	0x0010u		// variable is referenced flag						**CLCOMP** only
//#define fld_????		0x0020u
#define fld_READ_ONLY	0x0040u		// READ_ONLY flag. Only possible for system vars.
//#define fld_????		0x0080u
#define fld_LIB			0x0100u		// **DC Extension**. This variable holds a dynamic loaded library handle
#define fld_FUNC		0x0200u		// **DC Extension**. This variable holds a dynamic function ptr handle
#define fld_ADDROF		0x0400u		// **DC Extension**. Indicates to pass the *ADDRESS OF* this variable in function calls
//#define fld_????		0x0800u
//#define fld_????		0x1000u
//#define fld_????		0x2000u
//#define fld_????		0x4000u
#define fld_KEY			0x8000u

//rtab Opcode bits

#define rtb_GET			0x0001u		// GET		record
#define rtb_GETNEXT		0x0002u		// GETNEXT	record
#define rtb_GETPREV		0x0004u		// GETPREV	record
#define rtb_PUT			0x0008u		// PUT		record
#define rtb_DELETE		0x0010u		// DELETE	record
#define rtb_CLEAR		0x0020u		// CLEAR	record
#define rtb_JOIN		0x0040u		// JOIN
#define rtb_SELECT		0x0080u		// SELECT	expression
#define rtb_SEL_ALL		0x0100u		// "ALL" specifier used with SELECT, GETNEXT/PREV command
#define rtb_LOCK		0x0200u		// LOCK		attribute
#define rtb_LOCKR		0x0400u		// LOCKR	lock request attribute
#define rtb_REPLACE		0x0800u		// REPLACE #replace t1 dbname 'Tranfl1' 'tf1' **UNDOCUMENTED** 
//#define rtb_???		0x1000u


// xtab->Flags flag bits

#define xtb_CHAR		0x0001u		// expect string value
#define xtb_DATE		0x0002u		// expect DATE value
#define xtb_DISPLAY		0x0004u		// this is a DISPLAY ONLY field
#define xtb_FDF			0x0008u		// FDF First Display Field for a block
#define xtb_JOIN		0x0010u		// JOIN command follows this accept field
#define xtb_LOCK		0x0020u		// LOCK ????
#define xtb_LKF			0x0040u		// LKF Last Key Field
#define xtb_NAME		0x0080u		// RANGE "NAME" type
#define xtb_NOJOIN		0x0100u		// NOJOIN command follows this accept field
#define xtb_NUMERIC		0x0200u		// expect NUMERIC value
#define xtb_REQUIRED	0x0400u		// value is required for this field
#define xtb_TIME		0x0800u		// expect TIME value
//#define xtb_???		0x1000u
//#define xtb_???		0x2000u
//#define xtb_???		0x4000u
#define xtb_KEY			0x8000u		// This is a Key field in table

// reserved TDno table names

#define	td_tables		0	// "tables"
#define	td_fields		1	// "fields"
#define	td_locks		2	// "locks"

// clgetyx parameters

#define	yx_answer		1
#define	yx_prompt		2	

// update() modes

#define upd_delete		0	// delete record
#define	upd_put			1	// put record
#define	upd_lock		2	// lock record	( create a lock record in TD2 )

#endif
