#ifndef UTILFNS_C
#define UTILFNS_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

//------------------------------------------
// Util functions:

void DumpPOS(POS* Pos)
{
	POS*	v1;
	int		posno; 

	if (Pos)	// If Pos == \0, most routines rely on TDefPtr records.
	{
		v1 = Pos;
		posno = 0;
		while (v1->ElementID)
		{
			printf("POS[%d] : ElemID = %04X (%4d), BuffOffset = %04X (%4d) \n",posno, v1->ElementID, v1->ElementID, v1->BuffOffset, v1->BuffOffset);
			v1++;
			posno++;
		}
	}	
}
void DumpPageTable(HDRTABLE* Table)
{
	if (Table)	// If Pos == \0, most routines rely on TDefPtr records.
	{
		printf("PageNo   = x%04X (%4d)\n",mstol(&Table->PageNo),mstol(&Table->PageNo));
		printf("Depth    = x%04X (%4d)\n",MSHORT(Table->Depth),MSHORT(Table->Depth));

		DumpBlock((char*)Table,6,6);

	}	
}

void DumpPTAB(PTAB* ptb)
{
	printf("ptb->SrcLineNo	= x%04X  (%4d)\n",ptb->SrcLineNo,ptb->SrcLineNo);
	printf("ptb->OpCode     = x%04X  (%4d)\n",ptb->OpCode,ptb->OpCode);
	printf("ptb->Operand	= x%04X  (%4d)\n",ptb->Operand,ptb->Operand);
	printf("ptb->TABno      = x%04X  (%4d)\n",ptb->TABno,ptb->TABno);

}

void DumpDataPage(char* Buffer)
{
	PAGE	*Page;
	unsigned short *Offset;
	int count;
	unsigned short TabOffset;
	unsigned short NextOffset;
	int Datalen;
	unsigned char PageType;
	char	*FixedData;
	
	Page = (PAGE*) Buffer;
	PageType = Page->pgData.PageType; 
	printf("PageType    = x%02X\n",PageType);
	printf("NumEntries  = x%04X (%3d)\n",Page->header.NumEntries,Page->header.NumEntries);
	//printf("DataStart  = x%08X \n",Page->DataStart); // Not used for Data Pages
	printf("TabStart    = x%p (TableStart)\n",Page->header.tsrs.TabStart);
	printf("TabEnd      = x%p\n",Page->header.TabEnd);

	DumpBlock(Buffer, 64, 0);
	if ((Page->pgData.PageType & 0xC0) == 0x40) //Normal data, variable record size (contains strings).
	{
		Offset = (unsigned short*) Page->header.tsrs.TabStart - 1;
		for (count = 0;count <= Page->header.NumEntries ; count++,Offset++)
		{
			TabOffset = MSHORT(*Offset);
			if (count == 0)	// First Data entry has no table offset entry
			{
				TabOffset = 0;
			}
			Datalen = MSHORT(*(Offset+1)) - TabOffset;
			printf("(%3d) = %04X (%4d) = x%p\n",count,TabOffset,TabOffset,(Page->header.TabEnd + TabOffset));
			if (count < Page->header.NumEntries)
			{	// Last Table offset points past end of Data!
				DumpBlock((char*)(Page->header.TabEnd + TabOffset), Datalen, 0);
			}
		}
	}
	
	if ((Page->pgData.PageType & 0xC0) == 0x00) // Normal data. *Fixed* rec size (contains *NO* strings).
	{
		FixedData = Page->header.TabEnd;
		Datalen = (int) Page->header.tsrs.RecSize; // RecSize in this case
		for (count = 0;count <= Page->header.NumEntries ; count++,FixedData += Datalen)
		{
			printf("(%3d) = ",count);
			DumpBlock(FixedData, Datalen,0);
		}
	}
}

void DumpIndexPage(char* Buffer)
{
	PAGE	*Page;
	unsigned short *Offset;
	int count;
	unsigned short TabOffset;
	unsigned short NextOffset;
	unsigned short Datalen;
	unsigned char PageType;

	
	Page = (PAGE*) Buffer;
	PageType = Page->pgData.PageType; 
	printf("PageType   = x%X\n",PageType);
	printf("NumEntries = x%04X (%3d)\n",Page->header.NumEntries+1,Page->header.NumEntries+1);
	printf("DataStart  = x%p \n",Page->header.DataStart);
	printf("TabStart   = x%p (ptr or KeySize)\n",Page->header.tsrs.TabStart);
	printf("TabEnd     = x%p\n",Page->header.TabEnd);

	DumpBlock(Buffer, 128,0);

	if ((Page->pgData.PageType & 0xC0) == 0x40)
	{
		Offset = (unsigned short*)Page->header.tsrs.TabStart - 1;
		for (count = 0;count <= Page->header.NumEntries ; count++,Offset++)
		{
			TabOffset = MSHORT(*Offset);
			if (count == 0)	// First Data entry has no table offset entry
			{
				TabOffset = 0;
			}
			Datalen = MSHORT(*(Offset+1)) - TabOffset;
			printf("(%3d) = %04X (%4d) = x%p\n",count,TabOffset,TabOffset,(Page->header.TabEnd + TabOffset));
			if (count < Page->header.NumEntries)
			{	// Last Table offset points past end of Data!
				DumpBlock((char*)(Page->header.TabEnd + TabOffset), Datalen, 0);
			}
		}
	}

}

void DumpBlock(char* Buffer, int NumBytes, int StructSize)
{
	int temp1, temp2, remaining;
	temp1 = 0;
	char lineAsc[65];	// ony use 17 or 33 normally. This allows for 64 bytes max 

	if (!StructSize)
		StructSize = DUMP_BYTES;					// bytes to show on a line 16 or 32 - see UTIL_FUNCTIONS.h

	while (temp1 < NumBytes)
	{
		remaining = StructSize;
		printf("%p = ",(void*)Buffer);
		temp2 = 0;
		while (remaining > 0 && temp1 < NumBytes)
		{
			// build display line
			unsigned char Current = *Buffer++;
			printf("%02X ",Current);				// current byte as 2 HEX digits
			if (Current >= ' ' && Current <= '~')	// 0x20 to 0x7E
				lineAsc[temp2] = (char)Current;		// displayable
			else
				lineAsc[temp2] = '.';				// **undisplayable** char 

			temp2++;
			temp1++;
			remaining--;
			if (!(temp2 % DUMP_BYTES) && remaining > 0)				// maximum num bytes on a line (16/32)
			{
				lineAsc[temp2] = '\0';				// display buffer and rest for next line
				printf(" %s\n",lineAsc);
				temp2 = 0;
				printf("x%p = ",Buffer);		// address for next line
				continue;
			}
		}
		lineAsc[temp2] = '\0';
		printf(" %s\n",lineAsc);
	}
}
char* DumpShortBits(short Input, char Output[20])
{
	// Display bit field values of a 16bit value 
	int count;
	int temp1;
	for(count = 0,temp1 = 15;count < 19 ; temp1-- ,count++)
	{
		if ((Input >> temp1 & 1) == 1)
		{
			Output[count] = '1';
		} 
		else
		{
			Output[count] = '0';
		}
		if (temp1 % 4 == 0)		// Split the bytes up for readability
		{
			count++;
			Output[count] = '-';
		}
	}
	Output[19] = 0;
	return Output;
}

void DumpDBptr(DBinfo* DB)
{
	printf("DB->DBnextDB	    = %p \n",DB->DBnextDB);
	printf("DB->DBfilehandle    = %04X \n",DB->DBfilehandle);
	printf("DB->DBpgsize	    = %04X \t(%4d)\n",DB->DBpgsize,DB->DBpgsize);
	printf("DB->DBunknown3	    = %08lX \n",DB->DBunknown3);
	printf("DB->DBunknown4	    = %08lX \n",DB->DBunknown4);
	printf("DB->DBnumtables     = %04X \t(%4d)\n",DB->DBnumtables,DB->DBnumtables);
	printf("DB->DBtableaddress  = %08lX \n",DB->DBtableaddress);
	printf("DB->DBmaxtables     = %04X \t(%4d)\n",DB->DBmaxtables,DB->DBmaxtables);
	printf("DB->DBvflag	        = %04X \t(%4d)\n",DB->DBvflag,DB->DBvflag);
	printf("DB->DBversion	    = %04X \t(%4d)\n",DB->DBversion,DB->DBversion);
	printf("DB->DBfreeaddress   = %08lX \n",DB->DBfreeaddress);
	printf("DB->DBmaxpages      = %08X \t(%8d)\n",DB->DBmaxpages,DB->DBmaxpages);
	printf("DB->PageList1[0]    = %p \n",&DB->PageList1[0]);
	printf("DB->PageList2[0]    = %p \n",&DB->PageList2[0]);
	printf("DB->PageList3[0]    = %p \n",&DB->PageList3[0]);
	printf("DB->SpareList[0]    = %p \n",&DB->SpareList[0]);
	printf("DB->DBname          = %p \t(%s)\n",DB->DBname,DB->DBname);
	printf("DB->DBSemaphore     = %08X \t(%8d)\n\n",DB->DBSemaphore,DB->DBSemaphore);

	DumpBlock((char*) DB, sizeof(DBinfo), 0);

}

void DumpJUNC(EXPR* Expr)
{
	// Dump a Junction record

	printf("\nExpr              = x%p \n",Expr);
	printf("Expr->NextEXPR    = %p \n",Expr->NextEXPR);
	printf("Expr->PrevEXPR    = %p \n",Expr->PrevEXPR);
	printf("Expr->Operator    = %04X (%5d)",Expr->Operator,Expr->Operator);
	if (Expr->Operator == 1)
	{
		printf("  *OR*\n");
	}
	else
	{
		printf("  *AND*\n");
	}
	printf("\nNextEXPR:");
	DumpEXPR(Expr->NextEXPR);
	printf("\nPrevEXPR:");
	DumpEXPR(Expr->PrevEXPR);

}

void DumpEXPR(EXPR* Expr)
{
	DBinfo	*DB;
	TDef	*TDef;
	int		FileOffset;
	char	BinString[20];

	printf("\nExpr              = x%p \n",Expr);
	printf("Expr->NEXT        = %p \n",Expr->NextEXPR);
	printf("Expr->PREV        = %p \n",Expr->PrevEXPR);
	printf("Expr->Operator    = %04X (%5d)",Expr->Operator,Expr->Operator);
	// bitfield operator codes:
	if (Expr->Operator == 0x10)	// equals
		printf("  ==\n");
	else if (Expr->Operator == 0x8)	//less than
		printf("  <\n");
	else if (Expr->Operator == 0x20)	// greater than
		printf("  >\n");
	else if (Expr->Operator == 0x28)	// not equal
		printf("  <>\n");
	else if (Expr->Operator == 0x30)	// Greater or equal
		printf("  >=\n");
	else if (Expr->Operator == 0x18)	// Less than or equal
		printf("  <=\n");
	else 
		printf("  *UNK*\n");

	printf("Expr->Type        = %04X (%5d)\n",Expr->Type,Expr->Type);	 
	printf("Expr->ExprSize    = %04X (%5d)\n",Expr->ExprSize,Expr->ExprSize);	 
	printf("&Expr->ExprData   = %p \n",&Expr->ExprData);
	if (Expr->ExprSize)
	{
		DumpBlock((char*) &Expr->ExprData,Expr->ExprSize + 1, 0);
	}
}

void DumpTDptr(TDinfo* TD,bool ShowTDef)
{
	DBinfo	*DB;
	TDef	*TDef;
	int		FileOffset;
	char	BinString[20];

	printf("TD->TDFlags     = x%04X (%s)\n",TD->TDFlags,DumpShortBits(TD->TDFlags,BinString));
	printf("TD->TDDBinfo    = x%p (%s)\n",TD->TDDBinfo,TD->TDDBinfo->DBname);
	// Each Index table entry is 12 bytes in length
	FileOffset = TD->TDDBinfo->DBtableaddress + (12 * TD->TDindexOff);
	printf("TD->TDindexOff  = x%04X \t(%08X File Offset)\n",TD->TDindexOff,FileOffset);	 
	printf("TD->HalfPageSize= x%04X \t(%4d)\n",TD->HalfPageSize,TD->HalfPageSize);	 
	printf("TD->Rec_plus_DB = x%04X \n",TD->Rec_plus_DB);	 
	printf("TD->TableDefs   = x%p \n",TD->TableDefs);	 
	printf("TD->TDSallocBuf = x%p \n",TD->TDSallocBuf);	 
	printf("TD->TDRecSize   = x%04X \t(%4d)\n",TD->TDRecSize,TD->TDRecSize);	 
	printf("TD->TDRecMin    = x%04X \t(%4d)\n",TD->TDRecMin,TD->TDRecMin);	 
	printf("TD->TDKeySize   = x%04X \t(%4d)\n",TD->TDKeySize,TD->TDKeySize);	 
	printf("TD->TDKeyMin    = x%04X \t(%4d)\n",TD->TDKeyMin,TD->TDKeyMin);	 
	printf("TD->TDNodePtr   = x%p \n",TD->TDNodePtr);	
	if (TD->TDNodePtr)
	{
		DumpBlock((char*) TD->TDNodePtr, 32, 0);
	}
	
	printf("TD->N1_2idx     = x%04X \n",TD->N1_2idx);	 
	printf("TD->KeyBuf1     = x%p \n",TD->KeyBuf1);	 
	printf("TD->Key1Size    = x%04X \n",TD->Key1Size);
	printf("TD->field_3A    = x%04X \n",TD->field_3A);
	printf("TD->KeyBuf2     = x%p \n",TD->KeyBuf2);	 
	printf("TD->Key2Size    = x%04X \n",TD->Key2Size);
	printf("TD->field_42    = x%04X \n",TD->field_42);
	printf("TD->TDKeyDefs   = x%p \n",TD->TDKeyDefs);	 
	printf("TD->KeyDefSize  = x%04X \n",TD->KeyDefSize);	 
	if (TD->TDKeyDefs)
	{
		DumpBlock((char*) TD->TDKeyDefs,TD->KeyDefSize, 0);
	}
	
	DB = TD->TDDBinfo;
	printf("DB->DBpgsize    = x%04X \t(%4d)\n\n",DB->DBpgsize,DB->DBpgsize);
	
	TDef = TD->TableDefs;
	if (TDef && ShowTDef)
	{
		DumpTDefptr(TDef);
	}
}

void DumpTDefptr(TDef* TDef)
{
		while(TDef->TDFsize)
		{
			printf("TDef->TDFentry = %04X \t(%4d)\n",TDef->TDFentry,TDef->TDFentry);	
			printf("TDef->TDFsize  = %04X \t(%4d)\n",TDef->TDFsize,TDef->TDFsize);	
			printf("TDef->TDFtype  = %04X \t(%4d)\n\n",TDef->TDFtype,TDef->TDFtype);
			TDef++;
		}
}

void DumpTTFLD(FLDdesc *FLD); // forward declaration for DumpTTptr

void DumpTTptr(TDesc* TT, bool ShowFields)
{
	FLDdesc *FLD;
	Qlen	*QL;
	char	BinString[20];	// For displaying Binary Bitfields

	printf("TT Address      = x%p\n",(void*)TT);
	printf("TT->DBcontext   = %04X (%4d)\n",TT->DBcontext,TT->DBcontext);		
	printf("TT->TDlocked    = %04X \n",TT->TDlocked);		
	printf("TT->NumFields   = %04X (%4d)\n",TT->NumFields,TT->NumFields);		
	printf("TT->TTrtd       = %04X (%4d)\n",TT->TTrtd,TT->TTrtd);					
	printf("TT->TDrecsize   = %04X (%4d)\n",TT->TDrecsize,TT->TDrecsize);		
	printf("TT->DBnumber    = %04X \n",TT->DBnumber);		
	printf("TT->TTselect    = %p \n",(void*)TT->TTselect);	
	if (TT->TTselect)
	{
		DumpEXPR(TT->TTselect);
	}
	printf("TT->TTfields    = %p \n",(void*)TT->TTfields);	
	printf("TT->TableName   = %s \n",TT->TableName);	
	printf("TT->TableAlias  = %s \n",TT->TableAlias);	
	printf("TT->TDworkarea1 = %p \n",(void*)TT->TDworkarea1);
	if (TT->TDworkarea1)
	{
		DumpBlock((char*)TT->TDworkarea1,TT->TDrecsize, 0);
	}
	printf("TT->TDworkarea2 = %p \n\n",(void*)TT->TDworkarea2);
	if (TT->TDworkarea2)
	{
		//DumpBlock((char*)TT->TDworkarea2,TT->TDrecsize, 0);
	}
	printf("TT->TableCRC    = %04X \n\n",(unsigned short)TT->TableCRC);
	FLD = TT->TTfields;
	if (FLD && ShowFields)
	{
		DumpTTFLD(FLD);
	}
}

void Dump1FLD(FLDdesc *FLD)		// Just dump 1 Field Descriptor
{
	Qlen	*QL;
	char	BinString[20];	// For displaying Binary Bitfields

	printf("FLD->TDFtype    = %04X (%s)\n",FLD->TDFtype,DumpShortBits(FLD->TDFtype,BinString));
	printf("FLD->FLDstat    = %04X ",(unsigned short)FLD->FLDstat);
	if (FLD->FLDstat & fld_KEY)			// 0x8000
		printf("KEY_FIELD ");
	if (FLD->FLDstat & fld_DATA_AVAIL)	// 0x0010		indicates variable accessed in CLCOMP only
		printf("DATA_AVAIL ");
	if (FLD->FLDstat & fld_ZERO)		// 0x0004
		printf("ZERO! ");
	printf("\n");
	printf("FLD->FLDelemID  = %04X (%d)\n",FLD->FLDelemID,FLD->FLDelemID);
	printf("FLD->FLDlen     = %04X (%d)\n",FLD->FLDlen,FLD->FLDlen);
	printf("FLD->FLDqlen    = %p \n",FLD->FLDqlen);
	QL = FLD->FLDqlen;
	if (QL)
	{
		printf("     QL->Qdeflen  = %d\n",QL->Qdeflen);
		//printf("     QL->QLunk1   = %d\n",QL->QLunk1);
		printf("     QL->QfieldHd = %s\n",QL->QfieldHd);
		printf("     QL->QHdrange = %s\n",(char*)(QL->QfieldHd+32));
		printf("     QL->Qdata    = %p",(void*)QL->Qdata);
		if (FLD->FLDtype == 'C')
		{
			printf("\t\"%s\"\n",QL->Qdata);
		}
		else if (FLD->FLDtype == 'N')
		{
			printf("\t%g\n",*(double*)QL->Qdata);
			DumpBlock((char*)QL->Qdata,16, 0);
		}
		else
		{
			printf("\n");
			DumpBlock((char*)QL->Qdata,16, 0);
		}
		printf("     QL->required = %c\n",QL->Qrequired);
	}
	printf("FLD->FLDname    = \"%s\" \n",FLD->FLDname);
	printf("FLD->FLDdata    = %p ",(void*)FLD->FLDdata);
	if (FLD->FLDtype == 'C')
	{
		printf("\t\"%s\"\n",FLD->FLDdata);
	}
	else
	{
		printf("\n");
	}
	DumpBlock((char*)FLD->FLDdata,16, 0);
	printf("FLD->FLDtype    = %02X (%c) \n",FLD->FLDtype,FLD->FLDtype);
	//printf("FLD->FLDunk6    = %X \n",FLD->FLDunk6);
	//printf("FLD->FLDunk7    = %X \n",FLD->FLDunk7);
	//printf("FLD->FLDunk8    = %X \n",FLD->FLDunk8);
	printf("\n");
}

void DumpTTFLD(FLDdesc *FLD)	// Dump all TTfields
{
	while(FLD->TDFtype)
	{
		Dump1FLD(FLD);
		FLD++;
	}
}

#endif


