#ifndef CALCULATE_C
#define CALCULATE_C

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <math.h>		// for various math functions
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

double clatof(const char *nptr)
{
    double result;

    result = strtod(nptr, 0);
	return finite(result) ? result : 0.0 ;
}

void calculate(ENTAB *ent)
{
    FLDdesc *fld_src;
	FLDdesc *fld_dest;
	
    char *v36;
	char *v47;
	
    double v45;
    
	// range check vars
	double	MaxVal;
    double	MinVal;
    double	TestVal;

    unsigned short OpCode;
    unsigned short OpCodeMap;
   
	int		FLDlen;
    int		TTno;

    short	v37;
    short	fno;					// Table field number, or variable number
    char	FLDtype;
    char	DestType;
    
	char	a2[1008];
    char	Src[1008];
    char	result[1001];
    
	FLDdesc fld1;
    FLDdesc fld2;
																// true for xD800 - xF000 Math+assign operators (Updates TTptr area)
	fno = (short)gettdfno(ENARR(ent->enleft), &TTno, &FLDtype,(unsigned short)(ent->TTno & 0xFC00) - 0xD401u <= 0x1BFF);

	assert(ent->entype == 0x02);	// quick sanity checks
	assert(fno >= 0);
    
	// Should only be possible when LVALUE and *ALL* RVALUE(S) are all integer types
	//if (ent->TTno & 0x0200)
	//	printf("calculate: Integer Calc flag detected\n");

	fld_dest = &ttab[TTno].TTfields[fno];
    fld_src = getftf(ENARR(ent->enright), 0, &fld1);

    if ( fld_dest->FLDtype == 'C' )			// String destination routines
    {
		//printf("calculate : String Routine [dest = C]\n");
        sv_wid = fld_dest->FLDlen;
        zap(result, 1001u);
        evalstr(ENARR(ent->enright), result);
        
		assert(fld_dest->FLDlen < sizeof(result));			// shouldn't be possible? clcomp won't allow strings > 1000 bytes in length

		OpCode = ent->TTno & 0xFC00;
		switch (OpCode)
		{					// math+assignment operators
		case f_ADDEQ:		//	+=
		case f_SUBEQ:		//	-=
		case f_MULEQ:		//	*=
		case f_DIVEQ:		//	/=
		case f_MODEQ:		//	%=
		case f_POWEQ:		//	**=
			cdbcpystr(a2, (char *)fld_dest->FLDdata, 0);	// We need the existing contents of the destination variable!
			switch (OpCode)				// remapping math+assign to straight math operators
			{
			case f_ADDEQ:					// '+='  -> '+'
				OpCodeMap = f_ADD;				// Addition
				break;
			case f_SUBEQ:					// '-=' -> '-'
				OpCodeMap = f_SUB;				// Subtraction
				break;
			case f_MULEQ:					// '*=' -> '*'
				OpCodeMap = f_MUL;				// Multiply
				break;
			case f_DIVEQ:					// '/=' -> '/'
				OpCodeMap = f_DIV;				// Division
				break;
			case f_MODEQ:					// '%=' -> '%'
				OpCodeMap = f_MOD;				// MOD Operator
				break;
			case f_POWEQ:					// '**=' -> '**'
				OpCodeMap = f_POW;				// Exponent
				break;
			//default:							// unreachable due to switch block above
			//	OpCodeMap = ent->TTno;			// default == don't remap opcode	*** should be unreachable ***
			//	break;
			}

			if ( !stroper(OpCodeMap, Src, a2, result) )	// stroper(OpCode, Dest, Src1, Src2)
				Src[0] = 0;

			Src[fld_dest->FLDlen] = 0;								// Ensure destination buffer isn't overrun!

			fld_dest = getftf(ENARR(ent->enleft), 0, &fld2);
			cdbcpystr((char *)fld_dest->FLDdata, Src, 0);
			break;

		case f_ASSIGN:							// '=' simple Assignment
			result[fld_dest->FLDlen] = 0;							// Ensure destination buffer isn't overrun!
            fld_dest = getftf(ENARR(ent->enleft), 0, &fld2);
			cdbcpystr((char *)fld_dest->FLDdata, result, 0);
			break;

		default :					// Invalid OpCode, Crash out!
			//__assert_fail("0", "calculate.c", 113, "calculate");
			assert(0);
			break;
        }

		if ( !TTno && fno == v_TAB )			// TTno == 0 if this is a system variable
        {
            setcvar(v_TAB, (char *)fld_dest->FLDdata);  // "TAB"
            TAB = *(char *)fld_dest->FLDdata;
        }
        if ( *(char *)fld_dest->FLDdata )		// Quick ZERO flag check
            fld_dest->FLDstat &= ~fld_ZERO;		// 0xFFFBu;
        else
            fld_dest->FLDstat |= fld_ZERO;		// 0x0004u;

    }
	else	//fld_dest->FLDtype != 'C' Numeric calcs follow
	{	
		sv_wid = 99;

		if ( ent->TTno & 0x0200 )				// Integer calc flag
		    v45 = (double)evalint(ENARR(ent->enright));
		else
			v45 = clround(evalnum(ENARR(ent->enright), fld_dest->FLDtype), fld_dest->FLDtype);

		fld_dest = getftf(ENARR(ent->enleft), 0, &fld2);	// get destination fld
		DestType = fld_dest->FLDtype;

		OpCode = ent->TTno & 0xFC00;
		if ( OpCode )							// Can OpCode ever be ZERO?
		{
			switch (OpCode)
			{
			case f_ADDEQ:																// '+=' Addition
				v45 = clround(*(double *)fld_dest->FLDdata + v45, DestType);
				break;
			case f_SUBEQ:																// '-=' Subtraction
			    v45 = clround(*(double *)fld_dest->FLDdata - v45, DestType);
				break;
			case f_MULEQ:																// '*=' Multiply + assign
				v45 = clround(v45 * *(double *)fld_dest->FLDdata, DestType);
				break;
			case f_DIVEQ:																// '/=' Division		(No check for /zero error?)
				v45 = clround(*(double *)fld_dest->FLDdata / v45, DestType);	
				break;
			case f_MODEQ:																// '%=' (MOD) operator
				v45 = clround(fmod(*(double *)fld_dest->FLDdata,v45), DestType);	
				break;
			case f_POWEQ:																// '**=' Exponent
				v45 = clround(power(*(double *)fld_dest->FLDdata, v45), DestType);
				break;
			case f_ASSIGN:																// simple assignment?
				//printf("calc - 0xA800 opcode encountered v45 = %G\n",v45);
				//*(double *)fld_dest->FLDdata = v45;	//*********************************************
				break;
			default:	// Somehow we reached an invalid OpCode!!
				//__assert_fail("0", "calculate.c", 176, "calculate");
				assert(0);
				break;
			}
			*(double *)fld_dest->FLDdata = v45;					// also handles the simple assignment case
		}

		if ( !TTno && fno == v_DATEFORM )    // "DATEFORM"
		    _DF = (short)*(double *)fld_dest->FLDdata;
    
		if ( v45 == 0.0 )				// set quick ZERO flag
		    fld_dest->FLDstat |= fld_ZERO;	// 0x0004u;
		else
			fld_dest->FLDstat &= ~fld_ZERO;	// 0xFFFBu;

		// special check for Dest Vartype = D, and Src = INT array of size 10
		if ( DestType == 'D' && fld_src && fld_src->FLDstat & fld_ARRAY && fld_src->FLDelemID == 10 && fld_src->FLDtype == 'N' )
			arrdate(fld_dest, fld_src);
	
		// Calc finished, do range limit checks if applicable ...
		if ( DestType == 'B' || DestType == 'I' || DestType == 'N' || DestType == 'L' )
		{    
			switch (DestType)
			{
			case 'B':
				MaxVal = 127.0;
				MinVal = -127.0;
				break;
			case 'I':
				MaxVal = 32767.0;
				MinVal = -32767.0;
				break;
			case 'L':
				MaxVal = 1.0e14;
				MinVal = -1.0e14;
				break;
			case 'N':
				MaxVal = 2147483647.0;
				MinVal = -2147483647.0;
				break;
			}
			TestVal = *(double *)fld_dest->FLDdata;
			if ( TestVal > MaxVal )
				*(double *)fld_dest->FLDdata = MaxVal;
			else if( TestVal < MinVal )
				*(double *)fld_dest->FLDdata = MinVal;
		}
	}


    if ( fld_dest->FLDstat & fld_ARRAY )	// fld_Array indicates no subcript (or [0]) specified. Copy value to *ALL* Array elements 
    {
		// special check for INT array size = 10, and vartype = D.
        if ( fld_src && fld_src->FLDtype == 'D' && fld_dest->FLDelemID == 10 && fld_dest->FLDtype == 'N' ) 
        {
            datearr(fld_dest);	// convert double value into array hanging off fld_dest->FLDname
        }
        else
        {
            if ( fld_dest->FLDtype == 'C' )
                FLDlen = (unsigned short)(fld_dest->FLDlen + 1);
            else
                FLDlen = sizeof(double);

            v47 = fld_dest->FLDname;			// Array variables hang of FLDname. 
            v36 = &v47[FLDlen];
            v37 = fld_dest->FLDelemID - 1;		// Number of Array elements
            if ( fld_dest->FLDelemID != 1 )
            {
                do
                {
                    cpybuf(v36, v47, FLDlen);	// copy value to all array elements!
                    v36 += FLDlen;
                    --v37;
                }
                while ( v37 );
            }
        }
    }
    else
    {
        if ( !TTno && fno == v_INTERRUPT )// "INTERRUPT"
            cl_intr(0);
    }
    fld_dest->FLDstat |= 0x0018u;	// (fld_DATA_AVAIL|fld_SUBREC) ???   fld halds data, and value is *dirty* (may need flush to disk)??
}

double evalnum(ENTAB *entab, int VarType)
{
	FLDdesc	*fld;
    unsigned short OpCode;
	
	double	val_right;
	double	val_left;
	
	FLDdesc fld1;
	FLDdesc fld2;
	
	char	nptr[8192];
	char	buff[1024];
	char	src[21];
	char	FLDtype;

    if ( !entab )
	    return 0.0;

	//printf("evalnum : entab->entype = %d, entab->TTno  = x%04X\n",entab->entype,(unsigned short)entab->TTno);
	//if (entab->TTno & 0x0200)
	//	printf("evalnum: Integer Calc flag detected\n");

	if ( entab->entype == 0x02 )
	{
		if ( entab->TTno & 0x0200 )			// 0x0200 == Integer calc flag
			return (double)evalint(entab);
		
		if ( entab->TTno & 0x0100 )			// 0x0100 == Reserved name flag
			return (double)getnresv(entab->RecNo);

		OpCode = entab->TTno & 0xFC00;		// 1111 11-- ---- ----

		switch (OpCode)
		{
		case f_ARRSTART:					// '[' Array operation
			goto LABEL_128;

		case f_RANDOM:
			return drand48();				// random()

		case f_KEYREADY:
			return (double)Tty_KeyIsReady();

		case f_UCASE :						// ucase()			// all string operations?
		case f_LCASE :						// lcase()
		case f_GETARG:						// getarg()
		case f_GETENV:						// getenv()
		case f_SDEL  :						// sdel()
		case f_SMULT :						// smult()
		case f_SWORD :						// sword()
		case f_GETXML:						// getxml()
		case f_TRIM  :						// trim()
		case f_GETCGI:						// getcgi()
		case f_SUBSTR:						// substr()
		case f_SREP  :						// srep()
		case f_SCONV :						// sconv()
		case f_SCAT  :						// scat()
			evalstr(entab, buff);
			
			if ( VarType == 'D' )
				return (double)clgetdate(buff);		// returns int
			else if ( VarType == 'T' )
				return gettime(buff);				// returns double anyway
			else
				return clatof(buff);				// try and interpret result as a number
		}
		
		// Next batch of Opcodes only require Left parameter:
		val_left  = evalnum(ENARR(entab->enleft), VarType);
		//printf("evalnum : val_left = %G\n",val_left);
		switch (OpCode)
		{
		case f_SQRT :
			return val_left > 0.0 ? sqrt(val_left) : 0.0;
		case f_LOG  :
			return val_left > 0.0 ? log(val_left) : 0.0;
		case f_SIN  :
			return sin(val_left);
		case f_COS  :
			return cos(val_left);
		case f_TAN  :
			return tan(val_left);
		case f_LOG10:
			return val_left > 0.0 ? log10(val_left) : 0.0;
		case f_ASIN :
			return val_left <= 1.0 ? asin(val_left) : 0.0;
		case f_ACOS :
			return val_left <= 1.0 ? acos(val_left) : 0.0;
		case f_ATAN :
			return atan(val_left);
		case f_DEGREES:									// degrees()
			return (val_left > 0.0) ? (val_left * 180.0 / 3.141592653589793) : 0.0;
		case f_RADIANS:									// radians()
			return (val_left > 0.0) ? (val_left / 180.0 * 3.141592653589793) : 0.0;
		case f_EXP  :									// exp() operator
			return exp(val_left);
		}
		
		// Remaining batch of Opcodes require Left *and* Right parameters. (Left evaluated above)
		val_right = evalnum(ENARR(entab->enright), VarType);
		//printf("evalnum : val_right = %G\n",val_right);
		switch (OpCode)
		{
		case f_ADD  :									// + add operator
			return val_left + val_right;
		case f_SUB  :									// - subtract operator
			return (entab->enright) ? val_left - val_right : -val_left;
		case f_POW  :									// ** exponent operator
			return power(val_left, val_right);
		case f_MUL  :									// * multiply operator
			return val_left * val_right;
		case f_DIV  :									// "/" divide operator
			return (val_right != 0.0) ? val_left / val_right : 0.0 ;	// avoid divide by zero error
		case f_MOD  :									// % MOD operator
			fld = getftf(ENARR(entab->enleft), 0, &fld2);
			FLDtype = fld ? fld->FLDtype : 'N' ;			// default to Integer
			if ( FLDtype == 'C' )
			{
				evalstr(entab, nptr);
				return (VarType == 'D') ? (double)clgetdate(nptr): clatof(nptr);
			}
			else
			{
				val_left = clround(val_left, FLDtype);
				fld = getftf(ENARR(entab->enright), 0, &fld1);

				val_right = clround(val_right, fld ? fld->FLDtype : 'N');	// default to 'N' long integer
				return (val_right != 0.0) ? fmod(val_left,val_right) : 0.0;
			}
		}
	}

LABEL_128:
//printf("evalnum : Label128 default handler, type = %d\n", entab->entype);

	fld = getftf(entab, 1, &fld2);
	if ( fld )
	{
		if ( fld->FLDtype == 'C' )						// Src == String
		{
			if ( VarType == 'D' || VarType == 'T' )		// Dest == Date/Time. Try to interpret string accordingly
			{
				strncpy(src,(char *)fld->FLDdata, 20u);
				src[20] = 0;
				return (VarType == 'D') ? (double)clgetdate(src) : gettime(src);
			}
			else
				return clatof((const char *)fld->FLDdata);	// try and interpret string as a number
		}
		else
			return *(double *)fld->FLDdata;				// all others stored internally as double

	}
	return 0.0;
}


int evalint(ENTAB *entab)
{
    ENTAB	*ent1;
	FLDdesc *fld;
    unsigned short OpCode;

    int		val_left;
    int		val_right;
    int		v9;
	int		TTno;

	FLDdesc fld_lit;	// used for temp storage of entab literals
	
	char	FLDtype;
    char	a1[1008];
    char	a2[1008];

    if ( !entab )
        return 0;

	//if (entab->TTno & 0x0200)
	//	printf("evalint: Integer Calc flag detected\n");

    if ( entab->entype == 0x02 )			
	{
		if ( entab->TTno & 0x0100 )											// reserved value flag, try to interpret as numeric value
			return getnresv(entab->RecNo);
		else if ( entab->TTno & 0x0200 )									// Integer calc flag
		{
			val_left = (int)evalint(ENARR(entab->enleft));
			val_right = (int)evalint(ENARR(entab->enright));
		}
		else
		{
			val_left = (int)evalnum(ENARR(entab->enleft),'N');
			val_right = (int)evalnum(ENARR(entab->enright),'N');
		}

		OpCode = entab->TTno & 0xFC00;
		switch (OpCode)
		{
		case f_ADD   :												// "+" operator. 
			return val_left + val_right;	
		case f_SUB   :												// "-" operator.
			return val_left - val_right;
		case f_POW   :												// "**" operator
			return (int)power((double)val_left, (double)val_right);
		case f_MUL   :												// "*" operator. 
			return val_left * val_right;
		case f_DIV   :												// "/" operator. Check divisor to avoid "divide by zero" error.
			return val_right ? val_left / val_right : 0 ;
		case f_MOD   :												// "%"  mod operator. Check divisor to avoid "divide by zero" error.
			return val_right ? val_left % val_right : 0 ;
		case f_MATCH :												// match()
			evalstr(ENARR(entab->enleft), a2);
			evalstr(ENARR(entab->enright), a1);
			return smatch(a1, a2);
		case f_RANDOM:												// random()		// not sure this is reachable as an integer RVALUE?
			return lrand48();
		case f_KEYREADY:											// keyready()
			v9 = Tty_KeyIsReady();
			return v9;
		case f_NAME  :												// name()
			a1[0] = 0;
			return isfname(evalstr(ENARR(entab->enleft),a1)) == 0;

		case f_ASC   :												// asc()
			a1[0] = 0;
			ent1 = ENARR(entab->enleft);	// drill down the left tree to the end
			while (ent1 && ent1->enleft)
				ent1 = ENARR(ent1->enleft);

			getftf(ent1, 0, &fld_lit);	// looks pointless, but sets global dbl_0, dbl_1 variables ?
			evalstr(ENARR(entab->enleft), a1);
			v9 = (unsigned char)a1[0];
			return v9;

		case f_SLEN  :												// slen()
			a1[0] = 0;
			ent1 = ENARR(entab->enleft);	// drill down the left tree to the end
			while (ent1 && ent1->enleft)
				ent1 = ENARR(ent1->enleft);

			fld = getftf(ent1, 0, &fld_lit);
			if ( fld && fld->FLDtype == 'C' )
				return lenstr(evalstr(ENARR(entab->enleft), a1));
			return 0;
			break;
		
		case f_ASIZE :												// asize()
			//printf("evalint asize()\n");
			ent1 = ENARR(entab->enleft);	// drill down the left tree to the end
			while (ent1 && ent1->enleft)
				ent1 = ENARR(ent1->enleft);

			val_right = gettdfno(ent1, &TTno, &FLDtype, 0);
			if ( TTno )									
				return ttab[TTno].NumFields - val_right;			// This is a table. return Numfields - current field no
			else		
				return ttab->TTfields[val_right].FLDelemID;			// Normal variable. return number of elements
		}
	}
	// printf("evalint 627 - entab->entype = %d\n",entab->entype);
	// default numeric handler. Will catch anything not handled above

	fld = getftf(entab, 1, &fld_lit);
	if ( fld && fld->FLDtype != 'C' )
		return (int)*(double *)fld->FLDdata;
	else
		return 0;
}

char *evalstr(ENTAB *entab, char *OutBuffer)
{
    ENTAB	*ent;
	FLDdesc *fld;
	char	*chr_ptr;
	char	*v63;
	char	*v65;
	char	*CBuf1;
	char	*CBuf2;
    char	*StrRight = 0; 
	char	*StrLeft = 0;
	double	num;						// for OpCodes which require a numeric parameter
    int		v60;
	unsigned short OpCode;
    short	v50;
    bool	doDefault = false;
    
	FLDdesc fld_lit;					// used as temp storage for entab literals

    char	dest[21];
    char	FLDtype;

    if ( !entab )
        return OutBuffer;				// Don't alter buffer, just return to caller

	if ( entab->entype != 0x02 )
	{
        if ( entab->entype == 0x10 )				// 3 char (or less) string stored directly in entab
			cdbcpystr(OutBuffer, (char *)&entab->TTno, 0);	// Simple copy operation
		else
            doDefault = true;			// fall through to default operation
	}
    else if ( entab->TTno & 0x0100 )	// get a reserved variable. Takes no params 
    {	// entab->entype == 2
        cdbcpystr(OutBuffer, getresv(entab->RecNo), 0);
    }
	else
	{
		OpCode = entab->TTno & 0xFC00;
		if ( OpCode - 0xC801u <= 0xBFF || OpCode - 0xF001u <= 0x7FF )
		{
			switch (OpCode)
			{
			case  f_SREP:		// srep("The string to look at", "string","new string") 
				StrLeft = (char *)mmalloc(1001u);
				evalstr(ENARR(entab->enleft), StrLeft);				// enleft = "The String to look at"        
				ent = ENARR(entab->enright);
				if ( ent->entype == 0x20 )							// function call type
				{
					CBuf1 = (char *)mmalloc(1001u);					// Old String to look for 
					CBuf2 = (char *)mmalloc(1001u);					// String to replace it with
					evalstr(ENARR(ent->enleft), CBuf1);				// *** recursion ***
					evalstr(ENARR(ent->enright), CBuf2);			// *** recursion ***
					repstr(OutBuffer, StrLeft, CBuf1, CBuf2);
					free(CBuf1);
					free(CBuf2);
					free(StrLeft);
					return OutBuffer;
				}
				__assert_fail("((((ce->enright) == 0 ? ((void *)0) : &enarr[0].enodes[((ce->enright)-1)]))->entype == 0x20)","calculate.c",669,"evalstr");
				break;
			case f_SUBSTR:		// substr("The string", start, length)
				StrLeft = (char *)mmalloc(1001u);
				evalstr(ENARR(entab->enleft), StrLeft);				// enleft contains the "The String"
				ent = ENARR(entab->enright);						// Params hang off enright
				if ( ent->entype == 0x20 )
				{
					int StartPos	= evalint(ENARR(ent->enleft));
					int Length		= evalint(ENARR(ent->enright));
					substr(OutBuffer, StrLeft, StartPos, Length);
					free(StrLeft);
					return OutBuffer;
				}
				__assert_fail("((((ce->enright) == 0 ? ((void *)0) : &enarr[0].enodes[((ce->enright)-1)]))->entype == 0x20)","calculate.c",638,"evalstr");
				break;
			case f_SCONV:														// sconv(url|soundex|tabsub, en|de|ign|unign|0|1-9, string3)
				StrLeft = (char *)mmalloc(1001u);
				evalstr(ENARR(entab->enleft), StrLeft);
				ent = ENARR(entab->enright);
				if ( ent->entype == 0x20 )		// function call expression. 
				{
					CBuf1 = (char *)mmalloc(1001u);
					CBuf2 = (char *)mmalloc(1001u);
					evalstr(ENARR(ent->enleft), CBuf1);
					evalstr(ENARR(ent->enright), CBuf2);
					cnvstr(OutBuffer, StrLeft, CBuf1, CBuf2);	// to do   sconv() function handler
					//cdbcpystr(OutBuffer, "function not enabled", 0);
					free(CBuf1);
					free(CBuf2);
					free(StrLeft);
					return OutBuffer;
				}
				__assert_fail("((((ce->enright) == 0 ? ((void *)0) : &enarr[0].enodes[((ce->enright)-1)]))->entype == 0x20)","calculate.c",651,"evalstr");
				break;

			case f_SCAT:		// scat("str",....) 
				StrLeft = (char *)mmalloc(1001u);
				evalstr(ENARR(entab->enleft), StrLeft);  // nice recursion
				cdbcpystr(OutBuffer, StrLeft, 0);										// First param is in enleft
				ent = ENARR(entab->enright);											// Remaining params are chained off enright
				while ( ent->entype == 0x20 )											// function call type expression
				{
					evalstr(ENARR(ent->enleft), StrLeft);	// nice recursion here
					concat(OutBuffer, StrLeft);
					ent = ENARR(ent->enright);				// chain extends down enright
				}
				evalstr(ent, StrLeft);
				concat(OutBuffer, StrLeft);
				free(StrLeft);
				return OutBuffer;
				break;

			default:
				*OutBuffer = 0;
				return OutBuffer;
				break;
			}
		}

		//set up StrRight, StrLeft for following routines
		if ( entab->TTno & 0xFDFF ) // 1111-1101-1111-1111	// mask out integer calc flag 0x0200
		{
			if ( entab->enleft )
			{
				StrLeft = (char *)mmalloc(1001u);
				evalstr(ENARR(entab->enleft), StrLeft);
			}
			if ( entab->enright )
			{
				StrRight = (char *)mmalloc(1001u);
				if ( OpCode == 0x6000 || OpCode == 0x6400 || OpCode == 0x6800 )			// '*' , '/' , '%'
					sprintf(StrRight, "%ld", evalint(ENARR(entab->enright)));
				else
					evalstr(ENARR(entab->enright), StrRight);
			}
		}

		switch (OpCode)
		{
		case f_GETENV:											// getenv()
			cdbcpystr(OutBuffer, getenv(StrLeft), 0);
			break;

		case f_ASC   :											// asc()			// **** CHECK THIS ****
		case f_SLEN  :											// slen()
			printf("evalstr asc/slen()\n");
			sprintf(OutBuffer, "%ld", evalint(entab));
			break;
		
		case f_TRIM  :											// trim()
			cdbcpystr(OutBuffer, ftrim(StrLeft), 0);			// ftrim == force trim. Ignores TRIM variable setting
			break;
		
		case f_MATCH :											// match()
			sprintf(OutBuffer, "%ld", smatch(StrRight, StrLeft));
			break;

		case f_SETENV:											// setenv()
			chr_ptr = mstrcpy(StrLeft, "=", StrRight, 0);
			putenv(chr_ptr);
			cdbcpystr(OutBuffer, chr_ptr, 0);
			break;

		case f_GETARG:											// getarg()
			cdbcpystr(OutBuffer, (char *)getarg(StrLeft), 0);
			break;

		case f_CHR   :											// chr()
			v50 = evalint(ENARR(entab->enleft));
			if ( v50 >= 0x100 )
				v50 &= 0xFF00;
			if (!v50)
			{
				OutBuffer[0] = '\\';									// *** need to check ***
				OutBuffer[1] = '0';
				OutBuffer[2] = 0;
			}
			else
			{
				OutBuffer[0] = v50;									// *** need to check ***
				OutBuffer[1] = 0;
			}
			break;

		case f_DAY   :											// day()
			fld = getftf(ENARR(entab->enleft), 1, &fld_lit);
			num = 0.0;
			if ( fld )
			{
				if ( fld->FLDtype == 'C' )
				{
					strncpy(dest, (const char *)fld->FLDdata, 20u);
					dest[20] = 0;
					num = (double)clgetdate(dest);
				}
				else
					num = *(double *)fld->FLDdata;
			}
			if ( num <= 0.0 )
				*OutBuffer = 0;		// invalid result, return blank string
			else
				cdbcpystr(OutBuffer, aday[(signed int)(double)num % 7], 0);
			break;

		case f_LCASE :											// lcase()
			chr_ptr = StrLeft;
			while ( *chr_ptr )
			{
				//v60 = *chr_ptr;
				//if (isupper(v60))
				//	*chr_ptr = tolower(v60);
				//chr_ptr++;
				*chr_ptr++ = tolower(*chr_ptr);
			}
			cdbcpystr(OutBuffer, StrLeft, 0);
			break;

		case f_UCASE:											// ucase()
			chr_ptr = StrLeft;
			while ( *chr_ptr )
			{
				//v60 = *chr_ptr;
				//if (islower(v60))
				//	*chr_ptr = toupper(v60);
				//chr_ptr++;
				*chr_ptr++ = toupper(*chr_ptr);
			}
			cdbcpystr(OutBuffer, StrLeft, 0);
			break;

		case f_GETCGI:											// getcgi()
			chr_ptr = cgiGetValue(StrLeft, sv_wid);				// sv_wid is some nasty globalness. should be passed into function
			trim(chr_ptr);
			if ( chr_ptr )
			{
				if ( strlen(chr_ptr) > 1000 )
				{
					strncpy(OutBuffer, chr_ptr, 1000u);
					OutBuffer[1000] = 0;
				}
				else
					cdbcpystr(OutBuffer, chr_ptr, 0);
			}
			else
				*OutBuffer = 0;
			break;

		case f_GETXML:											// getxml()		xmlGetValue
			v65 = "not enabled";
			//v65 = xmlGetValue(StrLeft, sv_wid);	// ** to do **
			v63 = OutBuffer;
			while ( v65 && *v65 && sv_wid-- )					// sv_wid is some nasty globalness. should be passed into function
				*v63++ = *v65++;
			*v63 = 0;
			break;

		case f_SFMT  :											// fmt()   *** real CL4 - doesn't handle numeric variables ***
			*OutBuffer = 0; // default return blank string
			fld = getftf(ENARR(entab->enleft), 1, &fld_lit);
			if ( fld )
			{
				//print ("fmt(fld true, FLDtype = %c,  StrRight = %s,  StrLeft = %s)\n", fld->FLDtype,  StrRight,  StrLeft);
				switch (fld->FLDtype)
				{
				case 'C':
					nfmt(OutBuffer, StrRight, clatof(StrLeft));
					break;
				case 'D':
					dfmt(OutBuffer, StrRight, (double)clgetdate(StrLeft));
					break;
				case 'N':															// *** non-standard extension ***
					sprintf(OutBuffer, StrRight, (int)evalint(ENARR(entab->enleft)));		// allows fmt(intval, '%04X') to return HEX formatted value
					printf("%s\n",OutBuffer);
					break;
				case 'T':
					tfmt(OutBuffer, StrRight, (double)gettime(ftrim(StrLeft)));
					break;
				}
			}
			else
			{
				ent = ENARR(entab->enleft);						// drill down the left tree to the end
				while (ent && ent->enleft)
					ent = ENARR(ent->enleft);
				fld = getftf(ent, 0, &fld_lit);

				FLDtype = fld ? fld->FLDtype : '9' ;			// default to max decimal places
				nfmt(OutBuffer, StrRight, clround(evalnum(ENARR(entab->enleft), FLDtype), FLDtype));
			}
			break;

		default:																	// remaining OpCodes require a numeric parameter to work:
			num = evalnum(ENARR(entab->enleft), 'F');
			switch (OpCode)
			{
			case f_SQRT :															// sqrt()
				sprintf(OutBuffer, "%.f",num > 0.0 ? sqrt(num) : 0.00 );
				break;
			case f_LOG  :															// log()
				sprintf(OutBuffer, "%.f",num > 0.0 ? log(num) : 0.00 );
				break;
			case f_SIN  :															// sin()
				sprintf(OutBuffer, "%.f", sin(num));
				break;
			case f_COS  :															// cos()
				sprintf(OutBuffer, "%.f", cos(num));
				break;
			case f_TAN  :															// tan()
				sprintf(OutBuffer, "%.f", tan(num));
				break;
			case f_LOG10:															// log10()
				sprintf(OutBuffer, "%.f",num > 0.0 ? log10(num) : 0.00 );
				break;
			case f_ASIN :															// asin()
				sprintf(OutBuffer, "%.f",num <= 1.0 ? (double)asin(num) : 0.0);
				break;
			case f_ACOS :															// acos()
				sprintf(OutBuffer, "%.f",num <= 1.0 ? (double)acos(num) : 0.0);
				break;
			case f_ATAN :															// atan()
				sprintf(OutBuffer, "%.f", (double)atan(num));
				break;
			case f_DEGREES:															// degrees()
				sprintf(OutBuffer, "%.f", num > 0.0 ? (num * 180.0 / 3.141592653589793): 0.0 );
				break;
			case f_RADIANS:															// radians()
				sprintf(OutBuffer, "%.f", num > 0.0 ? (num / 180.0 * 3.141592653589793): 0.0 );
				break;
			case f_EXP  :															// exp()
				sprintf(OutBuffer, "%.f",exp(num));
				break;
			default:
				doDefault = !stroper(entab->TTno, OutBuffer, StrLeft, StrRight);	// doDefault set true if stroper can't handle instruction
				break;
			}
			break;
		}
	}

    if ( doDefault )
    {
        fld = getftf(entab, true, &fld_lit);	// default handler. Just return the fld pointed to by the expression , update TT data if applicable
        if ( fld )
        {
			switch (fld->FLDtype)
			{
			case 'C':
                cdbcpystr(OutBuffer, (char *)fld->FLDdata, 0);
				break;
			case 'D':
				makedate(OutBuffer, *(double *)fld->FLDdata);
				break;
			case 'T':
				maketimehms(OutBuffer, *(double *)fld->FLDdata);
				break;
			case 'X':														// DC extension! 4 byte HEX value
				sprintf(OutBuffer, "x%04X",(int)*(double *)fld->FLDdata);
				break;
			default:
				sprintf(OutBuffer, "%.*f", typedp(fld->FLDtype),*(double *)fld->FLDdata);
				break;
			}
        }
        else
            *OutBuffer = 0;	// default == blank string
	}
    if ( StrLeft )
        free(StrLeft);
    if ( StrRight )
        free(StrRight);
    return OutBuffer;
}

void substr(char *Dest, char *Buffer, int Start, int MaxLen)
{
    short BuffLen;
	
    if ( Start > 0 && MaxLen >= 0 )
    {
        BuffLen = lenstr(Buffer);
        if ( MaxLen )
        {
            if ( Start <= BuffLen )
            {
                cpybuf(Dest, &Buffer[Start - 1], MaxLen);
                Dest[MaxLen] = 0;
            }
        }
        else
            cdbcpystr(Dest, &Buffer[Start - 1], 0);
    }
    else
        *Dest = 0;
}

void repstr(char *Dest, char *Src, const char *OldStr, const char *NewStr)
{
    char *v5;
	char *v6;
	char *v7;
	unsigned int v4;
	size_t OldLen;
	
	//printf("repstr(\"%s\",\"%s\",\"%s\")\n",Src,OldStr,NewStr);
    v4 = strlen(OldStr) + 1;
    OldLen = v4 - 1;
    
	if ( v4 == 1 )		// old string == <blank>. copy Src -> Dest
    {
        cdbcpystr(Dest, Src, 0);
    }
    else
    {
        v5 = Src;
        v6 = Src;
        while ((v7 = strchr(v5, *OldStr)))
        {
            if (!strncmp(v7, OldStr, OldLen))
            {
                *v7 = 0;
                strcat(Dest, v6);
                strcat(Dest, NewStr);
                v6 = &v7[OldLen];
                v7 = &v7[OldLen - 1];
            }
            v5 = v7 + 1;
        }
        strcat(Dest, v6);
    }
}

void concat(char *Dest, char *Src)
{
    short	v2;
    short	v3;
    char	Temp[1024]; 
    char	Save;

    v2 = lenstr(Dest);
    v3 = lenstr(Src);

    if ( v3 + v2 > 1000 )	// max result length is 1000
        v3 = (1000 - v2);
    
	Save	= Src[v3];		// preserve last char of source in case of buffer overrun. Normally ZERO
	Src[v3] = 0;
	cdbcpystr(Temp, Dest, Src, 0);
	cdbcpystr(Dest, Temp, 0);
	Src[v3] = Save;
}

bool stroper(unsigned short OpCode, char *Dest, char *Src1, char *Src2)
{
    //const char *v4;
	char	*v6;
	char	*i;
	char	*v8;
	char	*v12;
	char	*curr_char;
	
    int		v10;
	short	v13;
	signed short word_num;
	signed short curr_word;
	short	MaxLen;
	
    //v4 = Src2;
    OpCode &= 0xFC00;			// map out flag bits, leave only OpCode 
	switch (OpCode)
	{
	case f_ADD  :				// string addition 
        cdbcpystr(Dest, Src1, 0);
        concat(Dest, Src2);		// Limits destination to 1000 chars
        return true;
		break;
	
	case f_DIV  :				// string Division
		// Example:
		// a = 'abcde' / 203	a equals 'bcd' (starting at pos 2 for 3 chars). The divisor is made up of a starting
		//						position 0-99 followed by a number of characters to extract 00-99. The special case
		//						of a starting position followed by 00 extracts to the end of the string

		// a = 'abcde' / 200	a equals 'bcde' (starting at pos 2 to the end).
		v13 = strtol(Src2, 0, 10);
        if ( v13 > 99 )
        {
			//void substr(char *Dest, char *Buffer, int Start, int MaxLen)
			substr( Dest, Src1, (v13 - (v13 % 100)) / 100, v13 % 100);
        }
		else
	        cdbcpystr(Dest, Src1, 0);
        return true;
		break;
	
	case f_MUL  :				// string Multiply
	case f_SMULT:				// SMULT()
		v13		= strtol(Src2, 0, 10);								// no sanity check on mult value
        v10		= lenstr(Src1);
        MaxLen	= 0;
		v13--;
	    while ( v13 != -1 && MaxLen <= 1000 - v10 )
        {
			cdbcpystr(&Dest[MaxLen], Src1, 0);						// assume Dest is big enough to take 1000 chars
            MaxLen += v10;
		    v13--;
        }
        return true;			// always succeeds
		break;

	case f_SUB :				// string Subtraction
	case f_SDEL:				// SDEL()
        v12 = Src1;
        MaxLen = lenstr(Src2);
        while ( *v12 )
        {
			if ( *v12 == *Src2 && cmpbuf(v12, (char *)Src2, MaxLen) )
				cdbcpystr(v12, &v12[MaxLen], 0);
			else
				++v12;
        }
        cdbcpystr(Dest, Src1, 0);
        return true;
		break;

	case f_SKEY:				// SKEY
		/*
		skey(string1,string2)		string1 The string to match in string2
									string2 The string to be evaluated

		Character function, returning the characters following string1 in string2 up to the next current tab character. 
		The	default tab character is a space.
		Example:					tmp = 'The author=James and the title=Futures'
									author = skey('author=',tmp)
									The variable author contains 'James'.
		*/

		*Dest = 0;
        MaxLen = lenstr(Src1);
        while ( 1 )
        {
			v8 = strchr(Src2, *Src1);		// Look for first char of Src1 in Src2
            if ( !v8 )					// not found 
				return true;
			if ( !strncmp(v8, Src1, MaxLen) )
			{
				v6 = &v8[MaxLen];
                for ( i = v6; *i && *i != TAB; ++i )
					;
				*i = 0;
		        cdbcpystr(Dest, v6, 0);
	            return true;
			}
            Src2 = v8 + 1;
		}
		break;

	case f_MOD  :					// string % MOD Operator
	case f_SWORD:					// SWORD()
		
		// Example
		// a = 'one more' % 2		a equals 'more' (the second word). Words are separated by the current tab character
		// sword(string, num)		Character function, returns the num word of string. Words are separated by the current tab character.
		//
		//							set tab			#set tab to space
		//							string = sword('a little book',2)
		//							string contains 'little'

		word_num = strtol(Src2, 0, 10);
	    
		curr_word = 1;
	    curr_char = Src1;
		while ( *curr_char && word_num > curr_word )
		{
			if (*curr_char++ == TAB)	// Hit a seperator character?
			{
				curr_word++;
				if ( TAB == ' ' || TAB == '\t' )	// Multiple instances of <SPC> or <TAB> are only counted as *ONE*
				{
					while (*curr_char && *curr_char == TAB)		// continue to next non-seperator char or end of string, whichever comes first
						curr_char++;
				}
			}
		}
		
		while (*curr_char && *curr_char != TAB)		// found the word we want, copy to dest
			*Dest++ = *curr_char++;
		*Dest = 0;
		return true;
		break;
	}
	return false;	// indicate to caller that we didn't handle OpCode!
}

void datearr(FLDdesc *fld)
{
    double	*Array;
    short	count;
    int		Date_array[10];
	
    daysarr((signed int)*(double *)fld->FLDdata, Date_array);	// convert date val to Array 
	
	Array = (double *)fld->FLDname;								// Array variables hang off FLDname
	for ( count = 0 ; count <= 9; count++ )
        Array[count] = (double)Date_array[count];				// convert int values to double
}

void arrdate(FLDdesc *Dest_fld, FLDdesc *Src_fld)
{
    double	*Array;
	double	DateVal;
	short	count;
	int		Date_array[10];
	
    Array = (double *)Src_fld->FLDname;							// Array variables hang off FLDname
	for ( count = 0 ; count <= 9 ; count++ )
        Date_array[count] = (int)Array[count];					// convert double values to int array
    
	DateVal = (double)arrdays(Date_array);						// convert array to single double value/
    *(double *)Dest_fld->FLDdata = DateVal;						// day/month arithmetic may have occured!
    
	if ( DateVal == 0.0 )
        Dest_fld->FLDstat |= fld_ZERO;	// 0x0004;				// Quick zero flag check
    else
        Dest_fld->FLDstat &= ~fld_ZERO; // 0xFFFB;
    
	for ( count = 0 ; count <= 9; count++ )						// copy array back to Src, as hour/day arithmetic may have occured!!
        Array[count] = (double)Date_array[count];
}

#endif
