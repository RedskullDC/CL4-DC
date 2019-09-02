#ifndef PFRESH_C
#define PFRESH_C

#include <stdio.h>
#include <assert.h>		// for assert() naturlich!
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

void pscreen(char *ScreenName)
{
	STAB	**stb;
	STAB	*v5;
	int		ScreenNo;
	int		v3;
	short	Size;
	short	NameLen;
	short	scrno;
	
	//printf("pscreen(%s)\n",ScreenName);

	if ( ScreenName )
	{
		NameLen		= lenstr(ScreenName) + 1;
		stb			= stab;							// global - screen table array

		for ( ScreenNo = 0 ; no_stabs > ScreenNo; ScreenNo++ )
		{
			if ( *stb && cmpbuf((*stb)->ScreenName, ScreenName, NameLen))
				break;
			stb++;
		}
    
		if ( ScreenNo == no_stabs )					// got to end of screen def table with no match
		{
			stb = stab;
			if ( *stab && ScreenNo > 0 )
			{
				v3 = 0;
				while ( 1 )
				{
					++v3;
					++stb;
					if ( !*stb )
						break;
					if ( v3 >= ScreenNo )
					{
						scrno = v3;
						goto LABEL_15;
					}
				}
				scrno = v3;
			}
			else
				scrno = 0;
LABEL_15:
			if ( scrno == ScreenNo )
			{
				newstab();							// should bump no_stabs by 1
				assert(scrno < no_stabs);
				stb = &stab[scrno];
			}
		}

		Size = _co * _li + 10;						// what are the extra 10 bytes for?
		//Size = columns * lines + 10;

		if ( !*stb )
		{
			v5 = mmalloc(sizeof(STAB));				// sizeof(STAB) == 12
			v5->ScreenName	= mmalloc(NameLen + 1);
			v5->abuf	= mmalloc(Size);
			v5->ccbuf	= mmalloc(Size);
			*stb = v5;
		}
		cdbcpystr((*stb)->ScreenName, ScreenName, 0);
		cpybuf((*stb)->abuf, abuf, Size);
		cpybuf((*stb)->ccbuf, ccbuf, Size);
	}
}

void pfresh(char *ScreenName)
{
    STAB	**stb;
	int		ScreenNo;
	int		NameLen;
	int		MaxLen;
	
    if ( !ScreenName )		// no screen name is also legal for refresh?
        goto LABEL_11;
    
	MaxLen		= _co * _li + 10;			// what are the extra 10 bytes for?
    NameLen		= strlen(ScreenName) + 1;
    stb			= stab;
    ScreenNo	= 0;
    if ( no_stabs > 0 )
    {
        while ( !*stb || !cmpbuf((*stb)->ScreenName, ScreenName, NameLen))
        {
            ++ScreenNo;
            ++stb;
            if ( ScreenNo >= no_stabs )
                goto LABEL_7;
        }
        cpybuf(ccbuf, (*stb)->ccbuf, MaxLen);		// copy saved data back into main buffers
        cpybuf(abuf, (*stb)->abuf, MaxLen);
        mfree_0((*stb)->ccbuf);
        mfree_0((*stb)->abuf);
        mfree_0((*stb)->ScreenName);
        mfree_0(*stb);
        *stb = 0;
    }

LABEL_7:
    if ( ScreenNo != no_stabs )
LABEL_11:
        clrefresh();
}

#endif

