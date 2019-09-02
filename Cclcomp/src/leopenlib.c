#ifndef LEOPENLIB_C
#define LEOPENLIB_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

#include <assert.h>

bool leopenlib(int *LineNo)
{
	PTAB	*ptab;
	ENTAB	*entb;

	short	ENTno;
	short	ENTleft = 0;
	short	ENTright = 0;
	short	Varno = 0;

	// format of openlib call is : openlib( handle, 'Libpathname' ). Handle is an 'N' integer, treated as void*
	//
	symbol = getsym();
	if (symbol == 730)							// '('
	{
		symbol = getsym();
		if (symbol == 2230)						// expecting a pre-defined variable here
		{
			ENTleft = getenmem();
			if (gettfexp(ENTleft))				// get 'handle' variable name. *MUST* be pre-defined
			{	//	gettfexp advances symbol
				if (symbol == 180)				// comma
				{
					symbol = getsym();
					if (symbol == 2220 || symbol == 2230)	// libpathname constant or pre-defined variable
					{
						switch (symbol)
						{
						case 2220:
							Varno = putconst(sym, 'C');
							ENTright = getenmem();
							allenpt(ENTright, &entb);		// enclose mods to entab structure in these lock methods
							entb->TTno		= 0;			// normal variable
							entb->RecNo		= Varno;		// libname var
							entb->entype	= 1;
							dallenpt(&entb);
							break;

						case 2230:
							ENTright = getenmem();
							ENTright = gettfexp(ENTright);	// get 'library' name
															// will pop an error if invalid
							break;
						}
						// expression to point at both variables
						ENTno = getenmem();
						allenpt(ENTno, &entb);		// enclose mods to entab structure in these lock methods
						entb->TTno		= 0;		// normal variable
						entb->RecNo		= 0;		// 
						entb->entype	= 0x20;		// function call array
						entb->enleft	= ENTleft;	// 'handle' variable num from above
						entb->enright	= ENTright;	// 'library' variable num from above, may be zero!
						dallenpt(&entb);

						allptpt(getptabp((*LineNo)++), &ptab);
						ptab->OpCode	= 410;	// **** No-op while we are testing ************
						ptab->SrcLineNo	= lla;
						ptab->TABno	= ENTno;	// expression containing required pointers
						dallptpt(&ptab);

						symbol = getsym();
						if (symbol == 1300)		// ')'
						{
							symbol = getsym();
							return true;		// exit_success
						}
					}
				}
			}
		}
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
