#ifndef GET_PT_C
#define GET_PT_C

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void moveptptrs(PTAB *a1, PTAB *a2);

short getptabp(short seq)
{
	PTAB_ArrPtr *PTarr;
	unsigned short ptidx;
	
	PTarr = &ptarr + (unsigned short)curptarr;
	assert(seq <= PTarr->NextElement);

	if ( seq == PTarr->NextElement )
	{
		ptidx = getptmem();				// should bump ptidx so it == seq+1
		assert(seq+1 == ptidx);
	}
	else
	{
		ptidx = seq + 1;
	}
	return ptidx;
}

// following not called in clenter
//	getvarname()
//	getvars
//	getvarmem
//	getenmem
//	getxtmem

short getptmem()
{
	PTAB_ArrPtr	*v0;
	PTAB		*v4;
	PTAB		*v5;
	short		v1;
	short		v2;
	short		v3;
	
	v0 = &ptarr + curptarr;
	if ( !v0->MaxAvail )
		v0->TableAddr = (PTAB *)mmalloc(0);
	v1 = v0->NextElement;
//printf("getptmem 139 v1 = %d, MaxAvail = %d\n",v1, v0->MaxAvail);
	if ( v1 == v0->MaxAvail )
	{
		//printf("getptmem 144 v1(%d) == MaxAvail(%d)\n",v1, v0->MaxAvail);
		v2 = v0->MaxAvail;
		v3 = v1 + 1500;                             // alloc in blocks of 1500
		v0->MaxAvail = v3;
		v4 = v0->TableAddr;
		v5 = (PTAB*)mrealloc(v0->TableAddr, 8 * v2, 8 * (unsigned short)v3);
		v0->TableAddr = v5;
		moveptptrs(v4, v5);
	}
	v0->NextElement++;
//printf("getptmem 154 MaxAvail = %d, NextElement = %d\n", v0->MaxAvail,v0->NextElement);
	return v0->NextElement;
}

//	getrtmem
//	getramem
//	getprmem
//	getrdmem
//	getxfmem
//	getsfmem
//	getkxmem

short getstrmem(int amt)
{
     CLStrArray *v1;
	 int		v2;
	 size_t		v3;
	 short		v4;
	 
	 assert(amt < 200);

     v1 = &strarr + curstrarr;
     if ( !v1->StrMaxSize )
          v1->StringTable = (char *)mmalloc(0);
     if ( v1->StrTableSize + amt >= v1->StrMaxSize )
     {
          v2 = v1->StrMaxSize;
          v3 = v1->StrMaxSize + 200;            // blocks of 200bytes at a time
          v1->StrMaxSize = v3;
          v1->StringTable = (char *)mrealloc(v1->StringTable, v2, v3);
     }
     v4 = v1->StrTableSize;
     v1->StrTableSize += amt;
     return v4 + 1;
}

// changed this to a macro. See DBdefs.h
//PTAB* PTARR(unsigned short PTABno)
//{
//	return PTABno ? &ptarr.TableAddr[PTABno - 1] : 0;
//}

//	Not called in clenter

//	allenpt
//	moveenptrs
//	dallenpt
//	allxtpt
//	movextptrs
//	dallxtpt
//	allptpt

void moveptptrs(PTAB *a1, PTAB *a2)
{
     PTAB	**v3;
	 signed int v2;
	 
	 v2 = ptrarrsize - 1;	// in funcdefs.h
     do
     {
          if ( ptptrs[v2] )
          {
               v3 = ptptrs[v2];
               *v3 = (PTAB *)((char *)a2 + ((char *)*v3 - (char *)a1));
          }
          --v2;
     }
     while ( v2 >= 0 );
}

//	dallptpt
//	allxfpt
//	movexfptrs
//	dallxfpt
//	allkxpt
//	movekxptrs
//	dallkxpt
//	allrtpt
//	movertptrs
//	dallrtpt
//	allprpt
//	moveprptrs
//	dallprpt
//	allrdpt
//	moverdptrs
//	dallrdpt
//	allsfpt
//	movesfptrs
//	dallsfpt

#endif
