#ifndef MSTRINGS_C
#define MSTRINGS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <string.h>		// for void *memset(void *s, int c, size_t n);
#include "DBdefs.h"
#include "cl4.h"

//void mexit(int ErrorCode)
//{
//	;	// Do nothing on Linux if we can't find the memory we want. Uh-oh!
//}

void mexit(int ErrorCode)
{
	longjmp(sjbuf, ErrorCode);	// crash out back into main()
}


void* mfree_0(void *ptr)
{
	if ( ptr )
		free(ptr);
	return 0;
}

void* mmalloc(size_t reqsize)
{
	void	*memptr; // edi@3

	if ( reqsize < 4 )
		reqsize = 4;				// minimum 4bytes per allocation

	memptr = malloc(reqsize);
	if ( !memptr )
	{
		if ( !mbuf )
		{
			eprint("was mmalloc_init() called? Not enough memory - bye\n");
			abort();
		}
		mexit(-99);
	}
	memset(memptr, 0, reqsize);		// block returned always ZEROed.
	return memptr;
}

void* mrealloc(void *ptr, int oldsize, size_t newsize)
{
	void	*memptr; // esi@1

	memptr = realloc(ptr, newsize);
	
	if ( !memptr )
		mexit(-99);	// fatal error
	// clear only the newly allocated area if larger
	// old area is guaranteed to remain constant, even if moved
	if ( newsize > oldsize )
		memset(memptr + oldsize, 0, newsize - oldsize);

	return memptr;
}

void* mmalloc_init(void)
{
	mbuf = malloc(1000u);	// src/DBdefs.h:char*      mbuf;  //used by mmalloc_init(), mmalloc()
	return mbuf;
}

char *mstrcpy(char *Src, ...)
{
	unsigned int NewSize;
	char *v4;
	char *v9;
	char *Dest;
	va_list va;
	
	// first section works out the total length of the new string required.
	va_start(va, Src);
	NewSize = 0;
	v4		= Src;
	while( v4 )				//  Halt at NULL pointer, but not at null string, this is OK
	{
		NewSize += strlen(v4);
		v4 = va_arg(va,char*);
	}
	va_end(va);	// keep var_arg macros happy
	
	Dest = mmalloc(NewSize + 1);	// NewSize now equals the buffer size required (less \0).

	// go back to start of the list
	va_start(va, Src);
	v9 = Dest;
	v4 = Src;
	while (v4)
	{
		while(*v4)
			*v9++ = *v4++;
		v4 = va_arg(va,char*);
	}
	va_end(va);					// keep var_arg macros happy
	*v9 = 0;					// Ensure null terminated string
	return Dest;
}

char *mstrcat(char *orig_str, char *add_str, ...)
{
	char	*v4;
	char	*v9;
	char	*v10;
	int		oldlen; 
	int		addlen;
	va_list va; 

	//printf("mstrcat(orig_str = %s, add_str = %s)\n",orig_str,add_str);

	va_start(va, add_str);
	
	addlen = 0;
	oldlen = strlen(orig_str);
	v4 = add_str;
	// first section works out the total length of the new string required.
	while (v4)
	{
		while (*v4)
		{
			addlen++;		// length of extra string space required
			v4++;
		}
		v4 = va_arg(va,char*);
	}

	v9 = (char*)mrealloc(orig_str, oldlen, oldlen + addlen + 1);		// old area guaranteed to remain constant if larger

	v10 = &v9[oldlen];		// Start at byte after original string
	va_end(va);				// keep var_arg macros happy

	va_start(va, add_str);	// go back to start of the list
	v4 = add_str;			// First time through use arg#2, then hit the VA list 
	while ( v4 )
	{
		while(*v4)
			*v10++ = *v4++;
		v4 = va_arg(va,char*);
	}	

	va_end(va);	// keep var_arg macros happy
	*v10 = 0;
	return v9;
}

char *mstrncpy(char *Dest, int NumBytes, char *Src, ...)		// NumBytes == max size of resultant string
{
	char	*v4;
	char	*v6;
	va_list va;

	va_start(va, Src);
	v4 = Dest;
	v6 = Src;
	while (v6 && NumBytes )
	{
		while( *v6 && --NumBytes )
			*v4++ = *v6++;
		v6 = va_arg(va,char*);
	}

	va_end(va);	// keep var_arg macros happy
	*v4 = 0;
	return Dest;
}

#endif
