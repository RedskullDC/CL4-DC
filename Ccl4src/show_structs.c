#ifndef SHOWSTRUCT_C
#define SHOWSTRUCT_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values



void showFLD(void)		// Just dump 1 Field Descriptor
{

	printf("\nStructure:               Current Size   Expected x86   Expected X64\n");
	printf("sizeof(BTAB)           =   %3ld (%04lX)      72 (0048)      96 (0060) \n",sizeof(BTAB),sizeof(BTAB));
	printf("sizeof(ENTAB)          =   %3ld (%04lX)      12 (000C)      24 (0018) \n",sizeof(ENTAB),sizeof(ENTAB));
	printf("sizeof(KXTAB)          =   %3ld (%04lX)      64 (0040)      88 (0058) \n",sizeof(KXTAB),sizeof(KXTAB));
	printf("sizeof(ITAB)           =   %3ld (%04lX)      16 (0010)      16 (0010) \n",sizeof(ITAB),sizeof(ITAB));
	printf("sizeof(LTAB)           =   %3ld (%04lX)       6 (0006)       6 (0006) \n",sizeof(LTAB),sizeof(LTAB));
	printf("sizeof(XTAB)           =   %3ld (%04lX)      52 (0034)      64 (0040) \n",sizeof(XTAB),sizeof(XTAB));
	printf("sizeof(XFTAB)          =   %3ld (%04lX)       8 (0008)       8 (0008) \n",sizeof(XFTAB),sizeof(XFTAB));
	printf("sizeof(RATAB)          =   %3ld (%04lX)      12 (000C)      12 (000C) \n",sizeof(RATAB),sizeof(RATAB));
	printf("sizeof(RDTAB)          =   %3ld (%04lX)      12 (000C)      12 (000C) \n",sizeof(RDTAB),sizeof(RDTAB));
	printf("sizeof(RTAB)           =   %3ld (%04lX)      20 (0014)      20 (0014) \n",sizeof(RTAB),sizeof(RTAB));
	
	printf("\n");
	
	printf("sizeof(FLDdesc)        =   %3ld (%04lX)      24 (0018)      40 (0028) \n",sizeof(FLDdesc),sizeof(FLDdesc));
	printf("sizeof(Qlen)           =   %3ld (%04lX)      16 (0010)      32 (0020) \n",sizeof(Qlen),sizeof(Qlen));
	printf("sizeof(TDinfo)         =   %3ld (%04lX)      76 (004C)     128 (0080) \n",sizeof(TDinfo),sizeof(TDinfo));
	printf("sizeof(TDesc)          =   %3ld (%04lX)      80 (0050)     104 (0068) \n",sizeof(TDesc),sizeof(TDesc));
	printf("sizeof(DBinfo)         =   %3ld (%04lX)     372 (0174)     408 (0198) \n",sizeof(DBinfo),sizeof(DBinfo));
	printf("sizeof(DBase)          =   %3ld (%04lX)      12 (000C)      24 (0018) \n",sizeof(DBase),sizeof(DBase));

	printf("\n");

	printf("sizeof(SALLOCBUF)      =   %3ld (%04lX)      56 (0038)     104 (0068) \n",sizeof(SALLOCBUF),sizeof(SALLOCBUF));
	printf("sizeof(RLIST)          =   %3ld (%04lX)       8 (0008)       8 (0008) \n",sizeof(RLIST),sizeof(RLIST));
	printf("sizeof(RLARR)          =   %3ld (%04lX)      36 (0024)      40 (0028) \n",sizeof(RLARR),sizeof(RLARR));
	printf("sizeof(DPOS)           =   %3ld (%04lX)      16 (0010)      16 (0010) \n",sizeof(DPOS),sizeof(DPOS));
	printf("sizeof(NODE)           =   %3ld (%04lX)      40 (0028)      56 (0038) \n",sizeof(NODE),sizeof(NODE));
	printf("sizeof(NODE_1)         =   %3ld (%04lX)       8 (0008)      16 (0010) \n",sizeof(NODE_1),sizeof(NODE_1));
	
	printf("\n");

	printf("sizeof(ONESC)          =   %3ld (%04lX)      16 (0010)      24 (0018) \n",sizeof(ONESC),sizeof(ONESC));
	printf("sizeof(VARTREE)        =   %3ld (%04lX)      16 (0010)      32 (0020) \n",sizeof(VARTREE),sizeof(VARTREE));
	printf("sizeof(FIELD)          =   %3ld (%04lX)      12 (000C)      16 (0010) \n",sizeof(FIELD),sizeof(FIELD));
	printf("sizeof(INCLUDE)        =   %3ld (%04lX)      20 (0014)      40 (0028) \n",sizeof(INCLUDE),sizeof(INCLUDE));
	printf("sizeof(EXPR)           =   %3ld (%04lX)      16 (0010)      24 (0018) \n",sizeof(EXPR),sizeof(EXPR));
	printf("sizeof(KEYPTR)         =   %3ld (%04lX)       8 (0008)      16 (0010) \n",sizeof(KEYPTR),sizeof(KEYPTR));
	printf("sizeof(KEY_Elist)      =   %3ld (%04lX)      20 (0014)      40 (0028) \n",sizeof(KEY_Elist),sizeof(KEY_Elist));
	printf("sizeof(KEY_Elem)       =   %3ld (%04lX)      20 (0014)      32 (0020) \n",sizeof(KEY_Elem),sizeof(KEY_Elem));
	printf("sizeof(PTAB_ArrPtr)    =   %3ld (%04lX)       8 (0008)      16 (0010) \n",sizeof(PTAB_ArrPtr),sizeof(PTAB_ArrPtr));
	printf("sizeof(POS)            =   %3ld (%04lX)       8 (0008)       8 (0008) \n",sizeof(POS),sizeof(POS));
	printf("sizeof(DOMAINR)        =   %3ld (%04lX)       8 (0008)       8 (0008) \n",sizeof(DOMAINR),sizeof(DOMAINR));
	printf("sizeof(ELEMENTREC)     =   %3ld (%04lX)      76 (004C)      76 (004C) \n",sizeof(ELEMENTREC),sizeof(ELEMENTREC));
	printf("sizeof(TD0REC)         =   %3ld (%04lX)      30 (001E)      30 (001E) \n",sizeof(TD0REC),sizeof(TD0REC));
	printf("sizeof(TD1REC)         =   %3ld (%04lX)       8 (0008)       8 (0008) \n",sizeof(TD1REC),sizeof(TD1REC));
	printf("sizeof(TD2REC)         =   %3ld (%04lX)     133 (0085)     133 (0085) \n",sizeof(TD2REC),sizeof(TD2REC));
	printf("sizeof(PAGE)           =   %3ld (%04lX)      28 (001C)      40 (0028) \n",sizeof(PAGE),sizeof(PAGE));
	printf("sizeof(PAGEHDR)        =   %3ld (%04lX)      16 (0010)      28 (001C) \n",sizeof(PAGEHDR),sizeof(PAGEHDR));
	printf("sizeof(PAGEDATA)       =   %3ld (%04lX)      12 (000C)      12 (000C) \n",sizeof(PAGEDATA),sizeof(PAGEDATA));
	printf("sizeof(DBhdr)          =   %3ld (%04lX)      32 (0020)      32 (0020) \n",sizeof(DBhdr),sizeof(DBhdr));

	printf("\n\n");
}


void show_structs(void)
{
	showFLD();	
}

#endif
