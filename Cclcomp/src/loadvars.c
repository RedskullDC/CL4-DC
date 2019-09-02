#ifndef LOADVARS_C
#define LOADVARS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <assert.h>
#include "DBdefs.h"
#include "lvars.h"

void loadvars(void)
{
	FLDdesc *fld2;
	FLDdesc *fld;
	int reqsize;
	
	res_v_max = sizeof(lvars)/sizeof(FLDdesc) - 1;		// res_v_max = 62;
	//eprint("loadvars(res_v_max = %d)\n",res_v_max);

	fld2 = lvars;				// local pre-defined variables
	while ( fld2->FLDname )		// ZERO terminated list
	{
		assert(getvars(last_var) == ((void*)0));		// last_var == global nastiness
		fld = getvarmem();
		assert(fld == getvars(last_var));				// should now exist after we allocate mem

		reqsize = fld2->FLDtype == 'C' ? fld2->FLDlen + 1 : sizeof(double);	// all else get 8 bytes, each stored as (double)
		
		fld2->FLDdata = getmem(reqsize);
		cpybuf((char*)fld,(char*)fld2, sizeof(FLDdesc));
		
		// **TODO** expand this to allow sysvars to have pre-defined values!

		troot = gettnode(troot, fld2->FLDname, lenstr(fld2->FLDname), 1);
		fld2++;
	}
	assert(last_var == res_v_max);
}
#endif
