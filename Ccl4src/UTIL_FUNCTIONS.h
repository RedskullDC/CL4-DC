#ifndef UTILFNS_H
#define UTILFNS_H

#include "DBdefs.h"
#include "cl4.h"

#define DUMP_BYTES 24		// Number of bytes that DumpBlock shows on a line (16 or 32)
void DumpBlock(char* Buffer, int NumBytes,int StructSize);
void Dump1FLD(FLDdesc *FLD);		// Just dump 1 Field Descriptor
void DumpTTFLD(FLDdesc *FLD);
void DumpTTptr(TDesc* TT, bool ShowFields);
void DumpTDptr(TDinfo* TD,bool ShowTDef);
void DumpTDefptr(TDef* TDef);
void DumpEXPR(EXPR* Expr);
void DumpJUNC(EXPR* Expr);
void DumpIndexPage(char* Buffer);
void DumpPOS(POS* Pos);
void DumpDBptr(DBinfo* DB);
void DumpPageTable(HDRTABLE* Table);
void DumpPTAB(PTAB* ptb);

#include "UTIL_FUNCTIONS.c"

#endif
