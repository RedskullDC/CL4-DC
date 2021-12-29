#ifndef LOCALVARS_C
#define LOCALVARS_C

// 62 reserved vars: 0 - 61

FLDdesc	lvars[] = {
//TDFtype FLDstat FLDelemID FLDlen *ql *FLDname   *FLDdata FLDtype field_15 
	0x1a,	0x54,	0,		20,		0,	"NULL",			0,	 'C',	0,		//0
	0x44,	0x14,	0,		2,		0,	"ERROR",		0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"LOCKED",		0,	 'I',	0,	
	0x1a,	0x14,	0,		20,		0,	"MODE",			0,	 'C',	0,	
	0x44,	0x54,	0,		2,		0,	"ZERO",			0,	 'I',	0,	
	0x1a,	0x14,	0,		20,		0,	"REPLY",		0,	 'C',	0,		//5
	0x44,	0x14,	0,		2,		0,	"EXIT",			0,	 'I',	0,	
	0x1a,	0x14,	0,		20,		0,	"LREPLY",		0,	 'C',	0,	
	0x1a,	0x14,	0,		128,	0,	"V1",			0,	 'C',	0,		//8
	0x1a,	0x14,	0,		128,	0,	"V2",			0,	 'C',	0,	
	0x1a,	0x14,	0,		128,	0,	"V3",			0,	 'C',	0,		//10
	0x1a,	0x14,	0,		128,	0,	"V4",			0,	 'C',	0,	
	0x1a,	0x14,	0,		128,	0,	"V5",			0,	 'C',	0,	
	0x1a,	0x14,	0,		128,	0,	"V6",			0,	 'C',	0,	
	0x1a,	0x14,	0,		128,	0,	"V7",			0,	 'C',	0,	
	0x1a,	0x14,	0,		128,	0,	"V8",			0,	 'C',	0,		//15
	0x1a,	0x14,	0,		128,	0,	"V9",			0,	 'C',	0,	
	0x44,	0x14,	0,		2,		0,	"INTERRUPT",	0,	 'I',	0,	
	0x1a,	0x14,	0,		20,		0,	"MREPLY",		0,	 'C',	0,	
	0x44,	0x14,	0,		4,		0,	"LCOUNT",		0,	 'N',	0,	
	0x44,	0x14,	0,		4,		0,	"PCOUNT",		0,	 'N',	0,		//20
	0x44,	0x14,	0,		4,		0,	"PLINES",		0,	 'N',	0,	
	0x44,	0x14,	0,		4,		0,	"SLINES",		0,	 'N',	0,		//22
	0x44,	0x14,	0,		4,		0,	"SCOLS",		0,	 'N',	0,	
	0x1a,	0x14,	0,		1000,	0,	"MATCH",		0,	 'C',	0,	
	0x44,	0x14,	0,		2,		0,	"LMATCH",		0,	 'I',	0,		//25
	0x44,	0x54,	0,		2,		0,	"OUTPUT",		0,	 'I',	0,	
	0x44,	0x14,	0,		2,		0,	"GOBACK",		0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"ABORT",		0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"AAUTO",		0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"RAUTO",		0,	 'I',	0,		//30
	0x1a,	0x54,	0,		20,		0,	"FILL",			0,	 'C',	0,	
	0x44,	0x54,	0,		2,		0,	"ALIGN",		0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"BIGE",			0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"FNUM",			0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"NEXIT",		0,	 'I',	0,		//35
	0x44,	0x54,	0,		2,		0,	"SKIP",			0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"TRIM",			0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"DEBUG",		0,	 'I',	0,	
	0x1a,	0x14,	0,		1,		0,	"TAB",			0,	 'C',	0,	
	0x44,	0x54,	0,		2,		0,	"JUSTIFY",		0,	 'I',	0,		//40
	0x44,	0x54,	0,		2,		0,	"GOING_BACK",	0,	 'I',	0,	
	0x44,	0x14,	0,		2,		0,	"BYPASS",		0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"REPEAT",		0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"HANGUP",		0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"CONVERT",		0,	 'I',	0,		//45
	0x44,	0x54,	0,		2,		0,	"RAWDISPLAY",	0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"RAWPRINT",		0,	 'I',	0,	
	0x44,	0x14,	0,		2,		0,	"SCONVERR",		0,	 'I',	0,	
	0x44,	0x14,	0,		2,		0,	"LSCONV",		0,	 'I',	0,	
	0x44,	0x14,	0,		2,		0,	"MLINE",		0,	 'I',	0,		//50
	0x44,	0x14,	0,		2,		0,	"MCOL",			0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"XML_DEPTH",	0,	 'I',	0,	
	0x44,	0x14,	0,		2,		0,	"DATEFORM",		0,	 'I',	0,	
	0x1a,	0x54,	12,		15,		0,	"MONTHNAMES",	0,	 'C',	0,	
	0x1a,	0x54,	12,		6,		0,	"ABMONTHNAMES",	0,	 'C',	0,		//55
	0x1a,	0x54,	7,		12,		0,	"DAYNAMES",		0,	 'C',	0,	
	0x1a,	0x54,	7,		6,		0,	"ABDAYNAMES",	0,	 'C',	0,	
	0x1a,	0x14,	0,		128,	0,	"PAUSE_PROMPT",	0,	 'C',	0,	
	0x44,	0x54,	0,		2,		0,	"XML",			0,	 'I',	0,	
	0x44,	0x54,	0,		2,		0,	"DOS",			0,	 'I',	0,		//60
	0x44,	0x54,	0,		2,		0,	"CSV",			0,	 'I',	0,	
	0,		0,		0,		0,		0,	0,				0,	   0,	0		//nullterminated
};

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

#endif
