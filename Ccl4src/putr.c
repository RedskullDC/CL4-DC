#ifndef PUTREC_C
#define PUTREC_C

#include "DBdefs.h"
#include "cl4.h"

#define LOCK	= 0x01;
#define NEWREC	= 0x02;
// not public in libcl4
int _putr(int TDno, char *WorkArea, POS *Pos, short a4, short *a5)
{
	TDinfo *TDptr; // esi@1
	int v7; // eax@1
	int v9; // edi@5

	//eprint("_putr(TDno: %d,WorkArea: x%08X,POS: x%08X, a4: x%02X, a5: x%08X)\n" ,TDno,WorkArea,Pos,a4,a5);
	
	TDptr = _chktd(TDno);
	v7 = check_bup_sem(TDptr->TDDBinfo);
	if ( v7 )
		derror(v7, 0, TDptr);
	
	if ( (TDptr->TDFlags & 0x0C) != 0x08 )	// & 0000 1100 != 0000 1000     Looking for bit#3 set, bit#2 clear
		derror(37, 0, TDptr);

	v9 = _update(TDptr, WorkArea, Pos, ((a4 & 2) != 0) + 1, a5);
	if ( a4 & 1 )
		_lockr(TDptr->TDDBinfo, TDptr->TDindexOff, TDptr->TDKeyDefs, TDptr->KeyDefSize, 0);
	return v9;
}

int putr(int TDno, char *WorkArea, POS *Pos)
{
  return _putr(TDno, WorkArea, Pos, 0, 0);
}

int lputr(int TDno, char *WorkArea, POS *Pos)
{
  return _putr(TDno, WorkArea, Pos, 1, 0);
}

int newr(int TDno, char *WorkArea, POS *Pos)
{
  return _putr(TDno, WorkArea, Pos, 2, 0);
}

int lnewr(int TDno, char *WorkArea, POS *Pos)
{
  return _putr(TDno, WorkArea, Pos, 3, 0);
}

int lmodr(int TDno, char *WorkArea, short *a3)	// Not used in clenter
{
  return _putr(TDno, WorkArea, 0, 1, a3);
}

int modr(int TDno, char *WorkArea, short *a3)	// Not used in clenter
{
  return _putr(TDno, WorkArea, 0, 0, a3);
}

int replace(int TDno, char *WorkArea)
{
  return _putr(TDno, WorkArea, 0, 0, 0);
}
#endif
