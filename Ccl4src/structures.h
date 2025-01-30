#ifndef STRUCTS_H
#define STRUCTS_H

// common file for structure definitions

// Another (temp) structure used to index 'field' entries
typedef struct FIELD FIELD; //forward ref
struct FIELD
{
	FIELD	*NextFIELD;
	short	TDFentry;
	short	TDFsize;
	short	TDFtype;
};

typedef struct 
{
	short	KXFlags;
	short	WhereEXP;
	short	NumTTs;
	short*	KeyElemIDs;
	short	NumKeyFields;
	short*	TkeyTy;
	short*	DataElemIDs;
	short	NumDataFields;
	short	KeySize;
	short	RecSize;
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
	long	Spare;				// unused, but packs structure to 12 (x86) and 16 (x64)
}RATAB;		// Range type records  


typedef struct 
{
	short	RDTFlags;
	short	ENTno;			// expression describing single field
	int		TTno;
	short	NextRD;
	//short	field_A;
}RDTAB;

typedef struct 
{
	char	*abuf;
	char	*ccbuf;
	char	*ScreenName;
}STAB;				// Screen Buffer type records  (clenter only)

typedef struct 
{
	short	cmd_flags;		// command flag bits
	short	PRTno;			// PRTAB no. describing variable to be read/written
	short	AliasEnt;		// Expression number describing Aliasname
	short	FileNameEnt;	// Expression number describing filename
}SFTAB;

typedef struct 
{
	char*	KeyName;
	int		KeyNo;
}KEYTAB;

typedef struct 
{
	char*	Word;
	int		Rsv;
} ReservedWord;

typedef struct	
{
	int		TTno_to;		// transfer.   Used in copy instructions
	int		TTno_from;
}XFTAB;

typedef struct				// Label Definitions
{
	short	NameLen;		// Length of LabelName that follows structure
	short	PTno;			// PTAB#
	char	LabelName[2];	// Variable size structure
}LTAB;	// clcomp only

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
	short	VAR_ENTno;		// expression describing variable we are outputting
	short	WIDTH_ENTno;	// expression describing wdith required, or number of times to repeat
	short	WIDTH_FLAGS;	// width/number of times to repeat. May also have upper bit flags set 
	short	ReservedFlags;	// Non-zero means print reserved value
	short	NextPR;			// next 
}PRTAB;

typedef struct 
{
	short	LockType;
	short	OpType;
	short	WhereEXP;
	int		TTno;
	short	enleft;			// Pointer to expression.
	short	enright;		// Pointer to expression. Used by JOIN and REPLACE
	short	NextRT;			// chain to next RTAB (if required)
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
	ENUN	Enun;
	short	enleft;			// LVALUE	Dest	tree structue pointing at other expressions
	short	enright;		// RVALUE	Src
}ENTAB;

//=============================================

typedef struct VARTREE VARTREE;
struct VARTREE
{
	short	VarNo;
	short	field_2;
	char	*VarName;
	VARTREE	*VarPrev;
	VARTREE	*VarNext;
};

typedef struct SCR SCR;
struct SCR
{
	short	PT_start;	// LineNo
	short	PT_end;	// LineNo2
	SCR		*NextSCR;
};
typedef struct ONESC ONESC;

struct ONESC
{
	 short	OESCFlags;
	 short	BlockNo;
	 short	KeyNumber;
	 char	*BlockName;
	 ONESC	*NextESC;
};


typedef struct 
{
	short	Flags      ;
	short	Attr       ;
	short	ScrAttrib  ;
	short	FLDnum     ;		// Number displayed on screen when Field Numbering is *on*
	short	Width      ;
	short	width      ;
	short	PCol	   ;
	short	PLine	   ;
	short	ACol	   ;
	short	ALine	   ;
	short	C_X        ;
	short	C_Y		   ;
	ONESC	*onesc     ;
	short	RangeID    ;
	short	field_1E   ;
	char	*field_20  ;
	short	VarExpNo   ;
	short	WidthEXP   ;
	short	PCol_exp   ;
	short	PLine_exp  ;
	short	ACol_exp   ;
	short	ALine_exp  ;
	short	Prompt_exp ;
	short	field_32   ;		// read by loadenc, but not used??
}XTAB;

typedef struct EXPR EXPR;		// forward ref

typedef struct //(sizeof=0x48)
{
		int		BTFlags;		
		short	EndLine;		
		short	fmts;			
		short	StartLine;		
		short	TTno;			
		short	Scrs;			
		//short	field_E;		
		EXPR	*EXPR;			
		ONESC	*On_delete;		
		ONESC	*On_exit;		
		ONESC	*On_excep;		
		ONESC	*On_entry;		
		SCR		*Screens;		
		char	SrchMode;		
		char	Curr_Mode;		
		char	Maintain[7];	
		char	BlockName[21];	
		//short	field_46;		
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
		FILE	*filePTR;
		char	*FileName;
		INCLUDE	*parentINC;
		INCLUDE	*childINC;
};

typedef struct 
{
	short	MaxAvail;
	short	NextElement;
	char	*TableAddr;
} CLArrayPtr;

typedef struct 
{
	long	StrMaxSize;
	long	StrTableSize;
	char	*StringTable;
} CLStrArray;


typedef struct 
	{
	short	Qdeflen;	//0		Default 
	//short	QLunk1;		//2		??
	char*	QfieldHd;	//4		String Field Heading
	char*	Qdata;		//8		Variable Size work area. Vartype dependent
	char	Qrequired;	//12	Is Value required?
	//char	Qunused[3];	//13
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
	//short	TDunknown10;	//x4E	78		??

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
	long	DBunknown3;		//x08	8		Unused
	long	DBunknown4;		//x0C	12		Unused
	short	DBnumtables;	//x10	16		Number of tables in DBase
	//short	DBunknown5	;	//x12	18		
	long	DBtableaddress;//x14	20		00000020 Offset in file of table defs
	short	DBmaxtables;	//x18	24		Maximum number of Tables allowed
	short	DBvflag;		//x1A	26		VFLAG Volume Bitmap offset ??
	short	DBversion;		//x1C	28		Version type ($06e6)
	//short	DBunknown6;		//x1E	30
	long	DBfreeaddress;	//x20	32		FreeAddress - Start of Data??
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
	//short		DBunknown2;		//x02	2		?    Bitfield - 
	DBinfo		*TDDBinfo;		//x04	4		ptr -> Controlling DBinfo structure
	short		TDindexOff;		//x08	8		short 00000011 (Offset into Index Table)
	//short		DBunknown4;		//x0A	10		short 00000011 ??
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
	//short		DBunknown13;	//x32	50		??
	void		*KeyBuf1;		//x34	52		int 00010004  ???
	short		Key1Size;		//x38	56		set by restrict()
	short		field_3A;		//x3A	58		"	ditto	" 
	void		*KeyBuf2;		//x3C	60		int 00020000  ???
	short		Key2Size;		//x40	64		set by restrict()
	short		field_42;		//x42	66		"	ditto	" 
	void*		TDKeyDefs;		//x44	68		ptr -> KeyDefs Table
	short		KeyDefSize;		//x48	72		??
	//short		TDunknown20;	//x4A	74		??

	} TDinfo;					// returned by _chktd(TDno) = 76bytes

// VAriable length structure.
// TDinfo structure is created by _mktable function.
// Normally 76 bytes + Keysize long

typedef struct 
	{
	int			DBunknown1;
	short		DBfilehandle;
	short		DBversion ;
	short		DBpagesize;
	int			DBnumpages;
	int			DBfreeaddress;
	short		DBnumtables;
	int			DBtableaddress;
	short		DBvflag;
	short		DBmaxtables;
	} DBHdr ;

typedef struct 
	{
	short		DomID;		//x00	0
	short		DomUnk1;	//x02	2
	short		DomOffset;	//x04	4
	short		DomUnk2;	//x06	6
	} DOMAINR;


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

#pragma pack(push, 1)

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

//structure for "lock" table entries  TD2 
// pragmas stop GCC trying to align the PID field to a 4 byte boundary
typedef struct 
{		
	unsigned char	TableIndex;
	char			KeyBuff[128];
	int				PID;
}TD2REC;

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
#pragma pack(pop)

typedef struct 
{
	short	Version;	// Log Version . Correct value is 3576
	short	TransType;
	short	UID;
	short	field_6;
	int		PID;
	int		LPID;
	short	RecSize;
	short	field_12;
	short	TDno;
	short	field_16;
	int		cl_tx;
	time_t	TimeT;
}TRANSLOG;	//        struc ; (sizeof=0x20) [32 bytes]

//=============================================

typedef struct 
{
  short MaxAvail;
  short NextElement;
  PTAB *TableAddr;
}PTAB_ArrPtr;

typedef struct 
{
  short MaxAvail;
  short NextElement;
  ENTAB *TableAddr;
}ENTAB_ArrPtr;

typedef struct 
{
  short MaxAvail;
  short NextElement;
  XTAB *TableAddr;
}XTAB_ArrPtr;

typedef struct 
{
  short MaxAvail;
  short NextElement;
  RTAB *TableAddr;
}RTAB_ArrPtr;

typedef struct 
{
  short MaxAvail;
  short NextElement;
  RATAB *TableAddr;
}RATAB_ArrPtr;

typedef struct 
{
  short MaxAvail;
  short NextElement;
  XFTAB *TableAddr;
}XFTAB_ArrPtr;

typedef struct 
{
  short MaxAvail;
  short NextElement;
  SFTAB *TableAddr;
}SFTAB_ArrPtr;

typedef struct 
{
  short MaxAvail;
  short NextElement;
  PRTAB *TableAddr;
}PRTAB_ArrPtr;

typedef struct 
{
  short MaxAvail;
  short NextElement;
  RDTAB *TableAddr;
}RDTAB_ArrPtr;

typedef struct 
{
  short MaxAvail;
  short NextElement;
  KXTAB *TableAddr;
}KXTAB_ArrPtr;


//===================================================================

//PAGE defs
#pragma pack(push, 1)

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

// pragmas stop GCC trying to re-align some fields to 4 byte boundaries!!
typedef struct 
{
	short	DBversion;
	short	DBpgsize;
	int		DBmaxpages;
	int		DBfreeaddress;
	short	DBnumtables;
	int		DBtableaddress;
	short	DBvflag;
	short	DBmaxtables;
	char	Unused[10];

}DBhdr;		// Database header data
#pragma pack(pop)

//====================================================================


struct EXPR{
	EXPR	*NextEXPR;			//struc ; (sizeof=0x10   (16))
	EXPR	*PrevEXPR;
	short	Operator;
	short	Type;
	short	ExprSize;
	char	ExprData;	// variable length struct. data copied here
	//char	field_F;
};

typedef struct 
{
	char	*Data;
	short	Size;
	//short	field_6;	// padding to make alloc's match 4 byte boundaries.
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
	int		field_C;		// appears unused. However, is set address of DataStart in getnode()
	int		DataEnd;
	short	RecsInPage;
	short	field_16;		// Used for something? Set to either 0,2 or 8 in FreshNode
	short	NumEntries;
	short	field_1A;
	NODE_1	*NODE1ptr;
	NODE_2	*NODE2ptr;
	char	DataStart;		//Data is copied to this point in the structure
	//char	field_25;
	//char	field_26;
	//char	field_27;
}; 

typedef union
{
  PAGE *PAGE;
  NODE *NODE;
}PAGE_NODE;



//====================================================================

// structs related to temp tables

typedef struct
{
  int	PageNo;
  short Depth;
  //short field_6;
}RLIST;

typedef struct REFR REFR;	// forward ref
struct REFR
{
  REFR **Data;
  char *Offset;
};

// sallocbuff all a bit of a mystery at moment
struct SALLOCBUF
{
  REFR *DataPtr;
  short field_4;		// is used
  short MaxRecs2;
  short MaxRecs1;
  //short field_A;
  REFR refr;
  REFR **BufferEnd;
  NODE *NodeArr[6];
  RLIST *RL_Head;
  char DataStart;
  //char field_35;
  //char field_36;
  //char field_37;
};

typedef struct 
{
	int		PageNo;					//(sizeof=0x10)
	short	field_4;
	short	field_6;
	int		field_8;
	short	NumEntries;
	//short	field_E;
} DPOS;

typedef struct RLARR RLARR; //forward ref
struct RLARR
{
  RLARR		*NextRLARR;
  int		PageNo;
  short		Depth;
  //short		field_A;
  DPOS		Dpos;
  int		field_1C;
  char		DataStart;
  //char		field_21;
  //char		field_22;
  //char		field_23;
};

#endif

