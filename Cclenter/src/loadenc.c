#ifndef LOADENC_C
#define LOADENC_C

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

bool compile(char *FileName)
{
    char *ClcPath;
	char *LastChar;
	
    __pid_t v5;
	__pid_t v6;
	__pid_t v17;
	
    short argv_idx; // edi@14

    int v14;
	int v18;
	int v22;
	__sighandler_t handler;
	
	int stat_loc;
	char *argv[12];
	char name[64];
	char dest[128];
	
    if ( isCGI )
        sprintf(name, "/tmp/clcompout.%d", getpid());	// if running as clcgi, create enc file in tmp dir

    if ( !cmpbuf(fullpname, "clenter", 3) )				// if clenter was executed with full path, look for clcomp there
    {
		ClcPath = mstrcpy(fullpname, 0);				// grab a local copy of the full program name
        LastChar = &ClcPath[lenstr(ClcPath) - 1];		// position to last char of string before \0

		while (ClcPath < LastChar && *LastChar != '/')	// strip program name, and leave path
			--LastChar;
        if ( *LastChar == '/' )
			LastChar++;
		*LastChar = 0;
    }
	else
		ClcPath = mstrcpy(0, 0);	// clenter was invoked without a path

    v5 = fork();
    v6 = v5;
    if ( !v5 )			// this is the child process just created
    {
//=====================================================
		// build compile command array
        argv[0] = mstrcpy(ClcPath, "clcompDC", 0);		// path/clcomp always arg[0]
        argv_idx = 1;
        if ( noWarnings )
            argv[argv_idx++] = "-n";
        
		if ( lddeb )
            argv[argv_idx++] = "-D";
        
		if ( incl_dir )
        {
            argv[argv_idx++] = "-I";
            argv[argv_idx++] = incl_dir;
        }
        if ( dbrename )
        {
            argv[argv_idx++] = "-r";
            argv[argv_idx++] = dbrename;
        }
        if ( extendDC )								// **** DC Extension ****
            argv[argv_idx++] = "-E";

		if ( tpenc )
            argv[argv_idx++] = "-s";

		argv[argv_idx++] = FileName;
        argv[argv_idx] = 0;							// maximum last index is [10]

//==============================================
        if ( isCGI )	// running as clcgi
        {
            v14 = 1;
            strcpy(dest, argv[0]);
            if ( argv_idx > 1 )
            {
                do
                {
                    strcat(dest, " ");
                    strcat(dest, argv[v14++]);
                }
                while ( v14 < argv_idx );
            }
            strcat(dest, " > ");
            strcat(dest, name);
            strcat(dest, " 2>&1");
            execlp("/bin/sh", "sh", "-c", dest, NULL);	// *** doesn't return if successful ***
			syserror("pfork:execlp[%s]", dest);
        }
        else	// normal stdio environment
        {
            execvp(argv[0], argv);		// *** doesn't return if successful ***
			syserror("%s", argv[0]);
        }
        _exit(127);
    }
    if ( v5 == -1 )	// error, couldn't fork the child compile process
    {
        stat_loc = *__errno_location() << 8;
    }
    else			// This is the parent process continuing
    {
        handler = signal(SIGQUIT, SIG_IGN);
        do
        {
            v17 = wait(&stat_loc);		// wait for the child started above to complete
        }
        while ( v17 != v6 && v17 != -1 );
        signal(SIGQUIT, handler);
    }
    //v22 = BYTE1(stat_loc);		************
	v22 = (unsigned char)((stat_loc >>8) & 0x7F);
    if ( stat_loc & 0x7F )
        v22 = 0;
    
	v18 = 0;
    if ( (char)((char)((stat_loc & 0x7F) + 1) >> 1) >= 1 )
        v18 = stat_loc & 0x7F;
    
	if ( v22 || v18 )		// error conditions
    {
        eprint("\n%s: ", "clcomp");
        if ( v6 == -1 )
        {
            eprint("No more processes\n");
        }
        else
        {
            if ( v18 )
                eprint("signal = %d, ", v18);
            eprint("errors = %d\n", v22);
        }
        return false;		// error_exit
    }
    else	// success
    {
        if ( isCGI )
            unlink(name);	// file will disappear when we exit
        return true;		// exit_success!
    }
}

char *checkdbrename(char *DBname)
{
    const char **v1;
	size_t len;
	char *NewDBname;
	const char *ptr;
	
    v1 = (const char **)dbarray;
    if ( v1 )
    {
        ptr = mstrcpy(DBname, "=", 0);
        len = strlen(ptr);
        NewDBname = 0;
		while (*v1)
		{
			if (!strncmp(ptr, *v1, len))	// match!
			{
	            NewDBname = mstrcpy((char *)&(*v1)[len], 0);
				break;
			}
			v1++;
		}
        mfree_0((void *)ptr);
        if ( NewDBname )
        {
            mfree_0(DBname);	// replaces memory pointed to by *DBname
            DBname = NewDBname;
        }
    }
    return DBname;
}

ONESC *getonesc()
{
    ONESC *esc;
	
    esc = (ONESC *)mmalloc(sizeof(ONESC));		// 16
    if ( tpenc )
    {
        esc->OESCFlags	= getcount();
        esc->BlockNo	= getcount();
        esc->KeyNumber	= getcount();
        esc->NextESC	= (ONESC *)(int)getcount();		// wrong size. Adjusted by caller?
    }
    else
        getbuf((char *)esc, sizeof(ONESC));		// 16
    return esc;
}

ONESC *loadoe()
{
    ONESC *v0;
	ONESC *v1;
	ONESC *v2;
	
    v0 = getonesc();
    v1 = v0;
    if ( v0->NextESC )
    {
        do
        {
            v2 = getonesc();
            v0->NextESC = v2;
            v0 = v2;
        }
        while ( v2->NextESC );
    }
    return v1;
}

SCR *getscr(void)
{
    SCR *scr;
	
    scr = (SCR *)mmalloc(sizeof(SCR));		// 8
    if ( tpenc )
    {
        scr->PT_start	= getcount();
        scr->PT_end		= getcount();
        scr->NextSCR	= (SCR *)(int)getcount();	// wrong!  adjusted by caller
    }
    else
        getbuf((char *)scr, sizeof(SCR));	// 8

	return scr;
}

SCR *loadscr(void)
{
    SCR *v0; // esi@1
    SCR *v1; // edi@1
    SCR *v2; // eax@2

    v0 = getscr();	// always one, as there always has to be at least one block
    v1 = v0;
//printf("loadscr : PT_Start = %d, PT_end = %d \n",v0->PT_start, v0->PT_end);
    while ( v0->NextSCR )
    {
		v2 = getscr();
        v0->NextSCR = v2;
        v0 = v2;
//printf("loadscr : PT_Start = %d, PT_end = %d \n",v0->PT_start, v0->PT_end);
    }
    return v1;
}

//====================================================

short getcount()
{
    short ptr;
	short Dest;
	
    getbuf((char *)&ptr, sizeof(short));
    if ( tpenc )
    {
        domtox((char *)&Dest, (char *)&ptr, sizeof(short), 0x44u);		// signed short
        return Dest;
    }
    else
        return ptr;
}

unsigned short getucount()
{
    unsigned short ptr;
	unsigned short Dest;
	
    getbuf((char *)&ptr, sizeof(unsigned short));
    if ( tpenc )
    {
        domtox((char *)&Dest, (char *)&ptr, sizeof(unsigned short), 0x40u);		// unsigned short
        return Dest;
    }
    else
    {
        return ptr;
    }
}

short *getarrcount(unsigned int a1)
{
    short *v1;
	unsigned int i;
	
    v1 = (short *)mmalloc(sizeof(short) * (a1 + 1));	// +1 ensures null terminated array
    if ( tpenc )
    {
        for ( i = 0; i < a1; v1[i++] = getcount() )
            ;
    }
    else
    {
        getbuf((char *)v1, sizeof(short) * a1);
    }
    return v1;
}

int gettable()
{
    int Dest;
	int ptr;
	
    getbuf((char *)&ptr, sizeof(int));
    if ( tpenc )
    {
        domtox((char *)&Dest, (char *)&ptr, sizeof(int), 0x44u);	// signed int value
        return Dest;
    }
    else
        return ptr;
}

int *getarrtds(unsigned int count)
{
    int *v1; // edi@1
    unsigned int i; // esi@2

    v1 = (int *)mmalloc(sizeof(int) * (count + 1));	// +1 ensures null terminated array
    if ( tpenc )
    {
        for ( i = 0; i < count; v1[i++] = gettable() )
            ;
    }
    else
    {
        getbuf((char *)v1, sizeof(int) * count);
    }
    return v1;
}

int getbool()		// returns int, not a 1byte bool
{
    int Dest;
	int ptr;
	
    getbuf((char *)&ptr, sizeof(int));
    if ( tpenc )
    {
        domtox((char *)&Dest, (char *)&ptr, sizeof(int), 0x44u);	// signed int value
        return Dest;
    }
    else
        return ptr;
}

unsigned short getbits()
{
    unsigned short ptr;
	unsigned short Dest;
	
    getbuf((char *)&ptr, sizeof(unsigned short));
    if ( tpenc )
    {
        domtox((char *)&Dest, (char *)&ptr, sizeof(unsigned short), 0x40u);	// unsigned short value
        return Dest;
    }
    else
        return ptr;
}

long getlong()
{
	long Dest;
	long ptr;
	
    getbuf((char *)&ptr, sizeof(long));
    if ( tpenc )
    {
        domtox((char *)&Dest, (char *)&ptr, sizeof(long), 0x44u);	// 4 bytes on X86, 8 on X64
        return Dest;
    }
    else
        return ptr;
}

double *getdouble(short TDFtype)
{
    double *v1;
	double *Dest;
	
    v1 = (double *)mmalloc(sizeof(double));
    getbuf((char *)v1, sizeof(double));
    if ( tpenc )
    {
        Dest = (double *)mmalloc(sizeof(double));
        domtox((char *)Dest, (char *)v1, sizeof(double), TDFtype);	// 8 bit double value
        mfree_0(v1);
        return Dest;
    }
    else
    {
        return v1;
    }
}

float getfloat(short TDFtype)
{
	float	Dest;
	float	ptr;
	
    getbuf((char *)&ptr, sizeof(float));
    if ( tpenc )
    {
        domtox((char *)&Dest, (char *)&ptr, sizeof(float), TDFtype);		// 4 bit float value
        return Dest;
    }
    else
		return ptr;
}

void getstrarr(CLStrArray *clst)
{
    char	*ptr;
	int		StringSpace;
	int		i;
	size_t	v4;
	
    StringSpace			= getlong();			// 4 bytes of string table size
    clst->StrTableSize	= StringSpace;
    clst->StrMaxSize	= StringSpace;			// shouldn't grow as all string space pre-allocated?
    ptr = (char *)mmalloc(StringSpace);
    clst->StringTable	= ptr;
    for ( i = clst->StrTableSize; i; i -= v4 )
    {
        v4 = 15000;								// Read max 15000 byte chunks at a time
        if ( i < 15001 )
            v4 = i;
        getbuf(ptr, v4);
        ptr += v4;
    }
}

char *_gettext(int size)
{
    char *v1;
	
    v1 = (char *)mmalloc(size + 1);				// zero memory area too
    getbuf(v1, size);
    return v1;
}

XTAB *getxtabs(int NumXTABs)	// screent accept field defs
{
    XTAB	*xtb;
	XTAB	*ptr;
	int		i;
	
// printf("getxtabs : sizeof(XTAB) = %d\n",sizeof(XTAB));  // 52
    ptr = 0;
    if ( NumXTABs ) // optional table. Don't always need accept fields
    {
        ptr = (XTAB *)mmalloc( sizeof(XTAB) * NumXTABs);	// 52
        xtb = ptr;
        if ( tpenc )
        {
            for ( i = 0; i < NumXTABs; xtb++ )
            {
                xtb->Flags			= getbits();
                xtb->Attr			= getbits();
                xtb->ScrAttrib		= getbits();
                xtb->field_32		= getbits();
                xtb->FLDnum			= getcount();
                xtb->Width			= getcount();
                xtb->width			= getcount();
                xtb->PCol			= getcount();
                xtb->PLine			= getcount();
                xtb->ACol			= getcount();
                xtb->ALine			= getcount();
                xtb->C_X			= getcount();
                xtb->C_Y			= getcount();
                xtb->onesc			= (ONESC *)(int)getcount();	// adjusted by caller
                xtb->RangeID		= getucount();
                xtb->VarExpNo		= getucount();
                xtb->WidthEXP		= getucount();
                xtb->PCol_exp		= getucount();
                xtb->PLine_exp		= getucount();
                xtb->ACol_exp		= getucount();
                xtb->ALine_exp		= getucount();
                xtb->Prompt_exp		= getucount();
                i++;
            }
        }
        else
        {
            getbuf((char *)xtb, sizeof(XTAB) * NumXTABs);	// 52
        }
    }
    return ptr;
}

RTAB *getrtabs(int NumRTABs)
{
    RTAB	*rtb;
	RTAB	*ptr;
	unsigned int i;
	
//printf("getrtabs : sizeof(RTAB) = %d\n",sizeof(RTAB));  // 20
	ptr = 0;
    if ( NumRTABs )
    {
        rtb = (RTAB *)mmalloc(sizeof(RTAB) * NumRTABs);
        ptr = rtb;
        if ( tpenc )
        {
            for ( i = 0; i < NumRTABs; ++rtb )
            {
                rtb->LockType	= getbits();
                rtb->OpType		= getbits();
                rtb->WhereEXP	= getucount();
                rtb->TTno		= gettable();
                rtb->enleft		= getucount();
                rtb->enright	= getucount();
                rtb->NextRT		= getucount();
                ++i;
            }
        }
        else
        {
            getbuf((char *)rtb, sizeof(RTAB) * NumRTABs);
        }
    }
    return ptr;	// Error exit
}

RATAB *getratabs(int NumRATABs)	// Range specifiers
{
    RATAB	*ratb;
	RATAB	*ptr;
	unsigned int i;
	
	ptr = 0;
	if ( NumRATABs )
    {
        ratb = (RATAB *)mmalloc(sizeof(RATAB) * NumRATABs);  // 12
        ptr = ratb;
        if ( tpenc )
        {
            for ( i = 0; i < NumRATABs; ratb++ )
            {
                ratb->RangeType		= getcount();
                ratb->RangeFrom		= getucount();
                ratb->RangeTo		= getucount();
                ratb->StringOffset	= getucount();
                ++i;
            }
        }
        else
        {
            getbuf((char *)ratb, sizeof(RATAB) * NumRATABs);  // 12
        }
    }
    return ptr;
}

XFTAB *getxftabs(int NumXFTABs)
{
    XFTAB	*xftb; // esi@2
    XFTAB	*ptr; // [sp+8h] [bp-10h]@2
    int		i; // edi@3

//printf("getxftabs : sizeof(XFTAB) = %d\n",sizeof(XFTAB));  // 8
    ptr = 0;
    if ( NumXFTABs )	// optional tables
    {
        ptr = (XFTAB *)mmalloc( sizeof(XFTAB) * NumXFTABs);
        xftb = ptr;
        if ( tpenc )
        {
            for ( i = 0; i < NumXFTABs; ++xftb )
            {
                xftb->TTno_to	= gettable();
                xftb->TTno_from = gettable();
                ++i;
            }
        }
        else
        {
            getbuf((char *)xftb, sizeof(XFTAB) * NumXFTABs);
        }
    }
    return ptr;
}

SFTAB *getsftabs(int NumSFTABs)
{
    SFTAB	*sftb;
	SFTAB	*ptr;
	unsigned int i;
	
//printf("getsftabs : sizeof(SFTAB) = %d\n",sizeof(SFTAB));  // 8
    ptr = 0;
    if ( NumSFTABs )	// optional table
    {
        ptr = (SFTAB *)mmalloc( sizeof(SFTAB) * NumSFTABs);
        sftb = ptr;
        if ( tpenc )
        {
            for ( i = 0; i < NumSFTABs; ++sftb )
            {
                sftb->cmd_flags		= getucount();	// command flag bits
                sftb->PRTno			= getucount();	// PRTAB no. describing variable to be read/written
                sftb->AliasEnt		= getucount();	// Expression number describing Aliasname
                sftb->FileNameEnt	= getucount();	// Expression number describing filename
                ++i;
            }
        }
        else
        {
            getbuf((char *)sftb, sizeof(SFTAB) * NumSFTABs);
        }
    }
    return ptr;
}

PRTAB *getprtabs(int NumPRTABs)
{
    PRTAB	*prtb;
	PRTAB	*ptr;
	unsigned int i;
	
//printf("getprtabs : sizeof(PRTAB) = %d\n",sizeof(PRTAB));  // 10
	ptr = 0;
    if ( NumPRTABs )	// optional table
    {
        ptr = (PRTAB *)mmalloc(sizeof(PRTAB) * NumPRTABs);
        prtb = ptr;
        if ( tpenc )
        {
            for ( i = 0; i < NumPRTABs; ++prtb )
            {
                prtb->VAR_ENTno		= getucount();	// expression describing variable we are outputting
                prtb->WIDTH_ENTno	= getucount();	// expression 
                prtb->WIDTH_FLAGS	= getcount();
                prtb->ReservedFlags = getcount();
                prtb->NextPR		= getucount();
                ++i;
            }
        }
        else
        {
            getbuf((char *)prtb, sizeof(PRTAB) * NumPRTABs);
        }
    }
    return ptr;
}

RDTAB *getrdtabs(int NumRDTABs)
{
    RDTAB *rdtb; // esi@2
    unsigned int i; // edi@3
    RDTAB *ptr; // [sp+8h] [bp-10h]@2

//printf("getrdtabs : sizeof(RDTAB) = %d\n",sizeof(RDTAB));  // 12
    ptr = 0;
    if ( NumRDTABs )	// optional table
    {
        ptr = (RDTAB *)mmalloc( sizeof(RDTAB) * NumRDTABs);
        rdtb = ptr;
        if ( tpenc )
        {
            for ( i = 0; i < NumRDTABs; ++rdtb )
            {
                rdtb->RDTFlags	= getcount();
                rdtb->ENTno		= getucount();
                rdtb->TTno		= gettable();
                rdtb->NextRD	= getucount();
                ++i;
            }
        }
        else
        {
            getbuf((char *)rdtb, sizeof(RDTAB) * NumRDTABs);
        }
    }
    return ptr;
}

KXTAB *getkxtabs(int NumKxtabs)
{
    KXTAB	*kxtb; // esi@2
    KXTAB	*ptr; // [sp+18h] [bp-10h]@2
    unsigned int i; // edi@3
    //char	*v5; // eax@4
    //short v4; // ax@4

//printf("getkxtabs : sizeof(KXTAB) = %d\n",sizeof(KXTAB));  // 64
    ptr = 0;
    if ( NumKxtabs )
    {
        ptr = (KXTAB *)mmalloc(NumKxtabs * sizeof(KXTAB)); //  == 64
        kxtb = ptr;
        if ( tpenc )
        {
            for ( i = 0; i < NumKxtabs; i++, kxtb++)
            {
                kxtb->KXFlags		= getbits();
                kxtb->WhereEXP		= getucount();
                kxtb->NumTTs		= getcount();
                kxtb->NumKeyFields	= getcount();
                kxtb->NumDataFields = getcount();
                kxtb->KeySize		= getcount();
                kxtb->RecSize		= getcount();
                kxtb->TTno			= gettable();
                ////v4 = getcount(); // variable length block name
                ////v5 = _gettext(v4);
                //v5 = _gettext(getcount());				
                //cdbcpystr(kxtb->TableName, v5, 0);
                cdbcpystr(kxtb->TableName, _gettext(getcount()), 0);	// variable length block name
            }
        }
        else
        {
            getbuf((char *)kxtb, NumKxtabs * sizeof(KXTAB)); //  == 64
        }
    }
    return ptr;
}

PTAB *getptabs(unsigned int NumPTABs)
{
    PTAB *ptb; // esi@2
    unsigned int i; // edi@3
    PTAB *ptr; // [sp+8h] [bp-10h]@2

//printf("getptabs : sizeof(PTAB) = %d\n",sizeof(PTAB));  // 8

	ptr = 0;
    if ( NumPTABs ) // optional, but makes no sense to have a program with no statements :o( ??
    {
        ptr = (PTAB *)mmalloc(sizeof(PTAB) * NumPTABs);
        ptb = ptr;
        if ( tpenc )
        {
            for ( i = 0; i < NumPTABs; ptb++, i++)
            {
                ptb->SrcLineNo	= getucount();
                ptb->OpCode		= getcount();
                ptb->Operand	= getucount();
                ptb->TABno		= getucount();
            }
        }
        else
        {
            getbuf((char *)ptb, sizeof(PTAB) * NumPTABs);
        }
    }
    return ptr;
}

ENTAB *gettabs(unsigned int NumENTABs) // expressions table
{
    ENTAB	*entb;
    ENTAB	*ptr;
	int		count;
	
	ptr = 0;
    if ( NumENTABs )	// optional
    {
        ptr = (ENTAB *)mmalloc(sizeof(ENTAB) * NumENTABs);
        entb = ptr;
        if ( tpenc )
        {
            for ( count = 0; count < NumENTABs; entb++ )
            {
                entb->entype = getbits();
				
				if (extendDC)								// break compatiblity with real ENC file here!!
					entb->extOpCode = getbits();			// unused in real CL4 - used for extended opcodes in DC version

                switch ( entb->entype )
                {
					case 1:											// expression describing variable/table field
						entb->Enun.Enref.TTno	= getcount();
						entb->Enun.Enref.VarNum	= getcount();
						break;
					case 2:
						entb->Enun.Enop.Enoper	= getcount();		// expression describing function/assignment
						entb->Enun.Enop.RecNo	= getcount();
						break;
					case 4:											// float literal
						entb->Enun.float4 = getfloat(0x104u);
                        break;
                    case 8:											// long integer literal		** 8 bytes on X64, 4 on X86  **
						entb->Enun.long8 = getlong();
                        break;
					case 16:										// short string literal : 3 chars or less + \0
						getbuf(entb->Enun.char16, 4u);				//  could be 8 bytes long on X64
                        break;
				}
                entb->enleft	= getucount();						// all expression records are potentially a tree structure
                entb->enright	= getucount();
                count++;
            }
        }
        else
        {
            getbuf((char *)entb, sizeof(ENTAB) * NumENTABs);// non-portable enc file works with extended stuff by default!
        }
    }
    return ptr;	// ptr == start of ENTAB array
}

BTAB *getbtabs(unsigned int NumBtabs)
{
    BTAB	*btb; // esi@1
    BTAB	*ptr; // [sp+8h] [bp-10h]@1
    unsigned int i; // edi@2

	// always has to be at least one block in a program.

    ptr = (BTAB *)mmalloc(sizeof(BTAB) * (NumBtabs + 1));
    btb = ptr;
    if ( tpenc )
    {
        for ( i = 0; i < NumBtabs; btb++ )
        {
            btb->BTFlags	= getbool();				// int return value
            btb->EndLine	= getcount();
            btb->fmts		= getcount();
            btb->StartLine	= getcount();
            btb->TTno		= getcount();
            btb->Scrs		= getcount();
            btb->EXPR		= (EXPR *)(int)getcount();	// dummy values, all adjusted by caller
            btb->On_delete	= (ONESC *)(int)getcount();
            btb->On_exit	= (ONESC *)(int)getcount();
            btb->On_excep	= (ONESC *)(int)getcount();
            btb->On_entry	= (ONESC *)(int)getcount();
            getbuf(btb->Maintain, 7u);
            getbuf(btb->BlockName, 21u);
            i++;
        }
    }
    else
    {
        getbuf((char *)btb, sizeof(BTAB) * NumBtabs);
    }
    return ptr;
}

FLDdesc *getftabs(int NumVariables)
{
    FLDdesc *fld;
	FLDdesc *ptr;
	int		i;
	
	// not optional. Program always has system variables
    ptr = (FLDdesc *)mmalloc( sizeof(FLDdesc) * NumVariables);
    fld = ptr;
    if ( tpenc )
    {
        for ( i = 0; i < NumVariables; fld++ )
        {
            fld->TDFtype	= getbits();
            fld->FLDstat	= getbits();
            fld->FLDelemID	= getcount();		// not used in the normal way here!
            fld->FLDlen		= getcount();
            fld->FLDtype	= getcount();
            fld->field_15	= getcount();		// not used by anything!!
            i++;
        }
    }
    else
    {
        getbuf((char *)fld, sizeof(FLDdesc) * NumVariables);
    }
    return ptr;
}

size_t getbuf(char *ptr, size_t size)
{
    size_t result;

    *(int*)__errno_location() = 0;			// clear fread error ststus loc
    result = fread(ptr, 1u, size, fp);
    if ( result != size )
    {
        syserror("loadenc: fread wanted %u, got %u", size, result);
        mexit(1);
    }
    return result;
}

int loadenc(char *FileName)
{
	TDesc	*VarTTptr; // esi@61
    FLDdesc *fld; // edi@61
    char	*v2; // esi@1
    char	*EntFileName; // edi@1
    char	*EncFileName; // edi@1
    char	*DBname; // ST48_4@33
    char	*Alias; // eax@40
    KXTAB	*kxtb; // esi@75
    PTAB	*pt; // esi@77
    XTAB	*xt; // esi@81
    BTAB	*bt; // esi@102
    TDesc	*TTptr; // [sp+3Ch] [bp-FCh]@40
    char	*TableName; // [sp+40h] [bp-F8h]@33
    char	*finDBname; // [sp+48h] [bp-F0h]@33

    //short	NameLen; // ax@33
    //short	TabNameLen; // ax@33
    bool	locking;
	//short	TbNamelen; // ax@34
    short	bstat; // ax@35
    short	v17; // edi@46
    size_t	v21; // edx@64
    unsigned short NumPtabs; // ax@74
    short	count; // ax@74
    short	PTABno; // edx@76
    int		v37; // eax@87
    int		v38; // edi@90
    short	NumVariables; // [sp+4Ch] [bp-ECh]@61
    short	TDno; // [sp+58h] [bp-E0h]@32
    short	TDnob; // [sp+58h] [bp-E0h]@74

    unsigned short version;
	unsigned short EncVersion = 40404;
	unsigned short tp_ext;	// temp holder for tpenc and extendDC values
	bool	v59;

	short	xsent;
	short	xsenc;
	struct	stat64 EntStat;
	struct	stat64 EncStat;

    v2 = mstrcpy(chkpath(FileName, 0, "CLEPATH", 1, 256), 0);
    EncFileName = mstrcpy(v2, ".enc", 0);
    EntFileName = mstrcpy(v2, ".ent", 0);
    mfree_0(v2);

	/*xsent = __xstat64(_STAT_VER, EntFileName, &EntStat);
	xsenc = __xstat64(_STAT_VER, EncFileName, &EncStat);
	printf("loadenc: xsent = %d, xsenc = %d\n",xsent,xsenc);
	if (!xsent)
		printf("ent.st_mtime: x%08X \n" ,EntStat.st_mtime);
	if (!xsenc)
		printf("enc.st_mtime: x%08X \n" ,EncStat.st_mtime);
	*/
//=========================================================
	xsent = __xstat64(_STAT_VER, EntFileName, &EntStat);		// returns 0 on success, buffer filled
	v59 = true;
    do
    {
		xsenc = __xstat64(_STAT_VER, EncFileName, &EncStat);	// returns 0 on success, buffer filled
        if ( xsenc || EncVersion != 40404 )						// Enc not found, or Version incorrect. (EncVersion == 40404 on first pass through)
        {
            if ( !isclcomp )			// Initialised to 1 , never changed. Must be used for debugging
            {
                if ( xsenc )
                {
                    eprint("enc file not found - %s.enc\n", FileName);
                    return 1;
                }
                eprint("wrong version of enc file (%d)\n", EncVersion);
                return 2;
            }
            if ( xsent )				// No Ent file found
            {
                eprint("ent file not found - %s.ent\n", FileName);
                return 3;
            }
            if ( !compile(FileName) )
            {
                eprint("compile of %s.ent failed\n", FileName);
                return 4;
            }
        }
        else if ( !xsent && EncStat.st_mtime < EntStat.st_mtime && !compile(FileName)) // found ent, but ent modded *after* enc == force compile
        {
			eprint("Compile of %s.ent failed\n", FileName);
            return 4;
        }
        fp = fopen64(EncFileName, "r");
        if ( !fp )
        {
            eprint("can't open - %s\n", EncFileName);
            return 5;
        }
		// at this point we have the enc file. If version number is wrong, go around again and force compile

        version	= *(unsigned short*)_gettext(2u);		// first two bytes of file = Version Number = 0x9DD4 = 40404
		tp_ext	= *(unsigned short*)_gettext(2u);		// next two bytes specify if portable, or optimised for platform (1 = Portable, 0 = Opt)
														// ** at this point, we don't know which order they are in! (big/little endian)
		if (tp_ext > 255)								// true if *not* portable format
			tp_ext >>= 8;

		tpenc		= (tp_ext & 0x01) != 0;
		// **** DC Extension enable ***
		extendDC	= (tp_ext & 0x02) != 0;				// Are extended DC features in the file?(bit unused in real enc file, ignored by clenter)
		
		// **real** clenter cannot read an extended-portable file!!  Will BOMB!
        // ****************************				
		//extendDC = 0; //debug

		domtox((char *)&EncVersion, (char*)&version, 2, (tpenc ? 0x44u : 0));
		if ( EncVersion / 100u != 404 || (EncVersion%100) > 4u )   // looking for version 40404 or less
        {
            fclose(fp);
            if ( !isCGI )
                eprint("wrong version of enc file\n");
            if ( !isclcomp )
                return 2;
        }
        else
        {
            v59 = false;	// all is good. exit the loop and continue
        }
    }
    while ( v59 );

	mfree_0(EncFileName);
    mfree_0(EntFileName);

//---------------------------------------
    no_dtabs = 0;
    dtab	 = (DBase *)mmalloc(0);
    no_ttabs = getcount();                      // next two bytes == number of Tables that follow.

	if ( dbrename )
		dbarray = camake(dbrename, ',');		// split on comma:   DB1=DB2,DB3=DB4,etc.

    ttab = (TDesc *)mmalloc( sizeof(TDesc) * (no_ttabs + 3)); // no_ttabs * 80 + 3 system tables:  Tables, Fields, Locks 
    TDno = 0;
	while ( no_ttabs > TDno )					// It *is* possible to have CL4 program with no Tables access!!
    {
        DBname		= _gettext(getcount());		// DBName  \0 terminated, variable length
        TableName	= _gettext(getcount());		// TableName \0 terminated, variable length
        finDBname	= checkdbrename(DBname);	// actually replaces memory pointed to by *DBname

		locking = false;
        if ( cmpbuf(TableName, "locking", 8) )	// precedes tablename if "lock" on a DB table has been selected
        {
			//printf("loadenc: locking on TableName = %s \n",TableName);
			locking		= true;
            mfree_0(TableName);
            TableName	= _gettext(getcount());	// get actual table name, variable length
		}
        
		bstat = getbits();						// short TableStat
		// octal 0100 == hex 0x0040
		if (( locking && !(bstat & 0x0040)) || ( !locking && (bstat & 0x0040)))
			__assert_fail("(locking && (bstat & 0100)) || (!locking && !(bstat & 0100))", "loadenc.c", 894, "loadenc");
        //else if ( !locking && (bstat & 0x0040))
		//	__assert_fail("(locking && (bstat & 0100)) || (!locking && !(bstat & 0100))", "loadenc.c", 894, "loadenc");
        
		TTptr			= &ttab[TDno + 3];		// start at Table No#3
        TTptr->TDlocked = bstat & 0xFF7E;		// 1111-1111-0111-1110   - clear bits 0x80 and 0x01
        TTptr->TTrtd	= -1;					// table not loaded from DBase
        TTptr->TableCRC = getcount();			// short CRC of Tabledetails
        
		Alias = TableName;
		while (*Alias)
		{
			if ( *Alias == ',' )
			{
				*Alias++ = 0;	// terminate TableName, and bump Alias to next char
				break;
			}
			Alias++;
		}
		//printf("TableName = \"%s\", Alias = \"%s\"\n",TableName,Alias);
        mstrncpy(TTptr->TableAlias, 21, Alias,0);
        mstrncpy(TTptr->TableName, 21, TableName,0);
        v17 = 0;
        while ( no_dtabs > v17 )							// See if DBase containing this table is already open
        {
			if ( !strcmp(finDBname, dtab[v17].FullDBname) )
				break;
			v17++;
		}
        if ( no_dtabs == v17 )
        {
			newdtab();
			assert(v17 < no_dtabs);							// no_dtabs should have ++ed.
			dtab[v17].DBno = -1;
            dtab[v17].FullDBname = mstrcpy(finDBname, 0);
		}
        TTptr->DBnumber = v17;
        if ( !(TTptr->TDlocked & 0x0200) )
			TTptr->TDlocked |= 0x800u;
		mfree_0(finDBname);
        mfree_0(TableName);
        TDno++;
    }
//----------------------------
    no_ttabs += 3;											// +3 system tables: tables, fields, locks

	if ( dbarray )
		cafree(dbarray);
    if ( getcount() )
        oelist = loadoe();									// Load Escapes next. optional
//----------------------------

    VarTTptr			= ttab;
    NumVariables		= getcount();						// Variables follow. always exists, due to system variables
    VarTTptr->NumFields = NumVariables;
    fld					= getftabs(NumVariables);			// Variables all hang off Table 0 as normal Fields
    VarTTptr->TTfields	= fld;

	for (int CurrVar = 0; CurrVar < NumVariables; fld++, CurrVar++)	// not Possible to have no variables. Sysvars always present!
    {
        if ( fld->FLDstat & fld_ZERO )						// variable is set to NULL, allocate space for a value though
        {
            if ( fld->FLDtype == 'C' )
                v21 = (signed short)(fld->FLDlen + 1);		// string variable length + \0 termination byte.
            else
                v21 = sizeof(double);	// 8;				// all else get 8 bytes of storage space, each stored as (double)
            
			if ( fld->FLDelemID )							// Is this an Array variable? If so, ElemID contains array size
				fld->FLDname = (char *)mmalloc(v21 * fld->FLDelemID);	// Array vars hang of FLDname
			else
                fld->FLDdata = mmalloc(v21);				// allocate sufficient space for variable.
        }
		
		else if ( fld->FLDtype == 'C' )						// variable needs to be initialised from enc file.
		    fld->FLDdata = (void*)_gettext(fld->FLDlen);	// gettext and getdouble alloc required memory
		else												
		    fld->FLDdata = (void*)getdouble(fld->TDFtype);	// everything else stored internally as a double
    }

//-------------------------
	NumPtabs			= getucount();
//printf("loadenc NumPtabs = %d \n",NumPtabs);
    ptarr.NextElement	= NumPtabs;
    ptarr.MaxAvail		= NumPtabs;
    ptarr.TableAddr		= getptabs(NumPtabs);// Program steps

    count				= getucount();
//printf("loadenc Expressions = %d \n",count);
    enarr.NextElement	= count;
    enarr.MaxAvail		= count;
    enarr.TableAddr		= gettabs(count);     // ENTABs == Expressions

	count				= getcount();
//printf("loadenc Accept Fields = %d \n",count);
    xtarr.NextElement	= count;
    xtarr.MaxAvail		= count;
    xtarr.TableAddr		= getxtabs((unsigned short)count);// Accept fields definitions
    
	count				= getcount();
//printf("loadenc Record refs = %d \n",count);
    rtarr.NextElement	= count;
    rtarr.MaxAvail		= count;
    rtarr.TableAddr		= getrtabs((unsigned short)count);// record reference definitions
    
	count				= getcount();
//printf("loadenc Ranges Defs = %d \n",count);
    raarr.NextElement	= count;
    raarr.MaxAvail		= count;
    raarr.TableAddr		= getratabs((unsigned short)count);// range definitions
    
	count				= getcount();
//printf("loadenc Xfers = %d \n",count);
    xfarr.NextElement	= count;
    xfarr.MaxAvail		= count;
    xfarr.TableAddr		= getxftabs((unsigned short)count);// XFTAB = Xfer.   Copy definitions
    
	count				= getcount();
//printf("loadenc FCB's = %d \n",count);
    sfarr.NextElement	= count;
    sfarr.MaxAvail		= count;
    sfarr.TableAddr		= getsftabs((unsigned short)count);// FCBs :  Files we open/read/write
    
	count				= getcount();
//printf("loadenc Print Defs = %d \n",count);
    prarr.NextElement	= count;
    prarr.MaxAvail		= count;
    prarr.TableAddr		= getprtabs((unsigned short)count);// Print statement definitions
    
	count				= getcount();
//printf("loadenc READ defs = %d \n",count);
    rdarr.NextElement	= count;
    rdarr.MaxAvail		= count;
    rdarr.TableAddr		= getrdtabs((unsigned short)count);// Read statement definitions
//------------------------
// KXTABs follow
    count				= getcount();
//printf("loadenc Create Statements = %d \n",count);
    kxarr.NextElement	= count;
    kxarr.MaxAvail		= count;
    kxarr.TableAddr		= getkxtabs((unsigned short)count);// Create statement definitions
    TDnob = 0;
    while ( kxarr.NextElement > TDnob )
    {
		kxtb				= &kxarr.TableAddr[TDnob];
        kxtb->NumKeyFields	= getcount();
        kxtb->KeyElemIDs	= getarrcount(kxtb->NumKeyFields);
        kxtb->TkeyTy		= getarrcount(kxtb->NumKeyFields);
        kxtb->NumDataFields	= getcount();
        kxtb->DataElemIDs	= getarrcount(kxtb->NumDataFields);
        kxtb->NumTTs		= getcount();
        kxtb->TTlist		= getarrtds(kxtb->NumTTs);
        TDnob++;
    }
//---------------------
    getstrarr(&strarr);                         // Load string table

//---------------------
    TDnob = 0;
    for ( PTABno = (unsigned short)getptabp(0); pt = PTARR(PTABno), pt->OpCode ; PTABno = (unsigned short)getptabp(TDnob) )
    {
        //pt = PTABno ? &ptarr.TableAddr[PTABno - 1] : 0;
		//pt = PTARR(PTABno);
		//printf("loadenc [305]: PTno = %3d, OpCode = %3d, pt->TABno = %3d\n",PTABno,pt->OpCode,pt->TABno);

		//if ( !pt->OpCode )
        //    break;
		if ( (unsigned short)(pt->OpCode - 1) > 48u )
		{
			if (pt->TABno)
			{
				v37 = pt->OpCode;
				if ( v37 == 250 || v37 == 260 )     // 250 = refresh, 260 = screen
				{
					v38		  = getcount();			// get a "screen_name" here
					pt->TABno = getstrmem(v38);     // Offset into string table area
					getbuf(STRARR(pt->TABno), v38);
				}
				else if ( (unsigned short)(pt->OpCode - 650) <= 48u )
				{
					pt->TABno = getstrmem(20);      // get a block_name here, max 20chars
					getbuf(STRARR(pt->TABno), 20);
				}
			}
		}
		else
		{
			//printf(" loadenc [1302]: pt->TABno = %d\n",pt->TABno);
			xt = XTARR(pt->TABno);
			if ( xt->onesc )
				xt->onesc = loadoe();
			if ( xt->Width )
				xt->field_20 = (char *)mmalloc(xt->Width);	// allocate sufficient mem for the accept field to process. Appears unused?
		}
        TDnob++;
    }

//----------------------
    no_btabs	= getcount();					// Number of Blocks
    btab		= getbtabs(no_btabs);			// load block descriptors
    
	for ( bt = btab; bt->BlockName[0]; ++bt )   // followed by block related extra fields
    {
        bt->Screens = loadscr();                 // Screen definitions
        if ( bt->On_delete )
            bt->On_delete	= loadoe();			// Block escapes
        if ( bt->On_exit )
            bt->On_exit		= loadoe();
        if ( bt->On_excep )
            bt->On_excep	= loadoe();
        if ( bt->On_entry )
            bt->On_entry	= loadoe();
    }
    fclose(fp);									// all done
    return 0;
}

#endif
