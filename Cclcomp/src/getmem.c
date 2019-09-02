#ifndef TABLES_C
#define TABLES_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

#define PTR_MAX	30		// max size of PTR holder tables

void allptpt(unsigned short PTno, PTAB **a2)
{
	int v2 = PTR_MAX - 1;
	while ( ptptrs[v2] )
	{
		--v2;
		if ( v2 < 0 )
		{
			fwrite("allptpt: no pointer space available\n", 1u, 0x24u, stderr);
			exit(1);
		}
	}
	ptptrs[v2] = a2;
	*a2 = PTARR(PTno);
}

void dallptpt(PTAB **ptab)
{
	int v1 = PTR_MAX - 1;
	while ( ptptrs[v1] != ptab )
	{
		if ( --v1 < 0 )
			return;
	}
	ptptrs[v1] = 0;
}

void allxtpt(short XTno, XTAB **xtp)
{
	int v2 = PTR_MAX - 1;
    while ( xtptrs[v2] )
    {
		--v2;
		if ( v2 < 0 )
		{
			fwrite("allxtpt: no pointer space available\n", 1u, 0x24u, stderr);
			exit(1);
		}
    }
    xtptrs[v2] = xtp;
	*xtp = XTARR(XTno);
}

void dallxtpt(XTAB **xtp)
{
	int v1 = PTR_MAX - 1;
	while ( xtptrs[v1] != xtp )
	{
		if ( --v1 < 0 )
			return;
	}
	xtptrs[v1] = 0;
}

void movextptrs(XTAB *old, XTAB *New)
{
	XTAB **v3;
	int v2 = PTR_MAX - 1;
	do
	{
		if ( xtptrs[v2] )
		{
			v3 = xtptrs[v2];
			*v3 = (XTAB *)((char *)New + (((char *)*v3 - (char *)old)));
		}
		--v2;
	}
	while ( v2 >= 0 );
}
short getxtmem(void)
{
	XTAB_ArrPtr *v0;
	XTAB *v4;
	XTAB *v5;
	int v2;
	short v1;
	short v3;
	 
	v0 = &xtarr + (unsigned short)curxtarr;
	if ( !v0->MaxAvail )
		v0->TableAddr = (XTAB*)mmalloc(0);
	
	v1 = v0->NextElement;
	if ( v1 == v0->MaxAvail )
	{
		v2 = v0->MaxAvail;
		v3 = v1 + 50;
		v0->MaxAvail = v3;
		v4 = v0->TableAddr;
		v5 = (XTAB*)mrealloc(v0->TableAddr, sizeof(XTAB) * v2, sizeof(XTAB) * (unsigned short)v3);	// 52
		v0->TableAddr = v5;
		movextptrs(v4, v5);
	}
	return ++(v0->NextElement);
}

short getptmem()
{
	PTAB_ArrPtr *v0;
	PTAB		*v4;
	PTAB		*v5;
	short		v1;
	short		v2;
	short		v3;
	
	v0 = &ptarr + curptarr;
	if ( !v0->MaxAvail )
		v0->TableAddr = (PTAB*)mmalloc(0);

	v1 = v0->NextElement;
	if ( v1 == v0->MaxAvail )
	{
		v2 = v0->MaxAvail;
		v3 = v1 + 1500;                             // alloc in blocks of 1500
		v0->MaxAvail = v3;
		v4 = v0->TableAddr;
		v5 = (PTAB*)mrealloc(v0->TableAddr, sizeof(PTAB) * v2, sizeof(PTAB) * (unsigned short)v3);	// 8 = sizeof(PTAB)
		v0->TableAddr = v5;
		moveptptrs(v4, v5);
	}
	return ++(v0->NextElement);
}

void moveptptrs(PTAB *a1, PTAB *a2)
{
	PTAB **v3;
	int v2 = PTR_MAX - 1;
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
short getxfmem()
{
     XFTAB_ArrPtr	*v0;
	 XFTAB			*xft_old;
	 XFTAB			*xft_new;
	 int			v2;
	 short			v1;
	 short			v3;
	 
     v0 = &xfarr + (unsigned short)curxfarr;
     if ( !v0->MaxAvail )
          v0->TableAddr = (XFTAB*)mmalloc(0);
     v1 = v0->NextElement;
     if ( v1 == v0->MaxAvail )
     {
          v2 = v0->MaxAvail;
          v3 = v1 + 10;
          v0->MaxAvail = v3;
          xft_old = v0->TableAddr;
          xft_new = (XFTAB *)mrealloc(v0->TableAddr, sizeof(XFTAB) * v2, sizeof(XFTAB) * (unsigned short)v3);	// sizeof(XFTAB) = 8
          v0->TableAddr = xft_new;
          movexfptrs(xft_old, xft_new);
     }
	 return ++(v0->NextElement);
}

void allxfpt(unsigned short XFno, XFTAB **a2)
{
	int v2 = PTR_MAX - 1;
	while ( xfptrs[v2] )
	{
		--v2;
		if ( v2 < 0 )
		{
			fwrite("allxfpt: no pointer space available\n", 1u, 0x24u, stderr);
			exit(1);
		}
	}
	xfptrs[v2] = a2;
	*a2 = XFARR(XFno);
}
void dallxfpt(XFTAB **xftab)
{
	int v1 = PTR_MAX - 1;
	while ( xfptrs[v1] != xftab )
	{
		--v1;
		if ( v1 < 0 )
		   return;
	}
	xfptrs[v1] = 0;
}

void movexfptrs(XFTAB *xft_from, XFTAB *xft_to)
{
	XFTAB **v3;
	int v2 = PTR_MAX - 1;
	do
	{
		if ( xfptrs[v2] )
		{
			v3 = xfptrs[v2];
			*v3 = (XFTAB *)((char *)xft_to + ((char *)*v3 - (char *)xft_from));
		}
		--v2;
	}
	while ( v2 >= 0 );
}
short getenmem()
{
     ENTAB_ArrPtr *v0;
	 ENTAB		*v4;
	 ENTAB		*v5;
	 int		v2;
	 short		v1;
	 short		v3;
	 
     v0 = &enarr + (unsigned short)curenarr;
     if ( !v0->MaxAvail )
          v0->TableAddr = (ENTAB*)mmalloc(0);
     
	 v1 = v0->NextElement;
     if ( v1 == v0->MaxAvail )
     {
          v2 = v0->MaxAvail;
          v3 = v1 + 200;
          v0->MaxAvail = v3;
          v4 = v0->TableAddr;
          v5 = (ENTAB *)mrealloc(v0->TableAddr, sizeof(ENTAB) * v2, sizeof(ENTAB) * (unsigned short)v3);	// sizeof(ENTAB) = 12
          v0->TableAddr = v5;
          moveenptrs(v4, v5);
     }
	 return ++(v0->NextElement);
}

void allenpt(short ENno, ENTAB **a2)
{
	int v2 = PTR_MAX - 1;

	while ( enptrs[v2] )
	{
		--v2;
		if ( v2 < 0 )
		{
		   fwrite("allenpt: no pointer space available\n", 1u, 0x24u, stderr);
		   exit(1);
		}
	}
	enptrs[v2] = a2;
	*a2 = ENARR(ENno);
}

void dallenpt(ENTAB **a1)
{
	int v1 = PTR_MAX - 1;
	while ( enptrs[v1] != a1 )
	{
		--v1;
		if ( v1 < 0 )
			return;
	}
	enptrs[v1] = 0;
}
void moveenptrs(ENTAB *a1, ENTAB *a2)
{
	ENTAB **v3;
	int v2 = PTR_MAX - 1;
	do
	{
		if ( enptrs[v2] )
		{
		   v3 = enptrs[v2];
		   *v3 = (ENTAB *)((char *)a2 + ((char *)*v3 - (char *)a1));
		}
		--v2;
	}
	while ( v2 >= 0 );
}

void allrdpt(unsigned short RDno, RDTAB **a2)
{
	int v2 = PTR_MAX - 1;
	while ( rdptrs[v2] )
	{
		--v2;
		if ( v2 < 0 )
		{
		   fwrite("allrdpt: no pointer space available\n", 1u, 0x24u, stderr);
		   exit(1);
		}
	}
	rdptrs[v2] = a2;
	*a2 = RDARR(RDno);
}

void dallrdpt(RDTAB **rdtb)
{
	int v1 = PTR_MAX - 1;
	while ( rdptrs[v1] != rdtb )
	{
		--v1;
		if ( v1 < 0 )
			return;
	}
	rdptrs[v1] = 0;
}

void moverdptrs(RDTAB *a1, RDTAB *a2)
{
	RDTAB **v3;
	int v2 = PTR_MAX - 1;
	do
	{
		if ( rdptrs[v2] )
		{
			v3 = rdptrs[v2];
			*v3 = (RDTAB *)((char *)a2 + ((char *)*v3 - (char *)a1));
		}
		--v2;
	}
	while ( v2 >= 0 );
}
short getrtmem(void)
{
     RTAB_ArrPtr	*v0;
	 RTAB			*v4;
	 RTAB			*v5;
	 int			v2;
	 short			v1;
	 short			v3;
	 
     v0 = &rtarr + (unsigned short)currtarr;
     if ( !v0->MaxAvail )
          v0->TableAddr = (RTAB*)mmalloc(0);
     v1 = v0->NextElement;
     if ( v1 == v0->MaxAvail )
     {
          v2 = v0->MaxAvail;
          v3 = v1 + 50;                         // alloc in blocks of 50
          v0->MaxAvail = v3;
          v4 = v0->TableAddr;
          v5 = (RTAB *)mrealloc(v0->TableAddr, sizeof(RTAB) * v2, sizeof(RTAB) * (unsigned short)v3);	// sizeof(RTAB) = 20
          v0->TableAddr = v5;
          movertptrs(v4, v5);
     }
	 return ++(v0->NextElement);
}

void dallrtpt(RTAB **a1)
{
	int v1 = PTR_MAX - 1;
	while ( rtptrs[v1] != a1 )
	{
		--v1;
		if ( v1 < 0 )
			return;
	}
	rtptrs[v1] = 0;
}

void allrtpt(unsigned short RTno, RTAB **a2)
{
	int v2 = PTR_MAX - 1;
	while ( rtptrs[v2] )
	{
		--v2;
		if ( v2 < 0 )
		{
			fwrite("allrtpt: no pointer space available\n", 1u, 0x24u, stderr);
			exit(1);
		}
	}
	rtptrs[v2] = a2;
	*a2 = RTARR(RTno);
}

void movertptrs(RTAB *Old, RTAB *New)
{
	RTAB **v3;
	int v2 = PTR_MAX - 1;
	do
	{
		if ( rtptrs[v2] )
		{
			v3 = rtptrs[v2];
			*v3 = (RTAB *)((char *)New + ((char *)*v3 - (char *)Old));
		}
		--v2;
	}
	while ( v2 >= 0 );
}

void moveprptrs(PRTAB *a1, PRTAB *a2)
{
	PRTAB **v3; // edx@3
	int v2 = PTR_MAX - 1;
	do
	{
		if ( prptrs[v2] )
		{
			v3 = prptrs[v2];
			*v3 = (PRTAB *)((char *)a2 + ((char *)*v3 - (char *)a1));
		}
		--v2;
	}
	while ( v2 >= 0 );
}

short getprmem()
{
     PRTAB_ArrPtr *v0;
     PRTAB	*v4;
	 PRTAB	*v5;
	 int	v2;
	 short	v1;
	 short	v3;

    v0 = &prarr + (unsigned short)curprarr;
	//eprint("getprmem() - v0->MaxAvail = %d , v0->NextElement = %d\n",v0->MaxAvail,v0->NextElement);

    if ( !v0->MaxAvail )
		v0->TableAddr = (PRTAB*)mmalloc(0);
    v1 = v0->NextElement;
    if ( v1 == v0->MaxAvail )
    {
		v2 = v0->MaxAvail;
        v3 = v1 + 50;
        v0->MaxAvail = v3;
		//eprint("getprmem(realloc) - v0->MaxAvail = %d \n",v0->MaxAvail);
        v4 = v0->TableAddr;
        v5 = (PRTAB *)mrealloc(v0->TableAddr, sizeof(PRTAB) * v2, sizeof(PRTAB) * (unsigned short)v3);	// sizeof(PRTAB) = 10
		//eprint("getprmem(realloc) - v5 = x%08X\n",v5);
        v0->TableAddr = v5;
        moveprptrs(v4, v5);
	}
	return ++(v0->NextElement);
}

void allprpt(unsigned short PRno, PRTAB **a2)
{
 	int v2 = PTR_MAX - 1;
	while ( prptrs[v2] )
	{
		--v2;
		if ( v2 < 0 )
		{
			fwrite("allprpt: no pointer space available\n", 1u, 0x24u, stderr);
			exit(1);
		}
	}
	prptrs[v2] = a2;
	*a2 = PRARR(PRno);
}

void dallprpt(PRTAB **a1)
{
 	int v1 = PTR_MAX - 1;
	while ( prptrs[v1] != a1 )
	{
		--v1;
		if ( v1 < 0 )
			return;
	}
	prptrs[v1] = 0;
}

short getsfmem()
{
     SFTAB_ArrPtr *v0;
	 SFTAB	*v4;
	 SFTAB	*v5;
	 int	v2;
	 short	v1;
	 short	v3;
	 
     v0 = &sfarr + cursfarr;
     if ( !v0->MaxAvail )
          v0->TableAddr = (SFTAB*)mmalloc(0);
     
	 v1 = v0->NextElement;
     if ( v1 == v0->MaxAvail )
     {
          v2 = v0->MaxAvail;
          v3 = v1 + 10;
          v0->MaxAvail = v3;
          v4 = v0->TableAddr;
          v5 = (SFTAB *)mrealloc(v0->TableAddr, sizeof(SFTAB) * v2, sizeof(SFTAB) * (unsigned short)v3);	// sizeof(SFTAB) = 8
          v0->TableAddr = v5;
          movesfptrs(v4, v5);
     }
	 return ++(v0->NextElement);
}

void allsfpt(unsigned short SFno, SFTAB **a2)
{
	int v2 = PTR_MAX - 1;
	while ( sfptrs[v2] )
	{
		--v2;
		if ( v2 < 0 )
		{
			fwrite("allsfpt: no pointer space available\n", 1u, 0x24u, stderr);
			exit(1);
		}
	}
	sfptrs[v2] = a2;
	*a2 = SFARR(SFno);
}

void dallsfpt(SFTAB **a1)
{
 	int v1 = PTR_MAX - 1;
	while ( sfptrs[v1] != a1 )
	{
		--v1;
		if ( v1 < 0 )
			return;
	}
	sfptrs[v1] = 0;
}

void movesfptrs(SFTAB *a1, SFTAB *a2)
{
	SFTAB **v3;
	int v2 = PTR_MAX - 1;
	do
	{
		if ( sfptrs[v2] )
		{
			v3 = sfptrs[v2];
			*v3 = (SFTAB *)((char *)a2 + ((char *)*v3 - (char *)a1));
		}
		--v2;
	}
	while ( v2 >= 0 );
}
short getramem(void)
{
     RATAB_ArrPtr *v0;
	 int	v2;
	 short	v1;
	 short	v3;

     v0 = &raarr + curraarr;
     if ( !v0->MaxAvail )
          v0->TableAddr = (RATAB *)mmalloc(0);
     v1 = v0->NextElement;
     if ( v1 == v0->MaxAvail )
     {
          v2 = v0->MaxAvail;
          v3 = v1 + 10;
          v0->MaxAvail = v3;
          v0->TableAddr = (RATAB *)mrealloc(v0->TableAddr, sizeof(RATAB) * v2, sizeof(RATAB) * (unsigned short)v3);	// sizeof(RATAB) = 12
     }
	 return ++(v0->NextElement);
}

short getkxmem(void)
{
     KXTAB_ArrPtr *v0;
	 KXTAB		*v4;
	 KXTAB		*v5;
	 int		v2;
	 short		v1;
	 short		v3;
	 
     v0 = &kxarr + curkxarr;
     if ( !v0->MaxAvail )
          v0->TableAddr = (KXTAB*)mmalloc(0);
     
	 v1 = v0->NextElement;
     if ( v1 == v0->MaxAvail )
     {
          v2 = v0->MaxAvail;
          v3 = v1 + 10;
          v0->MaxAvail = v3;
          v4 = v0->TableAddr;
          v5 = (KXTAB *)mrealloc(v0->TableAddr, sizeof(KXTAB) * v2, sizeof(KXTAB) * (unsigned short)v3);	//sizeof(KXTAB) = 64
          v0->TableAddr = v5;
          movekxptrs(v4, v5);
     }
	 return ++(v0->NextElement);
}

void dallkxpt(KXTAB **kxtab)
{
	int v1 = PTR_MAX - 1;
	while ( kxptrs[v1] != kxtab )
	{
		--v1;
		if ( v1 < 0 )
			return;
	}
	kxptrs[v1] = 0;
}

void allkxpt(unsigned short KXno, KXTAB **a2)
{
	int v2 = PTR_MAX - 1;
	while ( kxptrs[v2] )
	{
		--v2;
		if ( v2 < 0 )
		{
		   fwrite("allkxpt: no pointer space available\n", 1u, 0x24u, stderr);
		   exit(1);
		}
	}
	kxptrs[v2] = a2;
	*a2 = KXARR(KXno);
}

void movekxptrs(KXTAB *a1, KXTAB *a2)
{
     KXTAB **v3;
	 int v2 = PTR_MAX - 1;
	 do
     {
          if ( kxptrs[v2] )
          {
               v3 = kxptrs[v2];
               *v3 = (KXTAB *)((char *)a2 + ((char *)*v3 - (char *)a1));
          }
          --v2;
     }
     while ( v2 >= 0 );
}

short getptabp(short seq)
{
	PTAB_ArrPtr *PTarr;
	unsigned short ptidx;
	
	PTarr = &ptarr + (unsigned short)curptarr;
	
	//if ( seq > PTarr->NextElement )
	//	__assert_fail("seq <= pt->nextel", "getpt.c", 35, "getptabp");
	assert(seq <= PTarr->NextElement);

	if ( seq == PTarr->NextElement )
	{
		ptidx = getptmem();
		//if ( seq + 1 != ptidx )
		//	__assert_fail("seq+1 == ptidx", "getpt.c", 45, "getptabp");
		assert(seq + 1 == ptidx);
	}
	else
	{
		ptidx = seq + 1;
	}
	return ptidx;
}

//  Now a MACRO. see DBdefs.h
inline PTAB* PTARR(unsigned short PTABno)
{
	return PTABno ? &ptarr.TableAddr[PTABno - 1] : 0;
}

short getrdmem()
{
     RDTAB_ArrPtr *v0;
	 RDTAB		*v4;
	 RDTAB		*v5;
     int		v2;
	 short		v1;
	 short		v3;
	 
     v0 = &rdarr + currdarr;
     if ( !v0->MaxAvail )
          v0->TableAddr = (RDTAB*)mmalloc(0);
     
	 v1 = v0->NextElement;
     if ( v1 == v0->MaxAvail )
     {
          v2 = v0->MaxAvail;
          v3 = v1 + 50;
          v0->MaxAvail = v3;
          v4 = v0->TableAddr;
          v5 = (RDTAB *)mrealloc(v0->TableAddr, sizeof(RDTAB) * v2, sizeof(RDTAB) * (unsigned short)v3);	// 12 = sizeof(RDTAB)
          v0->TableAddr = v5;
          moverdptrs(v4, v5);
     }
	 return ++(v0->NextElement);
}

short getstrmem(int amt)
{
     CLStrArray *v1;
	 int	Old;
	 size_t New;
	 short	v4;
	 
     //if ( amt > 199 )
     //     __assert_fail("amt < 200", "getmem.c", 0x203u, "getstrmem");

	 assert(amt < 200);			// why only 200 at a time, when max string var can be 1500?

     v1 = &strarr + curstrarr;
     
	 if ( !v1->StrMaxSize )
          v1->StringTable = (char *)mmalloc(0);
     
	 if ( v1->StrTableSize + amt >= v1->StrMaxSize )
     {
          Old = v1->StrMaxSize;
          New = v1->StrMaxSize + 200;						// blocks of 200bytes at a time
          v1->StrMaxSize = New;
          v1->StringTable = (char *)mrealloc(v1->StringTable, Old, New);
     }
     v4 = v1->StrTableSize;
     v1->StrTableSize += amt;
     return v4 + 1;
}


#endif
