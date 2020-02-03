#ifndef LELIBCALL_C
#define LELIBACLL_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

#include <assert.h>

bool lelibcall(int *LineNo)
{
	PTAB	*ptab;
	ENTAB	*entb;
	ENTAB	*entr;
	ENTAB	*ent_test;
	FLDdesc *fld;

	short	ENTtop;
	short	ENTno;
	short	ENTleft		= 0;
	short	ENTright	= 0;
	short	Varno		= 0;
	short	params		= 0;		// count params found in call, 4 minimum
	bool	addr_of		= false;	// flag to indicate use of "Address Of" operator '&'

	// format of libcall is : libcall(returnvar, libhandle, funchandle, 'FunctionName', [void] [var1][,var2.....])
	//
	// returnvar is a previously defined variable. Will determine return type of function
	// libhandle is a previously initialised 'N' variable.
	// funchandle is a previously defined 'N' variable. Will be set on first entry to the libcall function.
	// 'FunctionName' is an existing function name in the dynamically loaded library. Literal, or String variable
	// Optional parameters, will determine function call param types. Use void for param 1 when no parameters required.

	symbol = getsym();
	if (symbol == 730)								// '('
	{
		symbol	= getsym();							// skip brace
		ENTleft	= loadexp(0,0);						// get return value variable

		ent_test = ENARR(ENTleft);
		if (!ent_test || ent_test->Enun.Enref.TTno || ent_test->entype != 1)	// libcall function accepts Variables or Literals *ONLY* for return value
			loaderr(92, sym);
		else
		{
			params++;								// track number of params, 5 (min) required
			ENTtop = getenmem();					// save ENTtop for later insertion in PTAB if all params correct
			allenpt(ENTtop, &entb);
		    entb->entype	= 0x20u;				// Function Call param list type
		    entb->enleft	= ENTleft;				// save return variable ptr here

			while(symbol == 180)					// comma
			{
				symbol = getsym();
				if (symbol == 80)	// '&'			// used to pass *ADDRESS OF* a pointer
				{
					addr_of = true;
					symbol = getsym();
				}
				else
					addr_of = false;
				//printf("sym = \'%s\',symbol = %d, syml = %d\n",sym,symbol,syml);
				ENTno = loadexp(0,0);				// advances symbol
				ent_test = ENARR(ENTno);
				//printf("ent_test = %08X, ent_test->TTno = %d, ent_test->entype = %d\n",ent_test, ent_test->TTno, ent_test->entype);
				//                     variable                  <3 char string          Integer literal
				if (ent_test && (ent_test->entype == 1 || ent_test->entype == 0x10|| ent_test->entype == 0x08))
				{
					params++;
					if (addr_of)
					{
						fld = getvars(ent_test->Enun.Enop.RecNo);
						fld->FLDstat |= fld_ADDROF;		// flag for clenter to indicate *ADDRESS OF* this variable is passed in function calls
					}
					entb->enright = ENTno;				// in case we are second last param. May be overwritten below
					if (symbol == 180)					// comma, means we can expect another parameter to follow
					{
						ENTright		= getenmem();
						allenpt(ENTright, &entr);
						entr->entype	= 0x20u;
						entr->enleft	= ENTno;
						dallenpt(&entr);
						entb->enright	= ENTright;
						dallenpt(&entb);

						allenpt(ENTright, &entb);		// lock for next loop around
						entb->entype	= 0x20u;
					}
				}
				else	
				{
					loaderr(92, sym);					// libcall function accepts Variables or Literals *ONLY*
					break;
				}
			}
			dallenpt(&entb);
		}
	}
	

	if (params >= 5 && !comp_errs )	// We got the required number of parameters, and no errors: create PTAB record
	{
		allptpt(getptabp((*LineNo)++), &ptab);
		ptab->OpCode	= 420;		// DC extension! libcall opcode
		ptab->SrcLineNo	= lla;
		ptab->TABno		= ENTtop;	// expression containing required pointers
		dallptpt(&ptab);
	}

	if (symbol == 1300)		// ')'
	{
		symbol = getsym();
		return true;		// exit_success
	}
	while ( symbol && symbol != 930 )	// advance to next line
		symbol = getsym();
    return true;						// exit_success
}

#endif

/*

Sample code to allocate a program instruction:

	allptpt(getptabp((*LineNo)++), &ptab1);
	ptab1->OpCode	 = 270;						// msg function code
	ptab1->SrcLineNo = lla;
	AuxError = !lemsg(ptab1);
    dallptpt(&ptab1);
	if ( !AuxError )
		return 1;
	break;


bool lemsg(PTAB *pt)
{
     ENTAB	*ent;
	 short	v1;
	 short	v3;
	 short	ENTno;
	 
     symbol = getsym();
     v1 = loadexp(0, 1);
     if ( v1 )
     {
          v3 = loadexp(0, 1);
          if ( v3 )
          {
               ENTno		= getenmem();
               ent			= ENARR(ENTno);
               ent->enleft	= v1;
               ent->enright	= v3;
               pt->TABno	= ENTno;	// point program instruction at this entno
               return true;				// exit success
          }
     }
     return false;	// error_exit
}



*/
