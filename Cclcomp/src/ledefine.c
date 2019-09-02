#ifndef LEDEFINE_C
#define LEDEFINE_C

#include <stdio.h>
#include <stdlib.h>				// for strtod(), strtol()
#include <assert.h>

//unsigned short FPtypes[] = { 0x0000u, 0x00A4u, 0x0084u, 0x00E4u, 0x00C4u, 0x0124u, 0x0104u, 0x0164u, 0x0144, 0x01A4u};

int ledefine(bool local)
{
	FLDdesc	*fld;
	char	*v5;
	char	*v6;

	double	v18;
	int		v10;
	int		fno;
	int		VarNameLen;
	int		MemSize;
	short	ArraySize;

	bool	TypeError;
	bool	StringVar;
	bool	InitVar;
	char	VarType;

	char	s[16];
	char	nptr[256];
	char	VarName[56];		// possible buffer overrun on VarName
	char	v37[80];			// possible buffer overrun on v37 (ArraySize)
	 
	TypeError = false;
    symbol = getsym();
    
	while ( symbol != 930 )
    {
		if ( symbol != 2230 )	// not a table/record/variable name
        {
			loaderr(8, sym);	// "invalid field name or number"
		    return 0;
        }
        if ( local )
			sprintf(s, "#%d", cur_block);	// local var, append #{BlockNo} to differentiate
        else
			s[0] = 0;
        
		cdbcpystr(VarName, sym, s, 0);		// *** buffer overrun is possible here !! ***
        StringVar	= false;
        InitVar		= false;
        
		// If variable type not specified, default to c20, non-array
		ArraySize	= 0;		
        VarType		= 'C';
        MemSize		= 20;

// We now have variable name, check for array type or "eq" <equate>
        symbol = getsym();
        if ( symbol == 740 )                  // "["  array 
        {
			symbol = getsym();
            cdbcpystr(v37, sym, 0);
            if ( symbol == 2200 )					// Numeric constant
            {
				ArraySize = strtol(sym, 0, 10);		// String to long, base 10
                symbol = getsym();
				if ( symbol == 1310 )				// "]" array sub terminator
					symbol = getsym();
                else
					ArraySize = 0;					// force error
			}
            if ( ArraySize <= 0 )					// error in subscript define
            {
				loaderr(31, v37);					// "array subscript must be +ve integer"
			    return 0;
			}
		}
        else if ( symbol == 1800 )					// "eq", "=" or "=="		indicates literal definition
        {
			symbol = getsym();
            cdbcpystr(nptr, sym, 0);				// save value for initialisation later

			switch (symbol)
			{
			case 2210:								// floating point literal
			    InitVar = true;
                v6 = strchr(nptr, '.');				// should always be true if symbol == 2210!
				//if ( v6 && lenstr(v6 + 1) == 20)	// ????????? ** error in real clcomp **
				//	MemSize = 9;
				//else
				//	MemSize = 2;
				if (v6 && (MemSize = lenstr(v6 + 1)))
				{
					if (MemSize < 2)
						MemSize = 2;
					else if (MemSize > 9)
						MemSize = 9;
				}
				else								// shouldn't be reachable, as token 2210 indicates a '.' was detected.
					MemSize = 2;
				VarType = MemSize + '0';	// '0' = 48
				break;
			case 2220:								// string literal
				StringVar	= true;
                VarType		= 'C';
                MemSize		= lenstr(nptr);
				break;
			case 2200:								// integer literal
				InitVar		= true;
                VarType		= 'N';
				break;
			default:
 			   	loaderr(32, VarName);				// "constant definition"
				return 0;							// error_exit
				break;
			}
            symbol = getsym();
		}

// this allows "variable = 65.5,c30".   Creates a C30 variable, but doesn't initialise it.

//		Normal variable definition. Look for separator and vartype
        if ( symbol == 180 )				// ","
        {
			symbol = getsym();
			//printf("ledefine #124: VarName = %s, sym = %s, syml = %d\n",VarName, sym, syml);
			//       !a-z       &&      !$       &&     ( !0-9 || not single digit)
            if ( !isalpha(sym[0]) && sym[0] != '$' && (!isdigit(sym[0]) || syml != 1) )
            {
 				loaderr(4, sym);			// "invalid data type"
				return 0;
            }
            if ( islower(sym[0]))			// ensure VarType is UpperCase
				VarType = toupper(sym[0]);
			else
				VarType = sym[0];

			if ( VarType == '$' )			// re-map $/F VarTypes
				VarType = '2';
			else if (VarType == 'F')
				VarType = '6';
											// dctypes includes 'X' for 4 digit HEX value
			v5 = alltypes;					// check for a match on vartype, alltypes[] == "BCDFGILNRTX$123456789"
            while ( *v5 && *v5 != VarType)
				v5++;
			if (!*v5)
	            TypeError = true;			// Error. Reached end of Alltypes array with no match
			else if (VarType == 'C')
			{
				v6 = &sym[1];				// look for char string length bytes
				if ( *v6 )
				{
					if ( !isdigit(*v6) || (MemSize = strtol(v6, 0, 10), MemSize == 0) )
						TypeError = true;
				}
				syml = 1;					// avoid second test below
			}
			if ( TypeError || syml != 1 )	// non-string variable, syml should always be 1 char long
			{
				loaderr(4, sym);	// "invalid data type"
				return 0;
			}
			symbol = getsym();		// exit_success
		}

//===============================================================================

		if (VarType != 'C')
			MemSize = sizeof(double);

		//printf("ledefine : 186 VarSize = %d, symbol = %d\n",MemSize, symbol);

		VarNameLen = lenstr(VarName);
        if ( VarNameLen > 55 )					// Max Variable Name Length = 56
        {
			VarName[55] = 0;
            VarNameLen = 55;
		}
        v10 = last_var;
        troot = gettnode(troot, VarName, VarNameLen + 1, 1);		// Add VarName to tree structure for easy location.
        if ( v10 == last_var )										// if last_var didn't change, variable already existed!
		{
			loaderr(17, VarName);				// "variable redefined"
		    return 0;
		}
		fno = last_fnd;
		
		assert(getvars(fno) == ((void *)0));	// assert that variable doesn't exist
		fld = getvarmem();						// allocate sapce for it.
		assert(getvars(fno) == fld);			// assert that the variable we created is now valid

		fld->FLDname = getmem(VarNameLen + 1);	// variable name
        cdbcpystr(fld->FLDname, VarName, 0);
		switch ( VarType )						// calculate vartype
		{
			case 'B':							// Integer types, Byte (8bit), Int (16bit), Long (32bit), LongLong (64bit)
				fld->FLDlen		= 1;
                fld->TDFtype	= 0x0044u;
                break;
            case 'C':							// String
                fld->FLDlen		= MemSize;
                fld->TDFtype	= 0x001Au;		// variable length flag (0x0002) set
                break;
            case 'D':							// Date
                fld->FLDlen		= 4;
                fld->TDFtype	= 0x1040u;		// only allow unsigned values
                break;
			case 'G':							// Dollar values
                fld->FLDlen		= 4;
                fld->TDFtype	= 0x0404u;
                break;
			case 'I':
                fld->FLDlen		= 2;
                fld->TDFtype	= 0x0044u;
                break;
            case 'L':							
                fld->FLDlen		= 8;
                fld->TDFtype	= 0x0044u;
                break;
            case 'N':
                fld->FLDlen		= 4;
                fld->TDFtype	= 0x0044u;
                break;
            case 'R':
                fld->FLDlen		= 4;
                fld->TDFtype	= 0x0604u;
                break;
            case 'T':							// Time
                fld->FLDlen		= 4;
                fld->TDFtype	= 0x0800u;		// only allow unsigned values
                break;
            case 'X':							// DC extension. 4 byte HEX value. Stored as N internally, but only unsigned!.
			case 'P':							// Pointer variable for function calls
                fld->FLDlen		= 4;
                fld->TDFtype	= 0x0040u;
                break;
            default:							// Floating point types
                fld->FLDlen		= 8;
				fld->TDFtype	= FPtypes[(VarType - '0')];
				break;
		}
		fld->FLDtype = VarType;
		fld->FLDdata = getmem(VarType == 'C' ? MemSize + 1 : MemSize ); // allow for terminating \0 in String type

// ======  save the init value  ======

		// default to variable == not initialised, and has ZERO value
		fld->FLDstat |= fld_ZERO;				// 0x0004 Variable is not initialised. So set quick zero flag.

		if (StringVar)
		{
			cdbcpystr((char *)fld->FLDdata, nptr, 0);
			if ( nptr[0] )
		        fld->FLDstat &= ~fld_ZERO;		// 0xFFFB clear zero flag if string present
		}
        else if ( InitVar )
        {
            v18 = strtod(nptr, 0);
            if ( v18 != 0.0 )
				fld->FLDstat &= ~fld_ZERO;		// 0xFFFB clear zero flag if required.

            *(double *)fld->FLDdata = v18;
        }

		if ( ArraySize )						// Use elemID as ArraySize, since unused for local vars
			fld->FLDelemID = ArraySize;

	} // end while( symbol != 930 )
	return 1;		// exit_success
}

#endif
