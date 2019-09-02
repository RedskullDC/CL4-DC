#ifndef PATTERN_C
#define PATTERN_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short trysmatch(char *a2, char *arg0);

short smatch(char *a1, char *a2)
{
    char	*v2;
	char	*v3;
	short	v4;
	short	result;
	char	v6;
	
    v2 = a2;
    v3 = a2;
    
	if ( *a2 )
    {
        while ( 1 )
        {
            v4 = 0;
            while ( *v2 && (*v2 != ('|') || v4))
			{
				if ( *v2 == '\\' )
					v4 = v4 == 0;
				else
					v4 = 0;
				++v2;
            }
            
			v6	= *v2;
            *v2 = 0;
            result = trysmatch(a1, v3);
            if ( result )
                break;
            *v2 = v6;
            if ( v6 )
                ++v2;
            v3 = v2;
            if ( !*v2 )
				return 0;
        }
        *v2 = v6;
    }
    else
    {
        result = 0;
    }
    return result;
}

short trysmatch(char *a2, char *arg0)
{
    char	*v2;
	char	*v4;
	char	*v5;
	char	*MatchChars;
	
    int		MatchCount;
	short	MaxLen;
	short	v10;
	
    v10 = 0;
    v2 = recomp(arg0, 0, 0);
	if ( v2 )
    {
        setmatch("", 0, 0);                     // clear any existing values for "MATCH" and "LMATCH"
        MatchChars = 0;
        v4 = reexec(v2, a2, &MatchChars);
        v5 = v4;
        if ( v4 )
        {
            if ( lenstr(v4) > 1000 )
                v5[1000] = 0;
            v10 = (short)(__loc1 - a2 + 1);
            if ( MatchChars )
            {
                MaxLen = lenstr(MatchChars);
                if ( MaxLen >= 1001 )
                    MaxLen = 1000;
            }
            else
            {
                MaxLen = 0;
                MatchChars = "";
            }
            setmatch(MatchChars, MaxLen, (v5 - __loc1));	// MatchCount = v5 - __loc1;
        }
        mfree_0(v2);
        return v10;
    }
    return -1;
}


void setmatch(char *MatchChars, short MaxLen, int MatchCount)
{
    FLDdesc *MatchFLD;
	
	MatchFLD = &ttab->TTfields[v_MATCH];		// "MATCH" fld
    
	if ( MaxLen > 999 )		// MATCH var has 1000 chars space pre-allocated in clcomp
        MaxLen = 999;
    
	cpybuf((char *)ttab->TTfields[v_MATCH].FLDdata, MatchChars, MaxLen);
    *((char *)MatchFLD->FLDdata + MaxLen) = 0;
    
	if ( *MatchChars )
        MatchFLD->FLDstat = (MatchFLD->FLDstat | fld_DATA_AVAIL) & ~fld_ZERO;
//        MatchFLD->FLDstat = (MatchFLD->FLDstat | 0x0010) & 0xFFFB;

    setvar(v_LMATCH, MatchCount);		// "LMATCH"
}

// pmatch() not called in clenter

#endif
