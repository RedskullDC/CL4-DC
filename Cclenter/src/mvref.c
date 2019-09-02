#ifndef MVREF_C
#define MVREF_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void _mvref(REFR *a1, REFR *a2)
{
    /*
    register REFR **v2; // eax@1

#1	a1->Offset = a2->Offset;
    v2 = a2->Data;				// These structures are chained together.
    a1->Data = a2->Data;
    *v2 = a1;
	
#2	v2	= a2->Data;
	*a1	= *a2;					// rely on GCC to copy whole structure here!
	*v2	= a1;
	*/

	*a1	= *a2;					// rely on GCC to copy whole structure here!
	*a2->Data = a1;				// doesn't preserve the existing a1->Offset value!

/*	above produces
a1              = dword ptr  8
.text:0804D110 a2              = dword ptr  0Ch
.text:0804D110
.text:0804D110                 push    ebp
.text:0804D111                 mov     ebp, esp
.text:0804D113                 mov     edx, [ebp+a2]
.text:0804D116                 push    ebx
.text:0804D117                 mov     eax, [ebp+a1]
.text:0804D11A                 mov     ebx, [edx+4]    ; ebx <- a2.Offset
.text:0804D11D                 mov     ecx, [edx]      ; ecx <- a2.Data
.text:0804D11F                 mov     [eax+4], ebx    ; a1.Offset <- ebx
.text:0804D122                 mov     [eax], ecx      ; a1.Data <- ecx
.text:0804D124                 mov     edx, [edx]      ; edx <- a2.Data
.text:0804D126                 mov     [edx], eax      ; *a2.Data <- a1
.text:0804D128                 pop     ebx
.text:0804D129                 pop     ebp
.text:0804D12A                 retn
*/

}

/*

Original:
void __cdecl mvref(REFR *a1, REFR *a2)
{
  REFR **v2;
  v2	= a2->Data;
  *a1	= *a2;
  *v2	= a1;
}

_mvref          proc near               ; CODE XREF: _loadr+212p
.text:08096168                                         ; _loadr+32Dp ...
.text:08096168
.text:08096168 a1              = dword ptr  8
.text:08096168 a2              = dword ptr  0Ch
.text:08096168
.text:08096168                 push    ebp
.text:08096169                 mov     ebp, esp
.text:0809616B                 mov     edx, [ebp+a1]	edx points at a1
.text:0809616E                 mov     ecx, [ebp+a2]	ecx points at a2
.text:08096171                 mov     eax, [ecx+4]		eax <- a2.Offset
.text:08096174                 mov     [edx+4], eax		copy a1.Offset <- a2.Offset
.text:08096177                 mov     eax, [ecx]		eax <- a2.Data
.text:08096179                 mov     [edx], eax		copy a1.Data <- a2.Data 
.text:0809617B                 mov     [eax], edx		(a2.Data) = a1
.text:0809617D                 pop     ebp
.text:0809617E                 retn
.text:0809617E _mvref          endp

struct REFR
{
  REFR **Data;
  char *Offset;
};

*/

#endif
