#ifndef LERANGE_C
#define LERANGE_C

#include "DBdefs.h"
#include "cl4.h"

int numrange(short ENTno, XTAB *xt)			// xt == accept field preceeding the range statement 
{
	ENTAB	*entb;
	double	v12;
	short	v4;
	char	nptr[256];

	entb = ENARR(ENTno);
	nptr[0] = 0;
	
	//if ( symbol == 2210 || symbol == 2200 )		// 2210 = floating point, 2200 = integer
	if ( symbol == 2210 || symbol == 2200 || symbol == 2220 )		// 2210 = floating point, 2200 = integer, 2220 = String
	{
		//eprint("numrange - symbol = %d, sym = %s,  xt->Flags = x%04X\n",symbol,sym, xt->Flags);
		cdbcpystr(nptr, sym, 0);
		if ( xt->Flags & xtb_DATE )		// Date field
		{
			if ( (double)clgetdate(nptr) < 0.0 )
			{
				loaderr(3, sym);		// "invalid date"
				return 0;
			}
			entb->Enun.Enref.TTno = 0;
			v4 = putconst(nptr, 'C');	// string type
		}
		else
		{
			if ( xt->Flags & xtb_TIME )	// Time field
			{
				v12 = gettime(nptr);
				if ( v12 < 0.0 )
				{
					loaderr(25, sym);	// "invalid time"
					return 0;
				}
				sprintf(nptr, "%f", v12);
			}
			// this catches all other numeric values
			entb->Enun.Enref.TTno = 0;
			v4 = putconst(nptr, '6');		// store numbers as 6 digit floats internally
		}
		if ( v4 >= 0 )
		{
			entb->Enun.Enref.VarNum = v4;
			entb->entype			= 1;
			symbol = getsym();
			return ENTno;
		}
		return 0;
	}
	if ( symbol == 2230 )		// Variable/Table Field
	{
		if ( gettfexp(ENTno))	// also advances to next symbol
			return ENTno;
		return 0;
	}
	loaderr(33, sym);			// unexpected symbol
	return 0;
}

int lerange(short PTno)
{
	PTAB	*pt;
	XTAB	*xt;
	RATAB	*ratb;
	ENTAB	*entb;
	
	short	v4;
	short	ENTno;
	short	RANGEno;
	short	v10;
	char	v21[256];
	
	pt = PTARR(getptabp(PTno - 1));				// We want to examine *previous* program instruction
	xt = XTARR(pt->TABno);						// look at previous instruction

	if ( pt->OpCode != 1 || xt->Flags & xtb_DISPLAY ) // OpCode 1 == accept field, Flags & 0x0004 == Display Only
    {
		loaderr(20, sym);				// "previous field not accept field"
        return 0;
    }
    if ( xt->RangeID )					// this accept field already has a range specifier attached!!
    {
		loaderr(18, sym);				// "cannot join and range check"
        return 0;
    }
    v4 = getramem();
	ratb = RAARR(v4);					// range specifier array
	
	xt->RangeID = v4;
    symbol = getsym();
    if ( symbol == 930 )
    {
		loaderr(7, sym);				// "unexpected end of line"
        return 0;
    }

    ENTno = getenmem();
    if ( xt->Flags & xtb_CHAR )			// Accept field is a String type
    {
		allenpt(ENTno, &entb);
        
		if ( symbol == 910 )			// "negative"
			ratb->RangeType = 2;
		
		else if ( symbol == 1140 )		// positive
			ratb->RangeType = 1;			// POSITIVE

		else if ( symbol == 840 )       // match		** undocumented **  perhaps unfinished?
		{
			ratb->RangeFrom			= ENTno;	// 840 match
			entb->Enun.Enop.Enoper	= 0x6C00u;
			entb->entype			= 2;
			ratb->RangeType			= 5;
			cdbcpystr(v21, sym, 0);		// ** never used **
			symbol = getsym();         
			
			if ( symbol != 730 || (entb->enleft = getparexp(0, 0), le_error) )		// 730 = "("
			{
				loaderr(26, sym);		// "expected a '(' or ')'"
				dallenpt(&entb);
				return 0;
			}
			if ( symbol != 1300 )       // 1300 = ")"
			{
				loaderr(26, sym);		// "expected a '(' or ')'"
				dallenpt(&entb);
				return 0;
			}
		}
		else if ( symbol == 900 )		// name
		{
			xt->Flags |= xtb_NAME;		// 0x0080u;		// 900 name
			ratb->RangeType = 6;
		}
        else if ( symbol == 2220 )		// string literal
		{
			entb->Enun.Enref.TTno	= 0;
			entb->Enun.Enref.VarNum = putconst(sym, 'C');
			entb->entype			= 1;
			ratb->RangeFrom			= ENTno;
			ratb->RangeType			= 4;		// range 'one,two,three'
		}
		else if ( symbol == 2230 )		// variable/field def
		{
			if ( !gettfexp(ENTno) )		// advances to the next symbol also
			{
				dallenpt(&entb);
				return 0;
			}
			ratb->RangeFrom = ENTno;
			ratb->RangeType = 4;			// range 'one,two,three'
			goto LABEL_MSG;					// skip getting another symbol
		}
		else	// error
		{
			if ( symbol )
				loaderr(33, sym);			// "unexpected symbol"
			dallenpt(&entb);
			return 0;
		}

        symbol = getsym();
LABEL_MSG:
        dallenpt(&entb);
	}
	else	// numeric values and date/time fields
	{
		if ( symbol == 1140 )                   // "positive"
		{
			ratb->RangeType = 1;
			symbol = getsym();
		}
		else if ( symbol == 910 )				// negative
		{
			ratb->RangeType = 2;
			symbol = getsym();
		}
		else
		{
			ratb->RangeType = 3;		// range FROM TO    eg: range 100 300 'must be between $1 and $2'
			RANGEno = numrange(ENTno, xt);		// also advances to next symbol
			if ( !RANGEno )
				return 0;	// error
			ratb->RangeFrom = RANGEno;			// from
			if ( symbol == 930 )
			{
				loaderr(7, sym);                // "unexpected end of line"
				return 0;
			}
			v10 = getenmem();
			RANGEno = numrange(v10, xt);		// also advances to next symbol
			if ( !RANGEno )
				return 0;	// error
			
			ratb->RangeTo = RANGEno;			// TO 
		}
	}
	// common exit code to find an optional custom error msg

	if ( symbol == 2220 )			// string literal = optional custom error message
	{
		ratb->StringOffset = getstrmem(lenstr(sym) + 1);
		cdbcpystr(STRARR(ratb->StringOffset), sym, 0);
		symbol = getsym();
	}
	if ( symbol == 930 )			// <CR>
		return 1;					// exit_success
	loaderr(33, sym);				// "unexpected symbol"
	return 0;// error exit
}

#endif
