#ifndef DUMP_ROUTINES_C
#define DUMP_ROUTINES_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

void putbuf(const void *ptr, size_t arg);
void putcount(short a1);
void putucount(unsigned short a1);
void puttext(char *ptr, size_t len);
void dumpoe(ONESC *ptr);
void putdouble(double a1, short TDFtype);
void putbits(unsigned short a1);
void putfloat(float a1, short TDFtype);
void putLong(long ptr);

void putptabs(PTAB_ArrPtr *a1);
void putrdtabs(RDTAB_ArrPtr *a1);
void putxtabs(XTAB_ArrPtr *a1);
void putenodes(ENTAB_ArrPtr *a1);
void putrtabs(RTAB_ArrPtr *a1);
void putprtabs(PRTAB_ArrPtr *a1);
void putxftabs(XFTAB_ArrPtr *a1);
void putsftabs(SFTAB_ArrPtr *a1);
void putratabs(RATAB_ArrPtr *a1);
void putkxtabs(KXTAB_ArrPtr *a1);
void putftabs(FLDdesc *ptr, int Count);

void puttable(int ptr);
void putarrcount(short *ptr, short a2);
void putarrtds(int *ptr, int a2);
void putbtabs(short count);
void dumpscr(SCR *ptr);
void putbool(int ptr);

void dumpscr(SCR *t24)
{
	while (t24)
	{
		if ( tpenc )
		{
			putcount(t24->PT_start);
			putcount(t24->PT_end);
			putcount(t24->NextSCR != 0);     // bool to indicate further T24's follow
		}
		else
			putbuf(t24, sizeof(SCR));

		t24 = t24->NextSCR;
	}
}

void putbtabs(short count)
{
    BTAB *btb;
	signed int i;
	
    putcount(count);
    if ( tpenc )
	{
        btb = btab;
        for ( i = 0; i < count; btb++ )
		{
            putbool(btb->BTFlags);				// never set in clcomp 
            putcount(btb->EndLine);
            putcount(btb->fmts);
            putcount(btb->StartLine);
            putcount(btb->TTno);
            putcount(btb->Scrs);
            putcount(btb->EXPR != 0);			// never set in clcomp
            putcount(btb->On_delete != 0);
            putcount(btb->On_exit != 0);
            putcount(btb->On_excep != 0);
            putcount(btb->On_entry != 0);
            putbuf(btb->Maintain, 7u);
            putbuf(btb->BlockName, 21u);
			i++;
		}
    }
    else
    {
        putbuf(btab, sizeof(BTAB) * count);			// 72
		#ifdef DEBUG_DUMP
		eprint("BTABS - [%d] StructSize = %d\n",count, sizeof(BTAB));
		DumpBlock((char*)btab, sizeof(BTAB) * count, sizeof(BTAB));
		#endif
	}
}

void putarrtds(int *ptr, int a2)
{
     int *v2;
	 int i;
	 
     if ( tpenc )
     {
          v2 = ptr;
          for ( i = 0; i < a2; v2++ )
          {
               puttable(*v2);
               i++;
          }
     }
     else
     {
          putbuf(ptr, 4 * a2);
     }
}

void putarrcount(short *ptr, short a2)
{
     short	*v2;
	 int	i;
	 
     if ( tpenc )
     {
          v2 = ptr;
          for ( i = 0; i < a2; v2++ )
          {
               putcount(*v2);
               i++;
          }
     }
     else
     {
          putbuf(ptr, 2 * a2);
     }
}

void putkxtabs(KXTAB_ArrPtr *a1)
{
	KXTAB *kxtb;
	int i;
	int count;
	
	count = a1->NextElement;
    putucount(count);
    if ( tpenc )
    {
		kxtb = a1->TableAddr;
        for ( i = 0; i < count; ++kxtb )
        {
			putbits(kxtb->KXFlags);
            putucount(kxtb->WhereEXP);
            putcount(kxtb->NumTTs);
            putcount(kxtb->NumKeyFields);
            putcount(kxtb->NumDataFields);
            putcount(kxtb->KeySize);
            putcount(kxtb->RecSize);
            puttable(kxtb->TTno);
            puttext(kxtb->TableName, 0);
            ++i;
        }
	}
    else
    {
		//putbuf(a1->TableAddr, count << 6); count * sizeof(KXTAB) == 64
		putbuf(a1->TableAddr, count * sizeof(KXTAB)); //  == 64
    }
}
void putrdtabs(RDTAB_ArrPtr *a1)
{
     RDTAB *rdtb; // esi@2
     int i; // edi@2
     int count; // [sp+8h] [bp-10h]@1

     count = a1->NextElement;
     putucount(count);
     if ( tpenc )
     {
          rdtb = a1->TableAddr;
          for ( i = 0; i < count; rdtb++ )
          {
               putcount(rdtb->RDTFlags);
               putucount(rdtb->ENTno);
               puttable(rdtb->TTno);
               putucount(rdtb->NextRD);
               i++;
          }
     }
     else
     {
          putbuf(a1->TableAddr, sizeof(RDTAB) * count);		// 12
     }
}
void putprtabs(PRTAB_ArrPtr *a1)
{
	PRTAB	*prtb;
	int	i;
	int	count;

	count = a1->NextElement;
	putucount(count);
	if ( tpenc )
	{
		prtb = a1->TableAddr;
		for ( i = 0; i < count; ++prtb )
		{
			putucount(prtb->VAR_ENTno);
			putucount(prtb->WIDTH_ENTno);
			putucount(prtb->WIDTH_FLAGS);
			putucount(prtb->ReservedFlags);
			putucount(prtb->NextPR);
			i++;
		}
	}
	else
	{
		putbuf(a1->TableAddr, sizeof(PRTAB) * count);		// 10
		#ifdef DEBUG_DUMP
		eprint("PRTABS - [%d] StructSize = %d\n",count, sizeof(PRTAB));
		DumpBlock((char*)a1->TableAddr, sizeof(PRTAB) * count, sizeof(PRTAB));
		#endif
	}
}

void putsftabs(SFTAB_ArrPtr *a1)
{
    SFTAB	*sftb;
	int		j;
	int		count;
	
    count = a1->NextElement;
    putucount(count);
    if ( tpenc )
	{
        sftb = a1->TableAddr;
        for ( j = 0; j < count; sftb++ )
		{
            putbits(sftb->cmd_flags);
            putucount(sftb->PRTno);
            putucount(sftb->AliasEnt);
            putucount(sftb->FileNameEnt);
			j++;
		}
    }
    else
	{
        putbuf(a1->TableAddr, sizeof(SFTAB) * count);			//  8
		#ifdef DEBUG_DUMP
		eprint("SFTABS - [%d] StructSize = %d\n",count, sizeof(SFTAB));
		DumpBlock((char*)a1->TableAddr, sizeof(SFTAB) * count, sizeof(SFTAB));
		#endif

	}
}

// transfer table records : copy x from y
void putxftabs(XFTAB_ArrPtr *a1)
{
	XFTAB *xftb;
	int i;
	int count;

	count = a1->NextElement;
	putucount(count);
	if ( tpenc )
	{
		xftb = a1->TableAddr;
		for ( i = 0; i < count; xftb++ )
		{
			puttable(xftb->TTno_to);
			puttable(xftb->TTno_from);
			i++;
		}
	}
	else
	{
		putbuf(a1->TableAddr, sizeof(XFTAB) * count);		// 8
		#ifdef DEBUG_DUMP
		eprint("XFTABS - [%d] StructSize = %d\n",count, sizeof(XFTAB));
		DumpBlock((char*)a1->TableAddr, sizeof(XFTAB) * count, sizeof(XFTAB));
		#endif
	}
}

// range records
void putratabs(RATAB_ArrPtr *a1)
{
	RATAB *ratb;
	int i;
	int count;

	count = a1->NextElement;
	putucount(a1->NextElement);
	if ( tpenc )
	{
		ratb = a1->TableAddr;
		for ( i = 0; i < count; ratb++ )
		{
			putcount(ratb->RangeType);
			putucount(ratb->RangeFrom);
			putucount(ratb->RangeTo);
			putucount(ratb->StringOffset);
			i++;
		}
	}
	else
	{
		putbuf(a1->TableAddr, sizeof(RATAB) * count);		// 12
		#ifdef DEBUG_DUMP
		eprint("RATABS - [%d] StructSize = %d\n",count, sizeof(RATAB));
		DumpBlock((char*)a1->TableAddr, sizeof(RATAB) * count, sizeof(RATAB));
		#endif
	}
}

void putrtabs(RTAB_ArrPtr *a1)
{
	RTAB	*rt;
	int	count;
	int	i;

	count = a1->NextElement;
	putucount(a1->NextElement);
	if ( tpenc )
	{
		rt = a1->TableAddr;
		for ( i = 0; i < count; rt++ )
		{
			putbits(rt->LockType);
			putbits(rt->OpType);
			putucount(rt->WhereEXP);
			puttable(rt->TTno);
			putucount(rt->enleft);
			putucount(rt->enright);
			putucount(rt->NextRT);
			i++;
		}
	}
	else
	{
		putbuf(a1->TableAddr, sizeof(RTAB) * count);		// 20
		#ifdef DEBUG_DUMP
		eprint("RTABS  - [%d] StructSize = %d\n",count, sizeof(RTAB));
		DumpBlock((char*)a1->TableAddr, sizeof(RTAB) * count, sizeof(RTAB));
		#endif
	}
}

void putxtabs(XTAB_ArrPtr *a1)
{
    int count;
	XTAB *xt;
	int i;
	
    count = a1->NextElement;
    putucount(count);
	xt = a1->TableAddr;
    if ( tpenc )
    {
          for ( i = 0; i < count; xt++ )
          {
               putbits(xt->Flags);
               putbits(xt->Attr);
               putbits(xt->ScrAttrib);
               putbits(xt->field_32);
               putcount(xt->FLDnum);
               putcount(xt->Width);
               putcount(xt->width);
               putcount(xt->PCol);
               putcount(xt->PLine);
               putcount(xt->ACol);
               putcount(xt->ALine);
               putcount(xt->C_X);
               putcount(xt->C_Y);
               putcount(xt->onesc != 0);        // bool
               putucount(xt->RangeID);
               putucount(xt->VarExpNo);
               putucount(xt->WidthEXP);
               putucount(xt->PCol_exp);
               putucount(xt->PLine_exp);
               putucount(xt->ACol_exp);
               putucount(xt->ALine_exp);
               putucount(xt->Prompt_exp);
               i++;
          }
    }
    else
    {
		putbuf(xt, sizeof(XTAB) * count);		// 52
		#ifdef DEBUG_DUMP
		eprint("XTABS  - [%d] StructSize = %d\n",count, sizeof(XTAB));
		DumpBlock((char*)xt, sizeof(XTAB) * count, sizeof(XTAB));
		#endif
    }
}

void putenodes(ENTAB_ArrPtr *a1)
{
	ENTAB	*entab;
	int		count;
	int		i;
	
	count = a1->NextElement;
	putucount(count);
	entab = a1->TableAddr;
	if ( tpenc )
	{
		for ( i = 0; i < count; entab++ )
		{
			putucount(entab->entype);		
			if (extendDC)						// break compatiblity with real ENC file here!!
				putucount(entab->extOpCode);	// unused in real CL4 - used for extended opcodes in DC version
			switch ( entab->entype )
			{
				case 1:										// expression describing variable/table
					putcount(entab->Enun.Enref.TTno);
					putcount(entab->Enun.Enref.VarNum);
					break;
				case 2:										// expression describing function/assignment
					putcount(entab->Enun.Enop.Enoper);
					putcount(entab->Enun.Enop.RecNo);
					break;
				case 4:
					putfloat(entab->Enun.float4, 0x104u);	// inline float, *real* clcomp doesn't create these
					break;
				case 8:
					putLong(entab->Enun.long8);				// long integer literal		** 8 bytes on X64, 4 on X86  **
					break;
				case 16:									// short string literal : 3 chars or less + \0
					putbuf(entab->Enun.char16, 4u);			// ** could be 7 bytes or less on X64 **
					break;
			}
			putucount(entab->enleft);
			putucount(entab->enright);
			i++;
		}
	}
	else
	{
		putbuf(entab, sizeof(ENTAB) * count);	// 12
		#ifdef DEBUG_DUMP
		eprint("ENTABS - [%d] StructSize = %d\n",count, sizeof(ENTAB));
		DumpBlock((char*)entab, sizeof(ENTAB) * count, sizeof(ENTAB));		// 12
		#endif

	}
}

void putptabs(PTAB_ArrPtr *a1)
{
	PTAB	*ptb;
	int		i;
	int		count;
	
	count = a1->NextElement;
	putucount(count);
	ptb = a1->TableAddr;
	if ( tpenc )
	{
		for ( i = 0; i < count; ptb++ )
		{
			putucount(ptb->SrcLineNo);
			putcount(ptb->OpCode);
			putucount(ptb->Operand);
			putucount(ptb->TABno);
			i++;
		}
	}
	else
	{
		putbuf(ptb, sizeof(PTAB) * count);		// 8
		#ifdef DEBUG_DUMP
		eprint("PTABS  - [%d] StructSize = %d\n",count, sizeof(PTAB));
		DumpBlock((char*)ptb, sizeof(PTAB) * count, sizeof(PTAB));
		#endif
	}
}

void putftabs(FLDdesc *fld, int count)
{
    int i;
	
    if ( tpenc )
	{
        for ( i = 0; i < count; fld++ )
		{
            putbits(fld->TDFtype);
            putbits(fld->FLDstat);
            putcount(fld->FLDelemID);
            putcount(fld->FLDlen);
            putcount(fld->FLDtype);
            putcount(fld->field_15);		// not currently used by anything....
			i++;
		}
    }
    else
    {
		putbuf(fld, sizeof(FLDdesc) * count);				// 24
		#ifdef DEBUG_DUMP
		eprint("Fields - [%d] StructSize = %d\n",count, sizeof(FLDdesc));
		DumpBlock((char*)fld, sizeof(FLDdesc) * count, sizeof(FLDdesc));		// 24
		#endif
	}
}

void dumpoe(ONESC *esc)
{
	while (esc)
	{
		if ( tpenc )
		{
			putcount(esc->OESCFlags);
			putcount(esc->BlockNo);
			putcount(esc->KeyNumber);
			putcount(esc->NextESC != 0);   // bool to indicate more OnEsc's follow
		}
		else
			putbuf(esc, sizeof(ONESC));		// 16

		esc = esc->NextESC;
	}
}

// =============================================================
// Low level put routines follow

void putbuf(const void *ptr, size_t len)
{
     size_t actual;
	 
     *__errno_location() = 0;
     actual = fwrite(ptr, 1u, len, dp);
     if ( actual != len )
     {
          syserror("dumpenc: fwrite tried %u, did %u", len, actual);
          mexit(1);
     }
}

void putbool(int ptr)	// only called from putbtabs(). Expects 4 byte value, *NOT* a 1 byte boolean!!
{
	if ( tpenc )
		xtodom((char *)&ptr, sizeof(int), 0x44u, (char *)&ptr);	// allow signed value
	putbuf(&ptr, sizeof(int));
}

void puttable(int ptr)
{
     if ( tpenc )
          xtodom((char *)&ptr, sizeof(int), 0x44u, (char *)&ptr);		// allowed signed value

	 putbuf(&ptr, sizeof(int));
}

void putdouble(double ptr, short TDFtype)
{
	if ( tpenc )
		xtodom((char *)&ptr, sizeof(double), TDFtype, (char *)&ptr);
	putbuf(&ptr, sizeof(double));
}

void putbits(unsigned short ptr)
{
    if ( tpenc )
		xtodom((char *)&ptr, sizeof(unsigned short), 0x40u, (char *)&ptr);		// unsigned allowed
	putbuf(&ptr, sizeof(unsigned short));
}

void putLong(long ptr)
{
	if ( tpenc )
		xtodom((char *)&ptr, sizeof(long), 0x44u, (char *)&ptr);	// signed value    ** 4 bytes on X86, 8 bytes on X64 **
	putbuf(&ptr, sizeof(long));
}

void putfloat(float Src, short TDFtype)
{
	int ptr;
	if ( tpenc )
		xtodom((char *)&ptr, sizeof(float), TDFtype, (char *)&Src);
	putbuf(&ptr, sizeof(float));									// float stored in 4 bytes
}

void puttext(char *ptr, size_t len)
{
	if ( !len )						// If no len specified, calc the zero-terminated length
	{
		len = strlen(ptr) + 1;
		putcount(len);				// put two bytes of length
	}
	putbuf(ptr, len);
}

void putcount(short ptr)
{
	if ( tpenc )
		xtodom((char *)&ptr, sizeof(short), 0x44u, (char *)&ptr);
	putbuf(&ptr, sizeof(short));
}

void putucount(unsigned short ptr)
{
	if ( tpenc )
		xtodom((char *)&ptr, sizeof(unsigned short), 0x40u, (char *)&ptr);
	putbuf(&ptr, sizeof(unsigned short));
}


#endif
