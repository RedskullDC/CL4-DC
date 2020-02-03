#ifndef PLIBCALL_C
#define PLIBCALL_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <avcall.h>		// using avcall() from libffcall
#include <dlfcn.h>		
#include "DBdefs.h"
#include "cl4.h"

bool plibcall(int ENTno)		// DC extension
{
	char	*error;	// for dl errors

	ENTAB	*entb;
	FLDdesc *fld_dest;
	FLDdesc *fld_libhandle;
	FLDdesc *fld_funchandle;
	FLDdesc *fld_funcname;
	FLDdesc *fld_param;
	char	libname[256];
	int		VarNo;

	FLDdesc fld_dummy;

	av_alist alist;									// avcall param array
	int		int_return = 0;
	double	double_return = 0.0;
	char*	str_return = NULL;
	void*	ptr_return = NULL;
	void*	funcptr;

	// format of libcall is : libcall(returnvar, libhandle, funchandle, 'FunctionName', [void] [var1][,var2.....])

	//                    ENTno
	//          retrunvar           entright
	//                        libhandle          entright    
	//                                    funchandle          entright
	//                                                  funcname       entright
	//                                                             param1        .....
	//                                                                    param N-1   param N
 
	entb = ENARR(ENTno);
	fld_dest = getftf(ENARR(entb->enleft), false, &fld_dummy);

	entb = ENARR(entb->enright);	// move down right chain
	fld_libhandle = getftf(ENARR(entb->enleft), false, &fld_dummy);

	if (fld_libhandle->FLDstat & fld_LIB)					// ensure that Library has been opened ok
	{
		entb = ENARR(entb->enright);						// move down right chain, now pointing at LibHandle
		fld_funchandle = getftf(ENARR(entb->enleft), false, &fld_dummy);

		entb = ENARR(entb->enright);						// move down right chain, now pointing at FunctionName
		fld_funcname = getftf(ENARR(entb->enleft), false, &fld_dummy);

		if (!(fld_funchandle->FLDstat & fld_FUNC))			// ensure function handle initialised, only call *once*
		{
			//printf("plibcall() - init funchandle\n");
			*(void**)fld_funchandle->FLDdata = dlsym( *(void**)fld_libhandle->FLDdata, (char*)fld_funcname->FLDdata);
			if ((error = dlerror()) != NULL)
			{
				printf("%s",error);							// stop GCC warning : format not a string literal and no format arguments 
				return false;								// avoid calling null pointers
			}
			fld_funchandle->FLDstat |= fld_FUNC;			// flag this variable as now holding a valid function pointer
			fld_funchandle->FLDstat &= ~fld_ZERO;			// not-zero
		}
		
		// Library and Function now all open and ready for use.

		funcptr = *(void**)fld_funchandle->FLDdata;
		
		switch (fld_dest->FLDtype)							// determine initialiser based on return type.
		{
		case 'C':			// String
			if (fld_dest == &ttab->TTfields[v_NULL])		// void passed as return param == Variable ZERO
				av_start_void (alist, funcptr);
			else
				av_start_ptr (alist, funcptr, char*, &str_return);
			break;
		case 'N':			// Integer
		case 'X':			// HEX
			av_start_int (alist, funcptr, &int_return);
			break;
		case 'P':			// Object
			av_start_ptr (alist, funcptr, void*, &ptr_return);
			break;
		case '0' ... '9':	// Double
			av_start_double (alist, funcptr, &double_return);
			break;
		}

		// Now start putting params on the stack.

		entb = ENARR(entb->enright);		// params chain down the right 
		while ( entb->entype == 0x20 )
		{
			fld_param = getftf(ENARR(entb->enleft), false, &fld_dummy);
			switch (fld_param->FLDtype)
			{
			case 'C':
				if (fld_param->FLDstat & fld_ADDROF)
					av_ptr (alist, char**, (char**)&fld_param->FLDdata);
				else
					av_ptr (alist, char*, (char*)fld_param->FLDdata);
				break;
			case 'N':
			case 'X':
				av_int (alist, (int)*(double*)fld_param->FLDdata);
				break;
			case 'P':
				if (fld_param->FLDstat & fld_ADDROF)
					av_ptr (alist, void*, (void*)&fld_param->FLDdata);
				else
					av_ptr (alist, void*, *(void**)fld_param->FLDdata);
				break;
			case '0' ... '9':
				av_double (alist, *(double*)fld_param->FLDdata);
				break;
			}
			entb = ENARR(entb->enright);		// chain extends down enright
		}

		// last, or only param if void passed in.
		//        <3 char lit,          Integer                          Non-VOID Variable
		if (entb->entype == 0x10 || entb->entype == 0x08 || (entb->entype == 1 && entb->Enun.Enref.VarNum != 0)) // RecNo = ZERO indicates void parameter , do nothing. Only valid for 1st param
		{
			fld_param = getftf(entb, false, &fld_dummy);
			switch (fld_param->FLDtype)
			{
			case 'C':
				if (fld_param->FLDstat & fld_ADDROF)
					av_ptr (alist, char**, (char**)&fld_param->FLDdata);
				else
					av_ptr (alist, char*, (char*)fld_param->FLDdata);
				break;
			case 'N':
			case 'X':
				av_int (alist, (int)*(double*)fld_param->FLDdata);
				break;
			case 'P':
				if (fld_param->FLDstat & fld_ADDROF)
					av_ptr (alist, void*, (void*)&fld_param->FLDdata);
				else
					av_ptr (alist, void*, *(void**)fld_param->FLDdata);
				break;
			case '0' ... '9':
				av_double (alist, *(double*)fld_param->FLDdata);
				break;
			}
		}
		
		av_call (alist);	// do actual function call!

		fld_dest->FLDstat |= (fld_ZERO|fld_DATA_AVAIL);				// default to ZERO result, DATA AVAIL flags

		switch (fld_dest->FLDtype)			//Update destination variable with result
		{
		case 'C':
			if (fld_dest != &ttab->TTfields[v_NULL])				// void passed as return param (v_NULL is type [C20])
			{
				cdbcpystr((char*)fld_dest->FLDdata, str_return, 0);	// copy string to our memory space. *** to do, check limits**
				if (*str_return)
					fld_dest->FLDstat &= ~fld_ZERO;					// clear ZERO flag
			}
			break;
		case 'N':
		case 'X':
			*(double*)fld_dest->FLDdata = (double) int_return;
			if (int_return)
				fld_dest->FLDstat &= ~fld_ZERO;						// clear ZERO flag
			break;
		case 'P':
			*(void**)fld_dest->FLDdata = ptr_return;
			if (ptr_return)
				fld_dest->FLDstat &= ~fld_ZERO;						// clear ZERO flag
			break;
		case '0' ... '9':
			*(double*)fld_dest->FLDdata = double_return;
			if (double_return)
				fld_dest->FLDstat &= ~fld_ZERO;						// clear ZERO flag
			break;
		}
	}
	else
	{
		printf("libcall() - Library Not OPEN!!\n");
		return false;
	}

	return true;	// exit_success
}

#endif

/*
One of the following two values must be included in flag:

RTLD_LAZY
	Perform lazy binding. Only resolve symbols as the code that references them is executed. 
	If the symbol is never referenced, then it is never resolved. (Lazy binding is only performed for function references; 
	references to variables are always immediately bound when the library is loaded.)
RTLD_NOW
	If this value is specified, or the environment variable LD_BIND_NOW is set to a nonempty string, 
	all undefined symbols in the library are resolved before dlopen() returns. If this cannot be done, an error is returned.

Zero or more of the following values may also be ORed in flag:

RTLD_GLOBAL
	The symbols defined by this library will be made available for symbol resolution of subsequently loaded libraries.
RTLD_LOCAL
	This is the converse of RTLD_GLOBAL, and the default if neither flag is specified. 
	Symbols defined in this library are not made available to resolve references in subsequently loaded libraries.
RTLD_NODELETE (since glibc 2.2)
	Do not unload the library during dlclose(). Consequently, the library's static variables are not reinitialized if 
	the library is reloaded with dlopen() at a later time. This flag is not specified in POSIX.1-2001.
RTLD_NOLOAD (since glibc 2.2)
	Don't load the library. This can be used to test if the library is already resident (dlopen() returns NULL if it is 
	not, or the library's handle if it is resident). This flag can also be used to promote the flags on a library that is 
	already loaded. For example, a library that was previously loaded with RTLD_LOCAL can be reopened with RTLD_NOLOAD | RTLD_GLOBAL. 
	This flag is not specified in POSIX.1-2001.
RTLD_DEEPBIND (since glibc 2.3.4)
	Place the lookup scope of the symbols in this library ahead of the global scope. This means that a self-contained library 
	will use its own symbols in preference to global symbols with the same name contained in libraries that have already been loaded. 
	This flag is not specified in POSIX.1-2001.
*/