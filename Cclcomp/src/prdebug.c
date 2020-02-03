#ifndef PRDEBUG_C
#define PRDEBUG_C

#include <ctype.h>	// for isctrl() etc.
#include "lvarnames.h"		// for flag bits

// print function ID
void prtfid(unsigned short ENTABno)
{
    FLDdesc *fld;
    TDesc	*TTptr;
	char	*v5;
    char	*Table;
    ENTAB	*entab;

    int OpCode;
    unsigned int v9;
    int TTno;
	
	entab = ENARR(ENTABno);
	if ( !entab )
		return;
	//DumpBlock(entab, sizeof(ENTAB));
	OpCode = entab->Enun.Enop.Enoper & 0xFC00u;

	fflush(stdout);
    if ( entab->entype == 2 )                  // Expression Type
	{
		//                    x0000 - x5000      ||      x8C00 - xA000
		if ((unsigned int)(OpCode - 1) <= 0x4FFF || OpCode - 0x8C00u <= 0x1400 )
		{
			printf("[%d]", ENTABno);
			switch (OpCode)
			{
				case 0x0400:
					printf(" NOT ");
					break;
				case 0x0800:
					printf(" SQRT ");
					break;
				case 0x0C00:
					printf(" LOG ");
					break;
				case 0x1000:
					printf(" SIN ");
					break;
				case 0x1400:
					printf(" COS ");
					break;
				case 0x1800:
					printf(" TAN ");
					break;
				case 0x1C00:
					printf(" UCASE ");
					break;
				case 0x2000:
					printf(" LCASE ");
					break;
				case 0x2400:
					printf(" SLEN ");
					break;
				case 0x2800:
					printf(" DAY ");
					break;
				case 0x2C00:
					printf(" GETARG ");
					break;
				case 0x3000:
					printf(" ASIZE ");
					break;
				case 0x3400:
					printf(" ASC ");
					break;
				case 0x3800:
					printf(" CHR ");
					break;
				case 0x3C00:
					printf(" SRAND ");
					break;
				case 0x4000:
					printf(" GETENV ");
					break;
				case 0x4400:
					printf(" LOG10 ");
					break;
				case 0x4800:
					printf(" ASIN ");
					break;
				case 0x4C00:
					printf(" ACOS ");
					break;
				case 0x5000:
					printf(" ATAN ");
					break;
				case 0x5800:						// unreachable (also error in real CL4)
					printf("(-)");
					break;
				//case 0x8000:						// unreachable (also error in real CL4)
				//	printf(" SETENV ");
				//	break;
				case 0x8C00:
					printf(" GETXML ");
					break;
				case 0x9000:
					printf(" DEGREES ");
					break;
				case 0x9400:
					printf(" RADIANS ");
					break;
				case 0x9800:
					printf(" EXP ");
					break;
				case 0x9C00:
					printf(" TRIM ");
					break;
				case 0xA000:
					printf(" GETCGI ");
					break;
				default:
					printf(" unop: ?%o? ", entab->Enun.Enop.Enoper);
					break;
			}
		}
        // All string functions follow
		//                  x5400 - x8800    ||           xCC00 - xD400   ||           xF400 - xF800
		else if ( OpCode - 0x5001u <= 0x3BFE || OpCode - 0xC801u <= 0xBFF || OpCode - 0xF001u <= 0x7FF )
        {
			v5 = 0;	// default to print nothing
			switch (OpCode)
			{
				case 0x6C00:
					v5 = "MATCH";
					break;
				case 0x7400:
					v5 = "SDEL";
					break;
				case 0x7800:
					v5 = "SMULT";
					break;
				case 0x7C00:
					v5 = "SWORD";
					break;
				case 0x8000:						// *fix* Is in the above case in *real* clcomp
					v5 = "SETENV";
					break;
				case 0x8400:
					v5 = "SKEY";
					break;
				case 0x8800:
					v5 = "SFMT";
					break;
				case 0xCC00:
					v5 = "SUBSTR";
					break;
				case 0xD000:
					v5 = "SREP";
					break;
				case 0xD400:
					v5 = "SCONV";
					break;
				case 0xF400:
					v5 = "SCAT";
					break;
			}
			if ( v5 )
			{
                printf(" %s(", v5);
                prtfid(entab->enleft);		// *** recursion here ***
                putchar(',');
                prtfid(entab->enright);		// *** recursion here ***
                putchar(')');
				return;
			}
		}
		else if ( OpCode == 0xB800 )
			printf(" random() ");
        else if (OpCode == 0xBC00)
			printf(" keyready() ");
	}
//-----------------------

	prtfid(entab->enleft);							// *** recursion here ***

	switch (entab->entype)
	{
	case 0x02:										// Expression
		v9 = ((unsigned int)entab->Enun.Enop.Enoper >> 9) & 1;		// 0x200 integer calc flag
		if ( v9 & 1 )
			printf(" (");
		else
			putchar(' ');

		if ( entab->Enun.Enop.Enoper & 0x3F )					// mask all but relational operator bits
		{
			printf("[%d]", ENTABno);
		    switch ( entab->Enun.Enop.Enoper & 0x1FF )
		    {
			case 0x01:
				printf("OR");
				break;
			case 0x02:
				printf("AND");
				break;
			case 0x08:
				putchar('<');
				break;
			case 0x10:
				putchar('=');
				break;
			case 0x14:
				printf("like");
				break;
			case 0x18:
				printf("<=");
				break;
			case 0x20:
				putchar('>');
				break;
			case 0x28:
				printf("<>");
				break;
			case 0x30:
				printf(">=");
				break;
			default:
				printf(" relop: ?%o?", entab->Enun.Enop.Enoper);
				break;
			}
		}
		else
		{
			printf("[%d]", ENTABno);
			switch (OpCode)
			{
			case 0x5400:
				putchar('+');
				break;
			case 0x5800:
				if ( entab->enright )
					putchar('-');
				else
					printf("(-)");
				break;
			case 0x5C00:
				printf("**");
				break;
			case 0x6000:
		   		putchar('*');
				break;
			case 0x6400:
				putchar('/');
				break;
			case 0x6800:
		        printf("%%");
				break;
			case 0xAC00:									// Array subscript operator
				putchar('[');
				break;
			default:										// Reserved string names
				if ( entab->Enun.Enop.Enoper & 0x0100 )
					prresv(entab->Enun.Enop.RecNo);					// in this case, RecNo is a bitmask
		
				else if ( entab->enright )
					printf(" op: ?%o?", entab->Enun.Enop.Enoper);
				break;
			}
		}
		if ( v9 )
			printf(") ");
		else
			putchar(' ');
		break;
	case 0x04:
		printf("~%f~", entab->Enun.float4);		// Embedded Float Literal ** not created by real clcomp **
		break;
	case 0x08:
		printf("~%ld~", entab->Enun.long8);		// Embedded Integer Literal
		break;
	case 0x10:
		printf("~%s~", entab->Enun.char16);		// Embedded String Literal ( <= 3 chars)
		break;
	case 0x20:
		putchar(',');								// Function Type. Multiple values follow
		break;
	case 0x01:										// Variable/ Table field
		TTno = entab->Enun.Enref.TTno;
        if ( TTno )
			fld = &ttab[TTno].TTfields[entab->Enun.Enref.VarNum];
		else
			fld = getvars(entab->Enun.Enref.VarNum);

		if (fld->FLDstat & fld_ADDROF)				// DC extension '&' = *ADDRESS OF*
			putc('&', stdout);

		if ( TTno )									// This is a field in a table record
        {
			TTptr = &ttab[TTno];
            if ( TTptr->TableAlias[0] )
				Table = TTptr->TableAlias;
			else
				Table = TTptr->TableName;
			printf("%s.%s", Table, fld->FLDname);
		}
        else if ( fld->FLDstat & fld_ZERO )			// If variable is uninitialised, print var name
        {
			prstr(fld->FLDname);
			if ( *fld->FLDname == '\'' )
				putc('\'', stdout);
		}
        else if ( fld->FLDtype == 'C' )				// else print variable contents if a string
        {
			putc('\'', stdout);
			prstr((char *)fld->FLDdata);
			putc('\'', stdout);
		}
		else
			prstr(fld->FLDname);					// varname for all other types

		printf("[%c] ", fld->FLDtype);
		break;

	default:	// Error, Illegal expression type!
		__assert_fail("((tf)->entype == 0x1)", "prdebug.c", 0xAEu, "prtfid");
		break;
	}

    prtfid(entab->enright);	// *** Many levels of recursion possible!! ***

	if ( entab->entype == 2 && OpCode == 0xAC00 )	// Array subscript operator
	{
        putchar(']');
		return;
	}
}

void prstr(char *a1)
{
    int v3;
	
	while ((v3 = *a1++))
	{
		if (iscntrl(v3))
			printf("^%c", v3 + 0x40);
		else
			putc(v3, stdout);
	}
}

void prresv(int ReservedMask)
{
    const char *v1;
	
	switch (ReservedMask)
	{
		case 0x0001:
			v1 = "'path'";
			break;
		case 0x0002:
			v1 = "'version'";
			break;
		//case 0x0004:			// 0x0004 free?
		case 0x0008:
			v1 = "'date'";
			break;
		case 0x0010:
			v1 = "'time'";
			break;
		case 0x0020:
			v1 =  "'pname'";
			break;
		case 0x0040:
			v1 = "'uname'";
			break;
		case 0x0080:
			v1 = "'pid'";
			break;
		case 0x0100:
			v1 = "'uid'";
			break;
		case 0x0200:
			v1 = "'login'";
			break;
		case 0x0400:
			v1 = "'tty'";
			break;
		case 0x0800:
			v1 = "'nname'";
			break;				
		//case 0x1000:			// 0x1000 free??
		case 0x2000:
			v1 = "'pwd'";
			break;
		case 0x4000:			// unreachable from clcomp generated code
			v1 = "'licensee'";
			break;
		//case 0x8000:			// 0x8000 free??
		default:	
			printf("'-%o-'", ReservedMask);			// unknown, just print mask value
			return;
			break;
	}
	printf("%s",v1);			// stop compiler warning on later GCC (format not a string literal and no format arguments)
    return;
}

// Print a screen accept definition
void prxt(XTAB *xtab, bool isHDTXT)
{
	FLDdesc	*fld;
	RTAB	*rtab;
	RATAB	*ratab;

	short	fno;
	int		TTno;
	char	FLDtype;

    if ( !xtab )
		return;

    prtfid(xtab->VarExpNo);
    putchar(' ');
    
	if ( xtab->widthEXP )
		prtfid(xtab->widthEXP);
    else
		printf("%d", xtab->width);
    
	if ( xtab->Flags & xtb_DISPLAY )		// 0x0004u	// this is a DISPLAY ONLY field
		printf(",d");
	else if ( xtab->Flags & xtb_REQUIRED)   // 0x0400u	// value is required for this field
		printf(",r");
	
	printf(" at ");
    
	if ( xtab->PCol_exp )					// expression trumps fixed number
		prtfid(xtab->PCol_exp);
	else
		prcolrow(xtab->PCol);
    
	if ( xtab->ScrAttrib )
    {
		switch ( xtab->ScrAttrib )
		{
        case 0x01:
			printf(",f");
            break;
		case 0x02:
			printf(",s");
            break;
		case 0x04:
			printf(",u");
            break;
		case 0x08:
			printf(",b");
            break;
		case 0x10:
			printf(",B");
            break;
		case 0x20:
			printf(",r");
            break;
		case 0x3F:
            printf(",n");
			break;
		default:
			printf(",%o", xtab->ScrAttrib);
            break;
        }
	}
    putchar(' ');
    if ( xtab->PLine_exp )					// expression trumps fixed number
		prtfid(xtab->PLine_exp);
	else
		prcolrow(xtab->PLine);
	putchar(' ');
    
	if ( !isHDTXT )						// true if this is an accept field. Show the Answer Col/Line values
	{
		if ( xtab->ACol_exp )				// expression trumps fixed number
			prtfid(xtab->ACol_exp);
		else
			prcolrow(xtab->ACol);
		if ( xtab->Attr )
        {
			switch ( xtab->Attr )
            {
            case 0x01:
				printf(",f");
                break;
			case 0x02:
				printf(",s");
                break;
			case 0x04:
				printf(",u");
                break;
			case 0x08:
				printf(",b");
                break;
			case 0x10:
				printf(",B");
                break;
			case 0x20:
				printf(",r");
                break;
			case 0x3F:
				printf(",n");
                break;
			default:
				printf(",%o", xtab->ScrAttrib);
                break;
			}
		}
        putchar(' ');
        if ( xtab->ALine_exp )
			prtfid(xtab->ALine_exp);
		else
			prcolrow(xtab->ALine);
		putchar(' ');
	}
    prtfid(xtab->Prompt_exp);
    
	if ( !isHDTXT )
    {
		fno = gettf(xtab->VarExpNo, &TTno, &FLDtype);
		assert(fno >= 0);
		
		if ( TTno )
			fld = &ttab[TTno].TTfields[fno];
		else
			fld = getvars(fno);
		
		if ( xtab->Flags & xtb_FDF )		// 0x0008u FDF First Display Field for a block
			printf(" FDF");
		
		if ( xtab->Flags >= 0 )
        {
			if ( fld->FLDstat < 0 )
				printf(" key");
		}
		else
        {
			printf(" tkey");
            if ( xtab->Flags & xtb_LKF )	// 0x0040u		// LKF Last Key Field
				printf(" LKF");
		}

//--------- indicate variable type expected ---------

		if ( fld->FLDstat & 0x0008 )		// ???			// Field is assigned a value in the code also
			printf(" result");
		if ( xtab->Flags & xtb_NUMERIC )	// 0x0200u		// expect NUMERIC value
			printf(" num");
		if ( xtab->Flags & xtb_CHAR )		// 0x0001u		// expect string value
			printf(" char");
		if ( xtab->Flags & xtb_DATE )		// 0x0002u		// expect DATE value
			printf(" date");
		if ( xtab->Flags & xtb_TIME )		// 0x0800u		// expect TIME value
			printf(" time");
		
		if ( xtab->Flags & 0x0110 )			// join or no_join flags
        {
			rtab = RTARR(xtab->RangeID);
			printf( xtab->Flags & 0x0010 ? "\n\t\tjoin " : "\n\t\tno_join ");
            if ( xtab->Flags & xtb_LOCK )
				printf("lock ");

			prtfid(rtab->enleft);
            printf(" = ");
            prtfid(rtab->enright);
            if ( rtab->WhereEXP )
            {
				printf(" where ");
                prtfid(rtab->WhereEXP);
			}
			if ( xtab->Width )
				printf(" vw=%d", xtab->Width);// visible width?
			return;
		}
        if ( !xtab->RangeID )
        {
			if ( xtab->Width )
				printf(" vw=%d", xtab->Width);// visible width?
			return;
		}
        printf("\n\t\trange ");
		
		ratab = RAARR(xtab->RangeID);
		switch (ratab->RangeType)
		{
		case 0x01:
			printf(" POSITIVE ");
			break;
		case 0x02:
			printf(" NEGATIVE ");
			break;
		case 0x06:
            printf(" NAME ");
			break;
		default:
			prtfid(ratab->RangeFrom);
            if ( !(xtab->Flags & 0x01) )
            {
				printf(" to ");
                prtfid(ratab->RangeTo);
			}
			break;
		}
        if ( ratab->StringOffset )
			printf(" '%s'", &strarr.StringTable[ratab->StringOffset - 1]);
		if ( xtab->Width )
			printf(" vw=%d", xtab->Width);// visible width?
		return;
	}
}

void prcolrow(unsigned short a1)
{
//printf("prcolrow(x%04X)\n",a1);	
	switch (a1)			// a1 = bitmask
	{
	case 0x1000:
		putchar('c');
		break;

	case 0x2000:
		putchar('r');
		break;

	default:
		switch (a1 & 0x7E00)				// 0111 1110 0000 0000
		{
		case 0x0200:
		case 0x1000:
			printf(">%d",(a1 & 0x1FF));		// range = 0 to 511   - Max 512 screen width
			break;

		case 0x0400:
		case 0x2000:
			printf("<%d", (a1 & 0x1FF));
			break;

		case 0x0800:
		case 0x4000:
			printf("=%d", (a1 & 0x1FF));
			break;

		default:
			printf("%d", (a1 & 0x1FF));		// default case, only reachable if multiple bits set
			break;
		}
		break;
	}
}

// Show an assignment expression
void prcalc(ENTAB *ent)
{
    char *v2;
	
	if ( ent )
    {
		prtfid(ent->enleft);				// Destination 	
        if ( ent->entype != 2 )				// 2 = calculation assignment. Other is string assighnment?
        {
			printf(" = ");
            prtfid(ent->enright);
            return;
		}

		switch (ent->Enun.Enop.Enoper & 0xFC00)			// OpCode
		{
		case 0xA800:
			v2 = "=";
			break;
		case 0xD800:
			v2 = "+=";
			break;
		case 0xDC00:
			v2 = "-=";
			break;
		case 0xE000:
			v2 = "*=";
			break;
		case 0xE400:
			v2 = "/=";
			break;
		case 0xE800:
			v2 = "%=";
			break;
		case 0xEC00:
			v2 = "**=";
			break;
		default:
			v2 = "<UNKNOWN>";
			break;
		}

		if ( ent->Enun.Enop.Enoper & 0x0200 )			// Integer result
			printf(" (%s) ", v2);
        else if ( ent->Enun.Enop.Enoper & 0x0040 )		// *LIKE* wildcard flag - indicate possible loss of precision in result
			printf(" %s RND ", v2);			
        else
			printf(" %s ", v2);

		prtfid(ent->enright);
        return;
	}
}

void prrt(unsigned short RTno)
{
    RTAB *rtb; // esi@1

	rtb = RTARR(RTno);
	if ( rtb )
    {
		putchar(' ');
        prtfid(rtb->enright);
        prrt(rtb->NextRT);    // some lovely recursion
	}
}

void prpf(unsigned short PRno)		// print flags
{
    PRTAB	*prtab;
	const char *v5;
	int		v4;
	int		v6;
	
	prtab = PRARR(PRno);
	if ( prtab )
    {
		if ( prtab->ReservedFlags )
        {
			switch (prtab->ReservedFlags)
			{
			case 0x0001:
				printf("cl");
				break;
			case 0x0002:
				printf("ff");
				break;
			case 0x0004:
				printf("nl");
				break;
			case 0x1000:
				printf("cr");
				break;
			default:
				prresv(prtab->ReservedFlags);			// reserved string name
				break;
			}
		}
		else
		{
			prtfid(prtab->VAR_ENTno);
		}
//--------------------------------------
		if ( prtab->WIDTH_FLAGS )
        {
			v4 = prtab->WIDTH_FLAGS & 0x7C00;			//	0111 1100 0000 0000
			v6 = prtab->WIDTH_FLAGS & 0xFFFF83FF;		//	1000 0011 1111 1111
			switch (v4)
			{
			case 0x0800:
			    v5 = ",%dx ";
				break;
			case 0x1000:
			    v5 = ",%dc ";
				break;
			case 0x2000:
			    v5 = ",%dr ";
				break;
			case 0x4000:
			    v5 = ",%dl ";
				break;
			default:	// case 0x0400
				v6 = prtab->WIDTH_FLAGS;
			    v5 = ",%d ";
				break;
			}
			printf(v5, v6);
		}
		else
		{
			if ( prtab->WIDTH_ENTno )
            {
				putchar(',');
                prtfid(prtab->WIDTH_ENTno);
			}
            else
            {
				putchar(' ');
			}
		}
		prpf(prtab->NextPR);	// ** Many levels of recursion possible **
	    return;
	}
}

// Show Redisplay Info
void prrd(unsigned short RDno)
{
     RDTAB *rdtab; // esi@1

     rdtab = RDARR(RDno);
	 if ( rdtab )
     {
          if ( rdtab->TTno <= 0 )
               prtfid(rdtab->ENTno);						// single field re-display
          else
               printf("%s", ttab[rdtab->TTno].TableName);	// redisplay all members of a record
          putchar(' ');
          prrd(rdtab->NextRD);								// ** Many levels of recursion possible **
     }
}

void prvars(bool ShowTree)
{
	FLDdesc *fld;
	short VarNum;
	char VarName[32];
	 
	if ( ShowTree )
    {
		prtree(troot, 0);
	}
	else
	{
		VarNum = 0;
		printf("Var VariableName         Typ Length\n");
		//      No. of variables = 253
		//        0 NULL                 [C] 20
		//        1 ERROR                [I] 2

		while ( (fld = getvars(VarNum)) )
		{
			if ( fld->FLDelemID )				// ElemID used as Array Size
				sprintf(VarName, "%s[%d]", fld->FLDname, fld->FLDelemID);
			else
				sprintf(VarName, "%s", fld->FLDname);
			
			printf("%3d %-20s [%c] %d", VarNum, VarName, fld->FLDtype, fld->FLDlen);
			fflush(stdout);
			
			if ( !(fld->FLDstat & fld_ZERO) )	// variable assigned a value at compile time, show it
			{
				switch (fld->FLDtype)
				{
				case 'C':
					printf("\t('%s')", (char*)fld->FLDdata);
					break;
		
				case 'I':
				case 'N':
				case 'B':
					printf("\t(%ld)", (long int)*(double *)fld->FLDdata);
					break;
		
				default:
					printf("\t(%f)", *(double *)fld->FLDdata);
					break;
				}
			}
			putchar('\n');
			VarNum++;
		}
	}
	return;
}

void prtree(VARTREE *a1, int Depth)
{
     if ( a1 )
     {
          prtree(a1->VarPrev, Depth + 1);			// Recursion here
          printf("%3d %s\n", Depth, a1->VarName);
          prtree(a1->VarNext, Depth + 1);			// Recursion here
     }
}

void prset(PTAB *ptb)
{
     int OpCode;
	 short Operand;
	 
    OpCode	= ptb->OpCode;
    Operand = ptb->Operand;
    printf("set ");
	switch ( OpCode )
    {
		case 673:
            printf("dateform,[%d]",Operand);
			break;
		case 652:
            printf("tab,[%c]", Operand);
			break;
		case 654:
            printf("fill,[%c]", Operand);
			break;
		case 650:
        case 656:
        case 658:
        case 660:
        case 662:
        case 664:
        case 668:
			switch ( OpCode )
            {
				case 650:
					printf("message,");
                    break;
				case 656:
					printf("accept,");
                    break;
				case 658:
					printf("prompt,");
                    break;
				case 660:
					printf("error,");
                    break;
				case 662:
					printf("head,");
                    break;
				case 664:
					printf("text,");
                    break;
				case 668:
					printf("print,");
                    break;
				//default:				// should be unreachable
				//	break;
			}
            switch ( Operand )
            {
				case 0x01u:
					putchar('f');		// feint (half-intensity)
                    break;
				case 0x02u:
					putchar('s');		// standout
                    break;
				case 0x04u:
					putchar('u');		// underline
                    break;
				case 0x08u:
					putchar('b');		// bold
                    break;
				case 0x10u:
					putchar('B');		// ???
                    break;
				case 0x20u:
					putchar('r');		// reverse?
                    break;
				case 0x3Fu:
					putchar('n');		// normal
                    break;
				default:
					printf("(%d)", Operand);
					break;
			}
            return;
		case 670:
            printf("abort,%s", Operand == 1 ? "on": "off");
			break;
		case 684:
			printf("align,%d", Operand);
			break;
		case 665:
			printf("E,%s", Operand == 1 ? "on": "off");
			break;
        case 630:
            printf("csv,%s", Operand == 1 ? "on": "off");
			break;
        case 640:
           printf("dos,%s", Operand == 1 ? "on": "off");
			break;
        case 682:
            printf("goback,%s", Operand == 1 ? "on": "off");
			break;
        case 680:
            printf("trim,%s", Operand == 1 ? "on": "off");
			break;
        case 676:
            printf("aauto,%s", Operand == 1 ? "on": "off");
			break;
        case 678:
           printf("rauto,%s", Operand == 1 ? "on": "off");
			break;
        case 679:
            printf("repeat,%s", Operand == 1 ? "on": "off");
			break;
        case 666:
            printf("justify,%s", Operand == 1 ? "on": "off");
			break;
        case 681:
            printf("hangup,%s", Operand == 1 ? "on": "off");
			break;
        case 686:
            printf("convert,%s", Operand == 1 ? "on": "off");
			break;
        case 687:
            printf("rawdisplay,%s", Operand == 1 ? "on": "off");
			break;
        case 688:
            printf("rawprint,%s", Operand == 1 ? "on": "off");
			break;
        case 659:
            printf("null_exit,%s", Operand == 1 ? "on": "off");
			break;
        case 672:														// debug
            printf("debug,%s", Operand == 1 ? "on": "off");
			break;
        case 674:														// fnum		*** not defined in real clcomp ***
            printf("fnum,%s", Operand == 1 ? "on": "off");
			break;
        case 651:
            printf("xml,%s", Operand == 1 ? "on": "off");
			break;
        case 669:
			if ( Operand )
				printf("skip,%s", Operand == 1 ? "on": "off");
            else
				printf("skip");
            break;
		case 667:
            printf("output,%s", Operand == 1 ? "console": "stdout");	//printf("print,%s", v25);	*** wrong in real clcomp ***
			break;
		default:
			printf("unknown,[%c]", Operand);
			break;
	}
}

void prtds()
{
    TDesc	*TTptr;
    PTAB	*pt;
    SCR		*scr;
    int		*BTCOUNT;
    BTAB	*bt;

	short	count;
    short	scrn_no;

    BTCOUNT = (int*) alloca(sizeof(int*) * no_btabs);

    count = 0;					// pointless really, alloca clears memory?
    while ( no_btabs > count )
		BTCOUNT[count++] = 0;

	count = 0;
    while((pt = PTARR(getptabp(count)), pt->OpCode))
    {
		if ( pt->OpCode == 720 || pt->OpCode == 600 )	// look for 'do' or 'format' OpCodes
			++BTCOUNT[pt->Operand];
		count++;
	}
    
	puts("\n<-- block name -->   <mode>  <td> <start>  <end> <calls>");
    count = 0;
    bt = btab;
	while ( bt->BlockName[0])
    {
		if ( bt->On_delete )			// If block is referred to by on_condition instructions, bump access count.
			++BTCOUNT[count];             
		if ( bt->On_exit )
			++BTCOUNT[count];
		if ( bt->On_excep )
			++BTCOUNT[count];
		if ( bt->On_entry )
			++BTCOUNT[count];
        
        printf("%-20.20s  %-5.5s  %3d   %5d  %5d   calls=%5d",bt->BlockName, bt->Maintain,bt->TTno,bt->StartLine, bt->EndLine, BTCOUNT[count]);
        if ( count && !BTCOUNT[count] )
			print(" block not called");

		if ( bt->On_delete )
            printf("  on_delete %s (%d) ", bt->On_delete->BlockName, bt->On_delete->BlockNo);
        if ( bt->On_exit )
            printf("  on_exit %s (%d) ", bt->On_exit->BlockName, bt->On_exit->BlockNo);
        if ( bt->On_excep )
            printf("  on_exception %s (%d) ", bt->On_excep->BlockName, bt->On_excep->BlockNo);
        if ( bt->On_entry )
            printf("  on_entry %s (%d) ", bt->On_entry->BlockName, bt->On_entry->BlockNo);
        putchar('\n');
//---------------------------          
		scr = bt->Screens;				// Screen defs
        scrn_no = 0;
        while ( scr )
		{
            printf("scr%d(%d-%d) ", scrn_no, scr->PT_start,scr->PT_end);
            scrn_no++;
			scr = scr->NextSCR;
        }
        printf("scrs(%d), fmts(%d)\n", bt->Scrs,bt->fmts);
		bt++;
		count++;
	}
//==============================
	puts("\ntd <- database -> <- table name -> < alias >  <status>");
    TTptr = ttab;
	count = 0;
    while ( no_ttabs > count )
    {
		if ( TTptr->TableName[0] )
        {
            printf("%2d %-14.14s %-14.14s   %-10.10s ", count, dtab[TTptr->DBnumber].FullDBname, TTptr->TableName, TTptr->TableAlias);
            if ( TTptr->TDlocked & ttl_LOCK)	// 0x0001
				printf("lock ");
			if ( TTptr->TDlocked & ttl_PUT )	// 0x0002
				printf("put ");
			if ( TTptr->TDlocked & ttl_GET )	// 0x0004
				printf("get ");
			if ( TTptr->TDlocked & ttl_GETK)	// 0x0008
				printf("getk ");
			if ( TTptr->TDlocked & ttl_HASK)	// 0x0010
				printf("hask ");
			if ( TTptr->TDlocked & ttl_AVAIL)	// 0x0020
				printf("avail ");
			if ( TTptr->TDlocked & ttl_FLOCK)	// 0x0040
				printf("flock ");
			if ( TTptr->TDlocked & ttl_LOCKR)	// 0x0080
				printf("lockr ");
			if ( TTptr->TDlocked & ttl_GETNP)	// 0x0100
				printf("getn/p ");
			if ( TTptr->TDlocked & ttl_CREATE)	// 0x0200
				printf("create ");
			if ( TTptr->TDlocked & ttl_UNIQUE)	// 0x0400
				printf("unique ");
			// 0x1000? 
			// 0x2000?
			if ( TTptr->TDlocked & ttl_HASSUB)	// 0x4000  has sub-records
				printf("hass ");
			if ( TTptr->TDlocked & ttl_DEL)		// 0x8000 delete is called on this TT
				printf("del ");
			if (!(TTptr->TDlocked & 0x0800))
				printf("not_open ");
			putchar('\n');
		}
        count++;
        TTptr++;
	}
}

void prkx(KXTAB *kxtab)
{
	int *TTcount;
	short *ptr;

	printf("[%s] tds: ", kxtab->TableName);
	for ( TTcount = kxtab->TTlist; *TTcount; ++TTcount )
		printf("%d ", *TTcount);
	
	printf("\n\t\tkey doms: ");
	for ( ptr = kxtab->KeyElemIDs; *ptr; ++ptr )
		printf("%d ", *ptr);
	
	printf("\n\t\tkey ty: ");
	for ( ptr = kxtab->TkeyTy; *ptr; ++ptr )
		printf("%#o ", *ptr);
	
	printf("\n\t\tdata doms: ");
	for ( ptr = kxtab->DataElemIDs; *ptr; ++ptr )
		printf("%d ", *ptr);
	
	printf("\n\t\tntds=%d, nkeys=%d, ndata=%d, ks=%d, rs=%d\n", kxtab->NumTTs, kxtab->NumKeyFields, kxtab->NumDataFields, kxtab->KeySize, kxtab->RecSize);

	printf("\t\twhere ");
	prtfid(kxtab->WhereEXP);
}

void prdatefield(unsigned short ENTno, unsigned short XTno, PRTAB *pr)
{
	ENTAB	*ent;
	//ENTAB	*v4;
	ENTAB	*v12;
	XTAB	*xtb;
	FLDdesc *fld;

	TDesc	*TTptr;

	char	*v10;
	char	*v20;

	int		TTno;
	short	LeftNo;
	bool	isExpr;

	//	ent = ENARR(ENTno);
	//	while (ent && ent->enleft)
	//		ent = ENARR(ent->enleft);

	ent = ENARR(ENTno);

	for (LeftNo = ent->enleft; LeftNo ; LeftNo = ent->enleft )
		ent = &enarr.TableAddr[LeftNo - 1];

//	if ( ent->enleft )
//    {
//		if ( !ent->enleft )	// Should be unreachable due to test above
//        {
//			v4 = 0;			// Should cause a CRASH!
//            goto LABEL_7;
//		}
//        do
//        {
//			v4 = &enarr.TableAddr[ent->enleft - 1];
//	LABEL_7:
//            ent = v4;
//		}
//        while ( v4->enleft );
//	}
    
	assert(ent->entype == 0x01);

	xtb = XTARR(XTno);
	
	TTno = ent->Enun.Enref.TTno;
    if ( TTno )
		fld = &ttab[TTno].TTfields[ent->Enun.Enref.VarNum];		// Field in a Table
	else
		fld = getvars(ent->Enun.Enref.VarNum);					// Normal variable

	if ( fld->FLDtype == 'D' )
    {
		++count_0;
        if ( count_0 == 1 )		// only show datefield check message once if applicable
            print("cl (%s) checking %s.ent for date fields\n", getclver(), clbasename(ename)); // version, EntFilename

		print("%s %s: ln %d: ", FileTree->FileName, xtb? "Input" : "Output", (unsigned short)ll);

        if ( TTno )				// field in table
        {
			TTptr = &ttab[TTno];
			print("%s.%s ", TTptr->TableAlias[0] ? TTptr->TableAlias : TTptr->TableName, fld->FLDname);
		}
        else					// normal variable
			print("%s ", fld->FLDname);

		if ( xtb )
        {
			if ( xtb->widthEXP )
				prtfid(xtb->widthEXP);
			else
				print("%d.", xtb->width);
            print("\n");
            return;
		}
        if ( !pr )
		{
            print("\n");
			return;
		}
		if ( !pr->WIDTH_ENTno )
        {
			print("fw=%d\n", pr->WIDTH_FLAGS & 0x83FFu);		// 1000 0011 1111 1111
			return;
		}
        v12 = &enarr.TableAddr[pr->WIDTH_ENTno - 1];			// must be true after block above
        isExpr = false;
        while ( v12->enleft )
        {
			isExpr = true;
			v12 = ENARR(v12->enleft);		// drill down
		}
		assert(v12->entype == 0x01);		// check expression type == 1

		TTno = v12->Enun.Enref.TTno;
          
		if ( TTno )
			fld = &ttab[TTno].TTfields[v12->Enun.Enref.VarNum];		// field in table
		else
			fld = getvars(v12->Enun.Enref.VarNum);					// normal variable
          
		if ( fld->FLDtype == 'C' )
        {
			if ( TTno )
            {
				TTptr = &ttab[TTno];
				print("%s.%s ", TTptr->TableAlias[0] ? TTptr->TableAlias : TTptr->TableName, fld->FLDname);
			}
            else
            {
				if ( *fld->FLDname == '\'' )		// ??? mask literals ???
					print("mask=literal val=[%s] ", fld->FLDdata);
				else
					print("mask=[%s] val=[%s] ", fld->FLDname, (char *)fld->FLDdata);
			}
		}
        else
        {
			if ( TTno )
            {
				TTptr = &ttab[TTno];
				print("%s.%s [%c] ", TTptr->TableAlias[0] ? TTptr->TableAlias : TTptr->TableName, fld->FLDname, fld->FLDtype);
			}
			else
				print("var=[%s[%c]] ", fld->FLDname, fld->FLDtype);
		}
        if ( isExpr )
			print("expr ");
		print("\n");
        return;
	}
}

#endif
