#ifndef POPENLIB_C
#define POPENLIB_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <avcall.h>		// using avcall() from libffcall
#include <dlfcn.h>		
#include "DBdefs.h"
#include "cl4.h"

bool popenlib(int ENTno)		// DC extension
{
	void	*handle;
	char	*error;

	ENTAB	*entb;
	FLDdesc *fld_dest;
	char	libname[256];

	av_alist alist;									// avcall param array
	void	(*voidptr)(void);						// A *one size fits* all pointer for avcall use
	char*	strval;
	int		intval = 0;
	void	*(*bookptr)(void);
	void*	BookHandle;

	// entleft	= handle destination ptr
	// entright = LibraryName

	entb = ENARR(ENTno);
	exptobuf(libname, ENARR(entb->enright), 0);		// Library path/name string variable

	entb = ENARR(entb->enleft);
	fld_dest = &ttab->TTfields[entb->RecNo];		// library handle variable 'N' type

	if (!(fld_dest->FLDstat & fld_LIB))				// ensure library only opened **once** !
	{
		handle = dlopen (libname, RTLD_LAZY);		// RTD_LAZY == "resolve undefined symbols as code from the dynamic library is executed"
		if (!handle)								// RTD_NOW  == "resolve all undefined symbols before dlopen() returns and fail if this cannot be done"
		{
			print(dlerror());
			return false;		// exit_fail
		}
		*(void**)fld_dest->FLDdata = handle;		// save library handle for later use
		fld_dest->FLDstat |= fld_LIB;				// flag this variable for closing the library later 
		fld_dest->FLDstat &= ~fld_ZERO;				// not-zero flag
	}

/*
	// test code only
	bookptr = dlsym(handle, "xlCreateBookCA");
	if ((error = dlerror()) != NULL)
	{
		printf(error);
		return true;								// avoid calling null pointers
	}
	BookHandle = (*bookptr)();						// create bookhandle object

	voidptr = dlsym(handle, "xlBookVersionA");
	if ((error = dlerror()) != NULL)
	{
		printf(error);
		return true;								// avoid calling null pointers
	}

	// call through avcall()
	av_start_int (alist, voidptr, &intval);
	av_ptr (alist, void*, BookHandle);
	av_call (alist);
	//printf ("CLversion() returned \"%s\"\n", strval);
	printf ("xlBookVersionA() returned \"%08X\"\n", intval);

	//dlclose(handle);
*/
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