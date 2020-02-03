#ifndef DBDEFS
#define DBDEFS

#include <pthread.h>	// for pthread specific stuff below
#include <time.h>       // for struct tm
#include <sys/times.h>  // for times()

//*************************************************************
// comment out to use our own local versions of these variables
//#define USE_LIB_CL4
//
//*************************************************************
// various debugs , uncomment to send debug output to stdout
// #define DEBUG_CL4
// #define DEBUG_DUMP
//*************************************************************
// Quick MACROs to swap bytes around

#define SWAP_SHORT(x)(*((unsigned char*)(x)+1) | (unsigned short)(*x << 8))
#define MSHORT(x) ((unsigned short)(x << 8) | (unsigned short)((x >> 8)& 0xFF))
#define M_INT(x) ((unsigned int)(x << 24) | (unsigned int)((x >> 8) & 0x0000FF00 )| (unsigned int)((x << 8)& 0x00FF0000)| (unsigned int)((x >> 24)& 0xFF))

// macros for accessing CL4 tables easily
//#define PTARR(x) (x ? &ptarr.TableAddr[x - 1] : 0)	// PTAB - Program instructions - is a *function* in REAL clcomp
#define ENARR(x) (x ? &enarr.TableAddr[x - 1] : 0)	// ENATB - Expressions 
#define XTARR(x) (x ? &xtarr.TableAddr[x - 1] : 0)	// XTAB  - Screen display / accept fields
#define PRARR(x) (x ? &prarr.TableAddr[x - 1] : 0)	// PRTAB - Print variables/fields
#define KXARR(x) (x ? &kxarr.TableAddr[x - 1] : 0)	// KXTAB - Create temp tables
#define XFARR(x) (x ? &xfarr.TableAddr[x - 1] : 0)	// XFTAB - Copy (xfer) from table to table
#define RDARR(x) (x ? &rdarr.TableAddr[x - 1] : 0)	// RDTAB re-display
#define RTARR(x) (x ? &rtarr.TableAddr[x - 1] : 0)	// relation specifier (join etc.)
#define RAARR(x) (x ? &raarr.TableAddr[x - 1] : 0)	// range specifier
#define SFARR(x) (x ? &sfarr.TableAddr[x - 1] : 0)	// file specifier

#define STRARR(x) (x ? &strarr.StringTable[x - 1] : 0)	// file specifier

#define max_it &itab[no_itabs]

typedef struct 
{
	short	KXFlags;
	short	WhereEXP;
	short	NumTTs;
	short	field_6;
	short*	KeyElemIDs;
	short	NumKeyFields;
	short	field_E;
	short*	TkeyTy;
	short*	DataElemIDs;
	short	NumDataFields;
	short	KeySize;
	short	RecSize;
	short	field_1E;
	int		TTno;
	int*	TTlist;
	char	TableName[24];
}KXTAB;			// create temp table records

typedef struct 
{
	char	*AttrName;
	short	*AttrData;
}TCSCRATTR;

typedef struct 
{
	char	*AttrName;
	char	**AttrStrData;
}TCSTRATTR;

typedef struct 
{
	char	*AttrName;
	int		*AttrBool;

}TCBOOLATTR;

typedef struct 
{
	short	RangeType;
	short	RangeFrom;
	short	RangeTo;
	short	StringOffset;
	short	field_8;
	short	field_A;
}RATAB;		// Range type records

typedef struct 
{
	short	cmd_flags;		// command flag bits
	short	PRTno;			// PRTAB no. describing variable to be read/written
	short	AliasEnt;		// Expression number describing Aliasname
	short	FileNameEnt;	// Expression number describing filename
}SFTAB;

typedef struct 
{
	short	FCBFlags;
	short	field_2;
	int		child_PID;		// child PID for pipes
	int		field_8;
	int		field_C;
	FILE*	RD_FILE;
	FILE*	WR_FILE;
	char*	ALIAS;
}FCB;

typedef struct 
{
	char*	KeyName;
	int		KeyNo;
}KEYTAB;

typedef struct	
{
	int		TTno_to;
	int		TTno_from;
}XFTAB;

typedef struct				// Label Definitions
{
	short	NameLen;		// Length of LabelName that follows structure
	short	PTno;			// PTAB#
	char	LabelName[2];	// Variable size structure
}LTAB;	// sizeof() == 6

typedef struct 
{
	short	VAR_ENTno;		// 0 expression describing variable we are outputting
	short	WIDTH_ENTno;	// 2 expression describing wdith required, or number of times to repeat
	short	WIDTH_FLAGS;	// 4 width/number of times to repeat. May also have upper bit flags set 
	short	ReservedFlags;	// 6 Non-zero means print reserved value
	short	NextPR;			// 8 next 
}PRTAB;

typedef struct 
{
	short	PTfalse;		// Jump to this instruction when "IF" expression  evaluates to false
	short	PTABno;
	int		HasElse;		// true if IF/ENDIF block has an 'ELSE' case!
	int		HasElsif;
	short	itType;			// 1 = 'if', 2 = 'while' ?
	short	field_E;
}ITAB;

typedef struct 
{
	short	LockType;
	short	OpType;
	short	WhereEXP;
	short	field_6;
//	int		TTno;
//	short	field_C;
//	short	field_E;
//	short	NextRT;
//	short	field_12;
	int		TTno;
	short	enleft;			// Pointer to expression.
	short	enright;		// Pointer to expression. Used by JOIN and REPLACE
	short	NextRT;			// chain to next RTAB (if required)
	short	field_12;		// padding
}RTAB;

//=============================================

// ENTAB defs

typedef struct
{
	short Enoper;
	short RecNo;
} ENOP;

typedef struct
{
	short TTno;
	short VarNum;
} ENREF;

typedef union
{

	ENOP	Enop;
	ENREF	Enref;
	float	float4;
	long	long8;
	char	char16[4];   

} ENUN;

typedef struct 
{
	short	entype;
	short	extOpCode;		// unused in real CL4 - could be used for extended opcodes??
	//short	TTno;			// enum which can hold few diff types
	//short	RecNo;
	ENUN	Enun;
	short	enleft;			// LVALUE	Dest	tree structue pointing at other expressions
	short	enright;		// RVALUE	Src
}ENTAB;

//=============================================

typedef struct 
{
	short	RDTFlags;
	short	ENTno;			// expression describing single field
	int		TTno;
	short	NextRD;
	short	field_A;
}RDTAB;

typedef struct VARTREE VARTREE;
struct VARTREE
{
	short	VarNo;
	short	field_2;
	char	*VarName;
	VARTREE	*VarPrev;
	VARTREE	*VarNext;
};

typedef struct SCR SCR;	// same as type24
struct SCR
{
	short	PT_start;	// LineNo
	short	PT_end;		// LineNo2
	SCR		*NextSCR;
};

typedef struct ONESC ONESC;

struct ONESC
{
	 short	OESCFlags;
	 short	BlockNo;
	 short	KeyNumber;
	 short	field_6;
	 char	*BlockName;
	 ONESC	*NextESC;
};

typedef struct 
{
	short	Flags      ;		// 0
	short	Attr       ;		// 2
	short	ScrAttrib  ;		// 4
	short	FLDnum     ;		// 6	Number displayed on screen when Field Numbering is *on*	short	field_6    ;
	short	Width      ;		// 8
	short	width      ;		// 10
	short	PCol	    ;		// 12
	short	PLine		;		// 14
	short	ACol		;		// 16
	short	ALine	   ;		// 18
	short	C_X		   ;		// 20
	short	C_Y		   ;		// 22
	ONESC	*onesc      ;		// 24
	short	RangeID    ;		// 28
	short	field_1E   ;		// 30
	int		field_20   ;		// 32
	short	VarExpNo   ;		// 36
	short	widthEXP   ;		// 38
	short	PCol_exp   ;		// 40
	short	PLine_exp  ;		// 42
	short	ACol_exp   ;		// 44
	short	ALine_exp  ;		// 46
	short	Prompt_exp ;		// 48
	short	field_32   ;		// 50
}XTAB;		// sizeof = 52

typedef struct EXPR EXPR;		// forward ref

typedef struct //(sizeof=0x48)
{
		int		BTFlags;		//0		not set in clcomp
		short	EndLine;		//4
		short	fmts;			//6
		short	StartLine;		//8
		short	TTno;			//0A
		short	Scrs;			//0C
		short	field_E;		//0E	padding
		EXPR	*EXPR;			//10
		ONESC	*On_delete;		//14
		ONESC	*On_exit;		//18
		ONESC	*On_excep;		//1C
		ONESC	*On_entry;		//20
		SCR		*Screens;		//24	screen ptr
		char	SrchMode;		//28
		char	Curr_Mode;		//29
		char	Maintain[7];	//2A
		char	BlockName[21];	//31
		short	field_46;		//46	padding
}BTAB;

typedef struct		//(sizeof=0x8)
{
		short	SrcLineNo;
		short	OpCode;
		short	Operand;	// multi-purpose field. Mainly used by "set" commands
		short	TABno;		// reference to another table. depends on OpCode type.
}PTAB;

typedef struct INCLUDE INCLUDE;

struct INCLUDE
{
		short	LastLine;
		short	field_2;
		FILE	*filePTR;
		char	*FileName;
		INCLUDE	*parentINC;
		INCLUDE	*childINC;
};

typedef struct 
{
	int		StrMaxSize;
	int		StrTableSize;
	char	*StringTable;
} CLStrArray;


typedef struct 
	{
	short	Qdeflen;	//0		Default 
	short	QLunk1;		//2		??
	char*	QfieldHd;	//4		String Field Heading
	char*	Qdata;		//8		Variable Size work area. Vartype dependent
	char	Qrequired;	//12	Is Value required?
	char	Qunused[3];	//13
	} Qlen;

typedef struct 
	{
	short	TDFtype;	//0
	short	FLDstat;	//2		// &0x8000 = Key Field , bit 0 set = data available
	short	FLDelemID;	//4		// Field ID in Elements table
	short	FLDlen;		//6		// Field length in DBase
	Qlen	*FLDqlen;	//8		// Ptr to 16byte work area
	char	*FLDname;	//12	// Ptr to Field Name
	char	*FLDdata;	//16	// Data area, gets filled when recs read
	char	FLDtype;	//20	// Field Type
	char	field_15;	//21	// not set, but clcomp examines!!!
	char	FLDunk7;	//22
	char	FLDunk8;	//23	// 3 bytes unused? sizeof() = 24   x18
	} FLDdesc;

// Bitmasks for FLDstat field:
#define		FLDdatav	0x0001;
#define		FLDkey		0x8000;

typedef struct 
	{
	int		DBno;			//x00	0		// 
	char	*FullDBname;	//x04	4		// DBase name 
	int		DBcontext;		//x08	8		// Context returned by CLinit()
	} DBase ;			// This struct lives at *(dtab + (12 * DBno))

typedef struct 
	{
	int		DBcontext;		//x00	0		Context identifies user. Returned by CLinit()
	short	TDlocked;		//x04	4		Bit 6 indicates locked (0x0040), 5(0x20) = Data Available (bit 0 = unlocked? see _ulckrec()
	short	NumFields;		//x06	6		Number of fields in a record
	short	TTrtd;			//x08	8		Relative TD number. (returned by rtdget(TTptr))		
	short	TDrecsize;		//x0A	10		Size of Record in bytes
	int		DBnumber;		//x0C	12		Associated DBnumber
	EXPR*	TTselect;		//x10	16		ptr to Select Expression
	FLDdesc	*TTfields;		//x14	20		Pointer to an Array of FieldDescriptors
	char	TableName[21];	//x18	24		Limited Table name
	char	TableAlias[23];	//x2D	45		Short Table Alias name
	char*	TDworkarea1;	//x44	68		Ptr to work area - (RecSize + 1)
	char*	TDworkarea2;	//x48	72		Ptr to work area - (RecSize + 1)
	short	TableCRC;		//x4C	76		CRC the data table for accuracy
	short	TDunknown10;	//x4E	78		??

	} TDesc ;			//This struct lives at *(ttab + (80 * TDno))

// Bitmasks for TDlocked field:
#define		TDdatav		0x0020;

// TDesc structure varies in length depending on the program!!!!!!!
// SCO (CL v4.1) = 76,  Linux V4.3,v4.4 = 80 bytes)

typedef struct 
	{
	short	TDFentry;		//x00	0		Points to field def in "Tables"
	short	TDFsize;		//x02	2		Size of field in bytes
	short	TDFtype;		//x04	4		Field type (as per CL4 types)		
	} TDef;	//pointed to by TDinfo->TableDefs //x14	20

// Often set to point at Shape Tables:
// fshape, xfshape (VFLAG=2)

// forward reference
typedef struct DBinfo DBinfo;
struct DBinfo{
	DBinfo	*DBnextDB;		//x00	0		PTR to next DBinfo structure if linked (Joins?)
	short	DBfilehandle;	//x04	4		File Handle for DBase file
	short	DBpgsize;		//x06	6		Short PageSize??
	int		DBunknown3;		//x08	8
	int		DBunknown4;		//x0C	12
	short	DBnumtables;	//x10	16		Number of tables in DBase
	short	DBunknown5	;	//x12	18		
	int		DBtableaddress;//x14	20		00000020 Offset in file of table defs
	short	DBmaxtables;	//x18	24		Maximum number of Tables allowed
	short	DBvflag;		//x1A	26		VFLAG Volume Bitmap offset ??
	short	DBversion;		//x1C	28		Version type ($06e6)
	short	DBunknown6;		//x1E	30
	int		DBfreeaddress;	//x20	32		FreeAddress - Start of Data??
	int		DBmaxpages;		//x24	36		Maximux grow size of DB in # pages
	int		PageList1[16];	//x28	40		unknown space
	int		PageList2[14];	//x68	104		unknown space
	int		PageList3[14];	//xA0	160		unknown space
	int		SpareList[33];	//xD8	216		Pointer table?
	int		DBopenflg;		//x15C	348		DB open ?
	int		Unkspac4[3];	//x160	352		unknown space
	char	*DBname;		//x16C	364		Ptr to full pathname
	int		DBSemaphore;	//x170	368		Set to ZERO by _rdhdr in V4.4 (not in V4.3)
	};			// returned by _chkdb(DBno)

// DBinfo structure is (x174) 372 bytes in length (in CL v4.3, v4.4 anyway)

typedef struct NODE NODE; // forward reference
typedef struct SALLOCBUF SALLOCBUF;	// fwd ref

typedef struct
	{
	short		TDFlags;		//x00	0		int 00000049 ?    Bitfield - 
	short		DBunknown2;		//x02	2		?    Bitfield - 
	DBinfo		*TDDBinfo;		//x04	4		ptr -> Controlling DBinfo structure
	short		TDindexOff;		//x08	8		short 00000011 (Offset into Index Table)
	short		DBunknown4;		//x0A	10		short 00000011 ??
	int			HalfPageSize;	//x0C	12		Page size in Bytes (/2 ?)
	int			Rec_plus_DB;	//x10	16		RecSize + DBPageSize
	TDef		*TableDefs;		//x14	20		ptr -> Array of Tdef (Shape) structures?
	SALLOCBUF*	TDSallocBuf;	//x18	24		00000000
	int			TDRecSize;		//x1C	28		int record size			000002C7
	int			TDRecMin;		//x20	32		int record minimum size 000000BF
	int			TDKeySize;		//x24	36		int key size			00000004
	int			TDKeyMin;		//x28	40		int key minimum size	00000004
	NODE		*TDNodePtr;		//x2C	44		Ptr to NODE structures
	short		N1_2idx;		//x30	48		Index into Node_1/NODE_2 arrays
	short		DBunknown13;	//x32	50		??
	void		*KeyBuf1;		//x34	52		int 00010004  ???
	short		Key1Size;		//x38	56		set by restrict()
	short		field_3A;		//x3A	58		"	ditto	" 
	void		*KeyBuf2;		//x3C	60		int 00020000  ???
	short		Key2Size;		//x40	64		set by restrict()
	short		field_42;		//x42	66		"	ditto	" 
	void*		TDKeyDefs;		//x44	68		ptr -> KeyDefs Table
	short		KeyDefSize;		//x48	72		??
	short		TDunknown20;	//x4A	74		??

	} TDinfo;					// returned by _chktd(TDno) = 76bytes

// VAriable length structure.
// TDinfo structure is created by _mktable function.
// Normally 76 bytes + Keysize long

typedef struct 
	{
	int	  DBunknown1;
	short DBfilehandle;
	short DBversion ;
	short DBpagesize;
	int	  DBnumpages;
	int	  DBfreeaddress;
	short DBnumtables;
	int	  DBtableaddress;
	short DBvflag;
	short DBmaxtables;
	} DBHdr ;

typedef struct 
	{
	short		DomID;		//x00	0
	short		DomUnk1;	//x02	2
	short		DomOffset;	//x04	4
	short		DomUnk2;	//x06	6
	} DOMAINR;

// forward reference for Elist* members:
typedef struct Elist Elist;
struct Elist{
	int		Size;		//x00	0
	Elist	*field_4;	//x04	4
	Elist	*field_8;	//x08	8
	Elist	*field_C;	//x0C	12
	};	// sizeof(Elist) = 16

typedef struct
{
	short	TBid;		//x00	0
	short	TBunk1;		//x02	2
	short	TBunk2;		//x04	4
} ShapeTbl;

typedef struct
{
	int		PosId;		//x00	0
	int		PosOf;		//x04	4
} PosTbl;

typedef struct 
{
	short	ElementID;	//(sizeof=0x8)
	short	field_2;
	int		BuffOffset;	// not sure if this is short or int. short leads to seg fault!
}POS ;

typedef struct 
{
	int		PageNo;
	short	Depth;
}HDRTABLE;
 
 // Record in "tables" TD0:
typedef struct
{
	char	TableName[22];		//x00	0
	short	IndexOff;			//x16	22
	short	HalfPageSize;		//x18	24
	short	RecPlusDBpagesize;	//x1A	26
	short	KeyFlags;			//x1C	28
} TD0REC;	// sizeof() = 30

//structure for "field" table entries  TD1 
typedef struct 
{		
	unsigned char	TableIndex;
	unsigned char	Rec_No;
	short			TDFentry;
	short			TDFtype;
	short			TDFsize;
}TD1REC;

//structure for "element" table entries 
typedef struct 
{		
	short			TDFentry;
	char			FieldName[20];
	short			FLDlen;
	char			Header[30];
	char			Header2[20];
	char			Required;
	char			Unused;
}ELEMENTREC;	// sizeof(76)


typedef struct 
{
	short	Version;	// Log Version . Correct value is 3576
	short	TransType;
	short	UID;
	short	field_6;
	int		PID;
	int		field_C;
	short	RecSize;
	short	field_12;
	short	TDno;
	short	field_16;
	int		Context;
	time_t	TimeT;
}TRANSLOG;	//        struc ; (sizeof=0x20) [32 bytes]

// Another structure used to index 'field' entries
typedef struct FIELD FIELD; //forward ref
struct FIELD
{
	FIELD	*NextFIELD;
	short	TDFentry;
	short	TDFsize;
	short	TDFtype;
	short	field_A;
};

//structure for "lock" table entries  TD2 
// pragmas stop GCC trying to align the PID field to a 4 byte boundary
#pragma pack(push, 1)
typedef struct 
{		
	unsigned char	TableIndex;
	char			KeyBuff[128];
	int				PID;
}TD2REC;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  PTAB *TableAddr;
}PTAB_ArrPtr;
#pragma pack(pop)

/*   64 */
#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  ENTAB *TableAddr;
}ENTAB_ArrPtr;
#pragma pack(pop)

/*   65 */
#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  XTAB *TableAddr;
}XTAB_ArrPtr;
#pragma pack(pop)

/*   66 */
#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  RTAB *TableAddr;
}RTAB_ArrPtr;
#pragma pack(pop)

/*   67 */
#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  RATAB *TableAddr;
}RATAB_ArrPtr;
#pragma pack(pop)

/*   68 */
#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  XFTAB *TableAddr;
}XFTAB_ArrPtr;
#pragma pack(pop)

/*   69 */
#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  SFTAB *TableAddr;
}SFTAB_ArrPtr;
#pragma pack(pop)

/*   70 */
#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  PRTAB *TableAddr;
}PRTAB_ArrPtr;
#pragma pack(pop)

/*   71 */
#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  RDTAB *TableAddr;
}RDTAB_ArrPtr;
#pragma pack(pop)

/*   72 */
#pragma pack(push, 1)
typedef struct 
{
  short MaxAvail;
  short NextElement;
  KXTAB *TableAddr;
}KXTAB_ArrPtr;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
  int	PageNo;
  short Depth;
  short field_6;
}RLIST;
#pragma pack(pop)

//===================================================================

//PAGE defs

typedef union
{
	char*	TabStart;
	int		RecSize;
} TSRS;

typedef struct 
{
	short	NumEntries; 
	short	field_2;
	char*	DataStart;
	TSRS	tsrs;
	char*	TabEnd;
} PAGEHDR;

typedef struct 
{
	unsigned short	PageType;			// This is first byte stored on a page on disk
	unsigned short	field_12;
	char	field_14;
	char	field_15;
	char	field_16;
	char	field_17;
	char	field_18;
	char	field_19;
	short	field_1A;			// ; Data normally starts here
} PAGEDATA;

typedef struct 
{
	PAGEHDR		header;
	PAGEDATA	pgData;
}PAGE;	//(sizeof=0x1E)

//====================================================================

struct EXPR{
	EXPR	*NextEXPR;			//struc ; (sizeof=0x10   (16))
	EXPR	*PrevEXPR;
	short	Operator;
	short	Type;
	short	ExprSize;
	char	ExprData;	// variable length struct. data copied here
	char	field_F;
};

typedef struct 
{
	char	*Data;
	short	Size;
	short	field_6;
} NODE_1 ;		//(sizeof=0x8)

typedef struct 
{
	int		PageNo;
} NODE_2 ;		//(sizeof=0x4)

struct NODE{         
	short	PageType;	//(sizeof=0x28); Variable length structure. First 24bytes are valid.
	short	field_2;
	short	PageNo;
	int		field_8;
	int		field_C;
	int		DataEnd;
	short	RecsInPage;
	short	field_16;
	short	NumEntries;
	short	field_1A;
	NODE_1	*NODE1ptr;
	NODE_2	*NODE2ptr;
	char	DataStart;		//Data is copied to this point in the structure
	char	field_25;
	char	field_26;
	char	field_27;
}; 

typedef struct 
{
	char*	Word;
	int		Rsv;
} ReservedWord;

#pragma pack(push, 1)
typedef struct REFR REFR;	// forward ref
struct REFR
{
  REFR **Data;
  char *Offset;
};
#pragma pack(pop)

// sallocbuff all a bit of a mystery at moment
#pragma pack(push, 1)
struct SALLOCBUF
{
  REFR *DataPtr;
  short field_4;
  short MaxRecs2;
  short MaxRecs1;
  short field_A;
  REFR refr;
  REFR **BufferEnd;
  NODE *NodeArr[6];
  RLIST *RL_Head;
  char DataStart;
  char field_35;
  char field_36;
  char field_37;
};
#pragma pack(pop)



typedef struct 
{
	int		PageNo;					//(sizeof=0x10)
	short	field_4;
	short	field_6;
	int		field_8;
	short	NumEntries;
	short	field_E;
} DPOS;

#pragma pack(push, 1)
typedef struct RLARR RLARR; //forward ref
struct RLARR
{
  RLARR		*NextRLARR;
  int		PageNo;
  short		Depth;
  short		field_A;
  DPOS		Dpos;
  int		field_1C;
  char		DataStart;
  char		field_21;
  char		field_22;
  char		field_23;
};
#pragma pack(pop)

// few globals in the cl4lib.
// change from extern as necessary when replacement functions are done.


// 
// Use LIBCL4 versions of variables:
#ifdef USE_LIB_CL4

extern int				no_dtabs;
extern int				no_ttabs;
extern TDesc			*ttab;				// pointer to array of TDesc structures. Doesn't look right, but works...
extern DBase			*dtab;				// pointer to array of DBase structures
	
extern int				sz_shape[4];
extern TDef				_lshape[4];
extern TDef				_xfshape[7];
extern TDef				_fshape[6];
extern TDef				_tshape[6];
extern short			_nspare;
//extern int			_psize;				// Current page size (DB page size + 16)
extern short			_psize;				// default page size for temp files: 0x1000
extern int				_fsize;				// default file size for temp files: 0x7A120

extern POS				_tpos[6];			// reposition data for "tables" : TD0
extern POS				_fpos[6];			// reposition data for "fields" : TD1

// see dates.c
extern char*			_YD;				// = "1955-2022:10"
extern char*			_YL;				// = "\n\n\0"
extern int				_DF;				// Date Format 0,1, or 2
extern int				defDateWidth;		// Default date width = 10 (0x0A)
extern char				THOU_SEP[10];		// used by loadMonthArrays
extern char				RADIX_CHAR[10];		// used by loadMonthArrays

//extern ReservedWord		reswords[115];
extern char				cwd[80];
extern char				pname[80];			// possible name contention here!!!!
extern char*			_pname;				// points to "cldblib" [syserror()]
extern int				time_stamp;			// used by syserror()
extern pthread_mutex_t	mutex;
extern pthread_key_t	ctxKey;				// used by CLgetError()
extern pthread_t		threads[256];
extern char*			_pnameArr[256];		// used by CLinit(), CLdeinit()
extern char*			pnameArr[256];		// used by CLinit(), CLdeinit()
extern int				contexts[256];		// Array of Context values. Context = PID + 10000
extern int				tx_started[256];
extern int				prev_tx[256];
extern int				cl_tx;				// transaction count?					
extern __pid_t			lpid;				
extern int				lfd[256];			// Array of Log file descriptors. Indexed by Context
extern int				cldblibInit;		// set to "1" when CLinit() finished

extern int				exact;				// Exact match on field names	default = 0
extern int				isClDemo;			// Is this a demo version of libcl4? 1 = yes
extern short			trim_it;			// Used to temporarily override trim on/off value

extern double			_dbias[7];			// used by xtodom()

extern int				isCGI;				// used by cllogerr()

#endif

// Use our own local versions:
#ifndef USE_LIB_CL4

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

char			pname[80];			// possible name contention here!!!!
char			*_pname;				// points to "cldblib" [syserror()]
int				time_stamp;			// used by syserror()
pthread_mutex_t	mutex;
pthread_key_t	ctxKey;				// used by CLgetError()
pthread_t		threads[256];
char			*_pnameArr[256];	// used by CLinit(), CLdeinit()
char			*pnameArr[256];		// used by CLinit(), CLdeinit()
int				contexts[256];		// Array of Context values. Context = PID + 10000
int				tx_started[256];
int				prev_tx[256];
int				cl_tx;				// transaction count?					
__pid_t			lpid;				
int				lfd[256];			// Array of Log file descriptors. Indexed by Context

// used by lefine() and putconst() to determine float var bit flags
unsigned short FPtypes[] = { 0x0000u, 0x00A4u, 0x0084u, 0x00E4u, 0x00C4u, 0x0124u, 0x0104u, 0x0164u, 0x0144, 0x01A4u};

int		cldblibInit = 0;			// set to "1" when CLinit() finished
int		exact		= 0;			// Exact match on field names	default = 0
int		isClDemo	= 0;			// Is this a demo version of libcl4? 1 = yes
short	trim_it		= 0;			// Used to temporarily override trim on/off value
int		isCGI;						// used by cllogerr()

double	_dbias[] = {1 , 100, 10000, 1e+06, 1e+08, 1e+10, 1e+12};	// used by xtodom()

// Max_nos data in memory:
/*int	MAX_nos[]	= {
0x0,0x0,0x0A2000000,0x426D1A94,0x0A2000000,0x426D1A94,0x20000000,0x4202A05F,
0x20000000,0x4202A05F,0x0,0x4197D784,0x0,0x4197D784,0x0,0x412E8480,0x0,
0x412E8480,0x0,0x40C38800};
*/

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


#endif
//--------------------------------------
// following are not public in libcl4:
//--------------------------------------

//extern POS		_pos[6];    // pos and pos_0 are *NOT* public!
//extern POS		_pos_0[4];

POS My_pos[] = {107, 0 , 0,
				106, 0, 22,
				108, 0, 28,
				109, 0, 24,
				110, 0, 26,
				0  , 0,  0};

POS My_pos_0[] = {102, 0, 4,
				  104, 0, 6,
				  103, 0, 8,
				  0, 0,	0};

// Alternative for _pos_0 which doesn't get all data from "fields"
POS MyPOS1[] = {106, 0, 0,
				101, 0, 1,
				102, 0, 2,
				103, 0, 4,
				104, 0,	6,
				0,	0,	0};

POS MyTpos[] = {107, 0, 0,
				106, 0, 22,
				108, 0, 28,
				109, 0, 24,
				110, 0,	26,
				0,	0,	0};

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

TDinfo	tdef = {0x0CD,0,0,0,0,0x100,0x212,_tshape,0, 27,  8, 20,  1,0,0,0,0,0,1,0,0,1,tkey,0,0};
TDinfo	fdef = {0x00D,0,0,1,0,0x100,0x208,_fshape,0,  8,  8,  2,  2,0,0,0,0,0,1,0,0,1,fkey,0,0};
TDinfo	ldef = {0x029,0,0,2,0,0x100,0x200,_lshape,0,133,133,129,129,0,0,0,0,0,1,0,0,1,lkey,0,0};

TDinfo	*def_1[] = {&tdef,&fdef,&ldef};

TDinfo	*_tbl[3005]	= {&tdef,&fdef,&ldef};

#define DB_MAX 255			// max size of open databases array
DBinfo	*_dbase[DB_MAX];

char	str_0[8192];		// used by getevar() and others?

char	buf_0[3072];		// used by CLdbschema and others?
char	buf_2[6144];		// used by CLtdschema and others?
char	buffer_0[120];		// used by _getfldno()
PAGE*	pbuf = 0;			// PageBuffer pointer used by getpg(), cdbfreepg()
int		psize = 0;			// current page size in bytes

int		fcount;				// Index fetch count : 

TRANSLOG	tl;				// used by cldel/put functions. Transaction Log structure
bool	logging = true;		// ditto, default value = logging is *ON*
int		before_logging;		// ditto, used to store int value of evar: "CLTBEFORELOG"
char	l_tty[20];			// Logging TTY id
char*	nlDbases_1;			//
int		tdArr_0[3005];		//

char	cdbtmpdir[100];		// used by _mktmp()
char	defcdbtdir[] = "/tmp";

FILE*	fp_1;				// used by eprint()

// see dates.c
char	*_YD = "1990-2050:10";		// Default width = 10: dd-mm-yyyy   old: "1955-2022:10"
char	*_YL = "\n\n\0";
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


// sutimes.c
int		retime = 0;
int		rstime = 0;
int		sustart = 0;
struct tms	tme;			// Time End
struct tms	tms;			// Time Start

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

