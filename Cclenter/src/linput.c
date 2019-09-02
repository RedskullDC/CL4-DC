#ifndef LINPUT_FNS_C
#define LINPUT_FNS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int isDoubleWidth(unsigned char a1)
{
    int result = 0;
	//printf("isDoubleWidth(%d) = ",a1);

    if ( !isJapan || (signed int)a1 <= 160 || (signed int)a1 > 223 )
        result = (unsigned int)a1 >> 7;         // return the high bit

	//printf("%d\n",result);
    return result;
}

int curChineseChar(char *a1, char *a2)
{
	char	*i;
    int		v2;
	signed int v4;
	
    v2 = 0;
    for ( i = a1; i <= a2; ++i )
    {
        switch ( v2 )
        {
            case 0:
            case 1:
            case 4:
            case 8:
                v4 = (unsigned char)*i;
                if ( !isJapan || v4 <= 160 || (v2 = 8, v4 > 223) )
                    v2 = ((char)v4 < 0) + 1;
                break;
            case 2:
                v2 = 4;
                break;
            default:
                continue;
        }
    }
    return v2;
}

int isChinese(char *a1, char *a2)
{
    int		v2;
	int		v3 = 0;

    v2 = curChineseChar(a1, a2);
    if ( v2 == 2 || v2 == 4 || v2 == 8 )
        v3 = 1;
    return v3;
}

int gchar(void)
{
    int result; // eax@5
	int	*errloc;

	errloc = (int*)__errno_location();

    *errloc = 0;
    while ( 1 )
    {
        result = Key_GetChar();
        if ( result != -1 )
            break;
        if ( *errloc != 4 )	//	#define EINTR 4  /* Interrupted system call */
        {
            alarm(1u);
            while ( 1 )     // Nasty deadlock loop while waiting for SIGALARM!
                ;
        }
    }
    return result;
}

int linput(char *Buffer, int MaxLen, ONESC *a2)
{
    //int result; // eax@2

    /*if ( *_YL == 'T' )
    {
        result = linputThai(Buffer, MaxLen, a2);
    }
    else
    {
        if ( *_YL == 'J' || *_YL == 'j' )
            isJapan = 1;
        result = linputChinese(Buffer, MaxLen, a2);
    }	*/

	//result = linputChinese(Buffer, MaxLen, a2);
    //return result;
	return linputChinese(Buffer, MaxLen, a2);

}

int strOffPaperLen(char *a1, int a2)
{
    //int result; // eax@2

    //if ( (*__ctype_toupper_loc())[*YL] == 'T' )
    //    result = thaiStrOffPaperLen(a1, a2);
    //else
    //    result = a2;
    //return result;
	return a2;
}

int strOnPaperLen(char *a1)
{
    //unsigned int result; // eax@2

    //if ( (*__ctype_toupper_loc())[*YL] == 'T' )
    //    result = thaiStrOnPaperLen(a1);
    //else
    //    result = strlen(a1);
    //return result;
	return strlen(a1);
}

int linputChinese(char *DestBuffer, int MaxLen, ONESC *onesc)
{

    char	*DestPtr;	// Points to current char position in DestBuffer
    char	*FillPtr;	// Points to start of "Fill" chars being shown in DestBuffer if applicable
    char	*v39;
	
    signed int Key; // esi@16
    int		v14; // esi@149
    char	v17; // al@153
    int		v18; // esi@166
    int		v30; // esi@254
    char	v31; // dl@255
    int		result; // eax@265
    short	v36; // ax@277
    short	v38; // ax@291

    char	FillChar;
	char	BkSpc = *(char *)_yh;			// convenience to slightly speed up processing
    short	v45 = 0;
	bool	FullRetn = false;				// Allow auto-complete when buffer area is full
    char	*v50;
	char	FieldSave[1024];				// unsure of real size. 1024 is ample for single field buffer.

    if ( MaxLen < 0 )
    {
        FullRetn = true;					// signifies allow return when DestBuffer full, don't wait for <CR>
        MaxLen = -MaxLen;
    }

	FillChar = *_Yf;
    if ( !FillChar )
        FillChar = ' ';
    
	char *DestEnd = &DestBuffer[MaxLen];	// Field can be MaxLen long + \0 terminator
	*DestEnd = 0;							// ensure DestBuffer is nil terminated

    DestPtr = DestBuffer;					// DestPtr 

	v50 = FieldSave;						// Buffer to save current state of field in case we opt out of any changes
    while( *DestPtr )
    {
        fputc(*DestPtr, stderr);			// copy string to screen,
		*v50++ = *DestPtr++;				// and to internal buffer
    }
    *v50 = 0;								// ensure internal buffer is terminated
	
	for ( FillPtr = DestPtr; DestPtr < DestEnd; ++DestPtr )	// Fill remainder of Screeen Buffer with FillChar
        fputc(FillChar, stderr);
    
	for ( ; DestBuffer < DestPtr; --DestPtr )
        fputc(BkSpc, stderr);				// Backspace to start of DestBuffer Area on Screen

    alarm(_Yd);								// allows for a timeout on field entry presumably?

	fputs(*_vs ? _vs : _ve , stderr);		// vs == Make cursor *very* visible. ve == normal cursor visibility
	fflush(stderr);

	setvar(v_GOING_BACK, 0);				// "GOING_BACK" 41
    
	bypass = incvar(v_BYPASS, 0);			// incvar(42,0) returns current value of "BYPASS"
    
	if ( !bypass )
    {
        while ( 1 )
        {
            Key = gchar();
			//printf("linputChinese 200: Key = \"%C\", %d\n",isprint(Key) ? Key : ' ',Key);
            if ( Key == 2000 )
                goto LABEL_343;
			// _YT is set in CLINFO file. Allow TAB as input terminator? Y/N
            if ( _YT && Key == '\t' || Key == '\n' || Key == '\r' || Key == 2660 || Key == 2027 || Key == 2005 )
                break;
            if ( Key == _tty_bc )					// Archaic Backspace support
                Key = 2658;
            
			if ( iscntrl(Key)) 		//if ( (*__ctype_b_loc())[Key] & 0x0002 )	// re-map control codes iscntrl()
            {
                if ( Key == *(char *)_ya )			// <ctrl>a
                    Key = 2651;
                else if ( Key == *(char *)_yb )		// <ctrl>b
					Key = 2652;
                else if ( Key == *(char *)_yc )		// <ctrl>c
                    Key = 2653;
                else if ( Key == *(char *)_yd )		// <ctrl>d
                    Key = 2654;
                else if ( Key == *(char *)_ye )		// <ctrl>e
                    Key = 2655;
                else if ( Key == *(char *)_yf )		// <ctrl>f
                    Key = 2656;
                else if ( Key == *(char *)_yh )		// <ctrl>h
                    Key = 2658;
                else if ( Key == *(char *)_yi )		// <ctrl>i
					Key = 2659;
                else if ( Key == *(char *)_yl )		// <ctrl>l
					Key = 2662;
                else if ( Key == *(char *)_yp )		// <ctrl>p
					Key = 2666;
                else if ( Key == *(char *)_yr )		// <ctrl>r
					Key = 2668;
                else if ( Key == *(char *)_yt )		// <ctrl>t
                    Key = 2670;
                else if ( Key == *(char *)_yu )		// <ctrl>u
                    Key = 2671;
				else if ( Key == *(char *)_yv )		// <ctrl>v
					Key = 2672;
                else if ( Key == *(char *)_yw )		// <ctrl>w
					Key = 2673;
				else if ( Key == *(char *)_yx )		// <ctrl>x
					Key = 2674;
				else if ( Key == *(char *)_yz )		// <ctrl>z
					Key = 2676;
//printf("linputChinese 277: iscntrl(Key) Key = %d\n",Key);

            }
//-------------------------------------
//printf("linputChinese 249: Key = \"%C\", %d\n",Key,Key);

            if ( Key != 27 )
            {
                v45 = linpesc(Key, onesc);	// check to see if this key is 'escaped'
                if ( v45 )
                {
                    fflush(stderr);
                    break;
                }
            }
//-------------------------------------------
			switch (Key)
			{
			case 2048:
			case 2670:	// <ctrl>t
                if ( DestPtr == DestBuffer )
                {
                    if ( DestBuffer < FillPtr )
                    {
                        do
                        {
                            fputc(*DestPtr++, stderr);
                        }
                        while ( DestPtr < FillPtr );
                    }
                }
                else
                {
                    for ( ; DestBuffer < DestPtr; --DestPtr )
                        fputc(BkSpc, stderr);
                }
				break;

			case 2674:			// <ctrl> x handler
			case 2001:
				if ( DestPtr >= FillPtr )
					fputc(7, stderr);
				else
				{
		            v30 = (isDoubleWidth(*DestPtr) != 0) + 1;
		            for ( v50 = DestPtr; *v50; ++v50 )
		            {
						v31 = 0;
		                if ( &v50[v30] <= DestEnd )
							v31 = v50[v30];
	
						*v50 = v31;
						fputc(*v50 ? *v50 : FillChar, stderr);
					}
					*(DestEnd - 1) = 0;
					for ( FillPtr -= v30; DestPtr < v50; --v50 )
						fputc(BkSpc, stderr);
				}
				break;
			
			case 2003:
				while ( DestPtr < FillPtr )
                    fputc(*DestPtr++, stderr);
				break;

			case 2009:
				for ( ; DestBuffer < DestPtr; --DestPtr )
					fputc(BkSpc, stderr);
				break;

			case 2028:
				for ( ; DestBuffer < DestPtr; --DestPtr )
					fputc(BkSpc, stderr);
				v50 = FieldSave;
                while ( *v50 )
                {
					fputc(*v50, stderr);
					*DestPtr++ = *v50++;
				}
                for ( ; *DestPtr && DestPtr < DestEnd; ++DestPtr )
                {
					fputc(FillChar, stderr);
                    *DestPtr = 0;
				}
                for ( ; DestBuffer < DestPtr; --DestPtr )
					fputc(BkSpc, stderr);
				v45 = 27;					// <ESC>
				break;

			case 2041:
			case 2666:		// <ctrl>p
				if (!prscreen())
					fputc(7, stderr);	// ring bell if we couldn't print screen
				break;

			case 2042:
			case 2651:		// <ctrl>a
				if ( clabort != 2 )
                {
					fputs(_vi, stderr);
                    alarm(0);
                    return -17;
				}
				fputc(7, stderr);
				break;
			
			case 2659:		// <ctrl>i		insert a space at cursor position
			case 2024:
			case 2002:
				//printf("linputChinese 386 <ctrl>I: Key = \"%C\", %d, DestPtr = x%08X, FillPtr = x%08X, DestEnd = x%08X\n",Key,Key, DestPtr, FillPtr, DestEnd);
				if ( DestPtr >= FillPtr )
					fputc(7, stderr);	// DestBuffer full, ring the bell!
				else
				{
					if ( FillPtr == DestEnd )
					{
						//printf("linputChinese 392 <ctrl>I: FillPtr == DestEnd\n");
						FillPtr = DestEnd - 1;
					    //*(DestEnd - 1) = 0;
						*FillPtr = 0;
					    //if ( DestPtr < DestEnd - 1 )
					    if ( DestPtr < FillPtr )
					    {
							if ( curChineseChar(DestBuffer, DestEnd - 2) == 2 )
					        {
								FillPtr = DestEnd - 2;
					            //*(DestEnd - 2) = 0;
								*FillPtr = 0;
							}
						}
					}
					for ( v50 = FillPtr; DestPtr < v50; --v50 )	// shift remaining chars along
						*v50 = *(v50 - 1);
					*v50 = ' ';					// set cursor pos to <space>

					for ( ++FillPtr ; v50 <= FillPtr && v50 < DestEnd; ++v50 )	// Pad field out with FillChar
						fputc(*v50 ? *v50 : FillChar, stderr);

					for ( ; DestPtr < v50; --v50 )		// Backspace to insert position
						fputc(BkSpc, stderr);
				}
				break;

			case 2658:		// <ctrl>h
			case 2023:		// <backspace> (non-destructive)
			case 2006:		// <left arrow>	
				if ( DestBuffer >= DestPtr )	// At left hand side of the field
					fputc(7, stderr);			// ring bell and do nothing
				else
				{
					fputc(BkSpc, stderr);
					--DestPtr;
					if ( DestBuffer < DestPtr && curChineseChar(DestBuffer, DestPtr) == 4 )
					{
						fputc(BkSpc, stderr);
					    --DestPtr;
					}
				}
				break;

			case 2662:		// <ctrl>l
			case 2008:		// or right arrow
				if ( DestPtr < FillPtr )
                {
                    fputc(*DestPtr++, stderr);
                    if ( curChineseChar(DestBuffer, DestPtr) == 4 )
						fputc(*DestPtr++, stderr);
				}
				else
					fputc(7, stderr);	// can't right-arrow into uncharted waters :o)
				break;

			case 2656:		// <ctrl>f
			case 2047:
				if ( DestPtr >= FillPtr )
				{
					fputc(7, stderr);
					break;
				}
                v18 = gchar();
                v50 = DestPtr + 1;
                if ( v50 >= FillPtr )
				{
					fputc(7, stderr);
					break;
				}
                while ( *v50 != v18 )
				{
					++v50;
                    if ( v50 >= FillPtr )
					{
						fputc(7, stderr);
		                goto LABEL_340;
					}
				}
                if ( v50 >= FillPtr )
					fputc(7, stderr);
				else
				{
					fputc(*DestPtr++, stderr);
					while ( DestPtr < FillPtr && *DestPtr != v18 )
					    fputc(*DestPtr++, stderr);
				}

				break;

			case 2655:			// <ctrl>e
			case 2046:
				//printf("linputChinese 504: Key = \"%C\", %d, clabort = %d\n",Key,Key,clabort);
				if ( clabort != 2 )
                {
					fputs(_vi, stderr);
                    alarm(0);
                    return -5;
				}
                fputc(7, stderr);
				break;

			case 2654:		// <ctrl>d	== delete from current position to end of line
			case 2045:
				if ( DestPtr < FillPtr )
                {
					for ( v50 = DestPtr; *v50; ++v50 )
                    {
						fputc(FillChar, stderr);
                        *v50 = 0;
					}
                    for ( ; DestPtr < v50; --v50 )
						fputc(BkSpc, stderr);
					FillPtr = DestPtr;
				}
				else
					fputc(7, stderr);
				break;

			case 2712:
				fputs(_vi, stderr);		// vi == make the cursor invisible ?
                alarm(0);
                return -30;
				break;

			case 2043:
			case 2652:					// <ctrl>b
				if ( DestBuffer >= DestPtr )
				{
					fputc(7, stderr);
	                goto LABEL_340;
				}
                for ( ;; )
                {
					fputc(BkSpc, stderr);
                    --DestPtr;
                    if ( DestBuffer >= DestPtr )
						break;
					if ( (*__ctype_b_loc())[*DestPtr] & 8 || isChinese(DestBuffer, DestPtr) )
                    {
						while ( DestBuffer < DestPtr )
                        {
							if ( !((*__ctype_b_loc())[*DestPtr] & 8) && !isChinese(DestBuffer, DestPtr) )
                            {
								if ( DestBuffer >= DestPtr || (*__ctype_b_loc())[*DestPtr] & 8 || isChinese(DestBuffer, DestPtr) )
									goto LABEL_340;
								fputc(*DestPtr++, stderr);
				                goto LABEL_340;
							}
                            fputc(BkSpc, stderr);
                            --DestPtr;
						}
                        goto LABEL_340;
					}
				}
				break;

			case 2801:									// <ctrl>r == refresh screen
			case 2668:
				fputs(_vi, stderr);						// vi == make the cursor invisible
                clrefresh();
                at();
                
				for ( v50 = DestBuffer; *v50; ++v50 )	// Copy buffer to screen
					fputc(*v50, stderr);
                for ( ; v50 < DestEnd; ++v50 )			// Fill chars if required
					fputc(FillChar, stderr);
                for ( ; DestPtr < v50; --v50 )			// Backspace to current cursor position
					fputc(BkSpc, stderr);
                
				fputs(*_vs ? _vs : _ve, stderr);		// vs == Make cursor *very* visible. ve == normal cursor visibility
				break;

			case 2044:
			case 2653:									// <ctrl>c     [appears to be trapped by the O.S.]
				if ( DestPtr >= FillPtr )
					fputc(7, stderr);

				else if ( curChineseChar(DestBuffer, DestPtr) == 2 )
                {
                    fputc(*DestPtr++, stderr);
		            fputc(*DestPtr++, stderr);
				}
                else if ( curChineseChar(DestBuffer, DestPtr) == 1 )		// swap case
				{
					v14 = *DestPtr;
					if (islower(v14))		// islower()  ==	0x0200 
					    v17 = toupper(v14);
					else if (isupper(v14))	// isupper()  ==	0x0100
					    v17 = tolower(v14);
					else
					    v17 = *DestPtr;
					*DestPtr = v17;
					fputc(*DestPtr++, stderr);
				}
				break;

			case 2010:							// up-arrow
			case 2011:
			case 2672:							// <ctrl>v
				//printf("linputChinese 566 UP-ARROW: Key = \"%C\", %d : goback = %d\n",Key,Key, goback);
				if ( goback != 2 )
                {
					fputs(_vi, stderr);			// vi == make the cursor invisible ?
                    alarm(0);
                    setvar(v_GOING_BACK, 1);	// "GOING_BACK" 41
                    return -22;
				}
				fputc(7, stderr);				// bell
				break;

			case 2050:
			case 2673:						// <ctrl>w		??  Behaves similar to right-arrow
				if ( DestPtr >= FillPtr )
					fputc(7, stderr);

				else if ( (*__ctype_b_loc())[*DestPtr] & 8 || isChinese(DestBuffer, DestPtr) )	// isalnum()  == 0x0008
                    fputc(*DestPtr++, stderr);

				else
				{
					while ( DestPtr < FillPtr )
					{
						if ( !((*__ctype_b_loc())[*DestPtr] & 8) && !isChinese(DestBuffer, DestPtr) )
					    {
							while ( DestPtr < FillPtr && !((*__ctype_b_loc())[*DestPtr] & 8) && !isChinese(DestBuffer, DestPtr) )
					        {
					            fputc(*DestPtr++, stderr);
							}
					        goto LABEL_340;
						}
					    fputc(*DestPtr++, stderr);
					}
				}
				break;

			case 2671:							// <ctrl>u		-- Undo current changes
			case 2049:
				while ( DestBuffer < DestPtr )	// backspace to start of buffer
                {
					fputc(BkSpc, stderr);
                    --DestPtr;
				}
                v50 = FieldSave;
                while ( *v50 )					// copy saved buffer to dest
                {
					fputc(*v50, stderr);
                    *DestPtr++ = *v50++;
				}
                for ( FillPtr = DestPtr; *DestPtr && DestPtr < DestEnd; ++DestPtr )	// add fill chars as required
                {
					fputc(FillChar, stderr);
                    *DestPtr = 0;
				}
                for ( ; DestBuffer < DestPtr; --DestPtr )	// backspace to start of screen field
					fputc(BkSpc, stderr);
				break;

			case 27:							// <ESC>
				v36 = gchar();
                v45 = v36;
                if ( v36 != 27 )
                {
					if ( (unsigned short)(v36 - 65) <= 0x39u )
						goto LABEL_340;
					v45 = ' ';
                    goto LABEL_340;
				}
                for ( ; DestBuffer < DestPtr; --DestPtr )
					fputc(BkSpc, stderr);

                v50 = FieldSave;
                while ( *v50 )
                {
					fputc(*v50, stderr);
                    *DestPtr++ = *v50++;
                }
                for ( ; *DestPtr && DestPtr < DestEnd; ++DestPtr )
                {
					fputc(FillChar, stderr);
                    *DestPtr = 0;
				}
                for ( ; DestBuffer < DestPtr; --DestPtr )
					fputc(BkSpc, stderr);
				break;

			default:
				//printf("linputChinese 654: Key = \"%C\", %d\n",Key,Key);
				v38 = Key - 2900;
				if ( (unsigned int)(Key - 2900) <= 122 )
				{
					v45 = v38;
					if ((*__ctype_b_loc())[v38] & 0x0400 )	// isalpha() == 0x0400
					    goto LABEL_340;
					v45 = ' ';
					goto LABEL_340;
				}
				else if ( DestPtr >= DestEnd )			// cursor at right end of field
				{
					//printf("linputChinese 666 buffer full: Key = \"%C\", %d\n",Key,Key);
					fputc(7, stderr);
					if ( isDoubleWidth(Key) )
						gchar();
				}
				else if ( DestPtr >= FillPtr )			// we are adding *new* chars in the "fill" area
				{
					//printf("linputChinese FILLAREA: Key = \"%C\", %d\n",Key,Key);
				    if ( isDoubleWidth(Key) )
				    {
					    if ( (signed int)(DestEnd - DestPtr) <= 1 )
					    {
					        fputc(7, stderr);
					        gchar();
					        FillPtr = DestPtr;
					        goto LABEL_340;
					    }
					    fputc(Key, stderr);
					    *DestPtr++ = Key;
					    Key = gchar();
					}
					fputc(Key, stderr);					// Copy the char to screen, dest buffer, bump fillptr
					*DestPtr++ = Key;
					FillPtr = DestPtr;
				}	
				else if ( isDoubleWidth(Key) )			// should only be true for foreign language chars? 
				{
					//printf("linputChinese DOUBLEWIDTH: Key = \"%C\", %d\n",Key,Key);
				    if ( isDoubleWidth(*DestPtr) )
				    {
				        fputc(Key, stderr);
				        *DestPtr++ = Key;
				        Key = gchar();
				        fputc(Key, stderr);
				        *DestPtr++ = Key;
				    }
				    else if ( FillPtr == DestEnd && isDoubleWidth(DestPtr[1]) )
				    {
				        fputc(7, stderr);
				        gchar();
				    }
					else
					{
						fputc(Key, stderr);
						*DestPtr = Key;
						v39 = DestPtr + 1;
						v50 = v39;
						if ( FillPtr < DestEnd )
						{
						    for ( v50 = FillPtr; v39 < v50; --v50 )
						        *v50 = *(v50 - 1);
						    ++FillPtr;
						}
						*v50 = gchar();
						for ( DestPtr = v39 + 1; v50 <= FillPtr && v50 < DestEnd; ++v50 )
						    fputc(*v50 ? *v50 : FillChar, stderr);
						
						for ( ; DestPtr < v50; --v50 )
						    fputc(BkSpc, stderr);
					}
				}
				else	// not DoubleWidth(Key)	// overwrite *existing* chars
				{
					//printf("linputChinese OVERWRITE: Key = \"%C\", %d\n",Key,Key);
				    if ( curChineseChar(DestBuffer, DestPtr) != 2 )
					{
				        fputc(Key, stderr);
				        *DestPtr++ = Key;
					}
					else
					{
						fputc(Key, stderr);
						*DestPtr++ = Key;

						if ( DestPtr >= FillPtr )		// field buffer overrun
						    fputc(7, stderr);
						else if ( !isDoubleWidth(Key) )
						{
						    for ( v50 = DestPtr; *v50; ++v50 )
						    {
						        *v50 = v50[1];
						        fputc(*v50 ? *v50 : FillChar, stderr);
						    }
						    --FillPtr;
						    for ( ; DestPtr < v50; --v50 )
						        fputc(BkSpc, stderr);
						}
					}
				}
				break;
			}

LABEL_340:	// main end of loop tests
            if ( DestPtr == DestEnd && FullRetn || (fflush(stderr),v45) )
                break;
LABEL_343:
            if ( bypass )
                goto LABEL_346;
        }	// end while(1)
    } // end if ( !bypass )

    if ( bypass )
LABEL_346:
        incvar(v_BYPASS, -1);			// "BYPASS" --
    
	fputs(_vi, stderr);					// vi == make the cursor invisible ?
    alarm(0);							// cancel any pending timer 
    trim(DestBuffer);					// trim leading/trailing spaces if global var "trim_it" != 2
    
	if ( v45 )
        result = -v45;					// error exit?
    else
        result = strlen(DestBuffer);	// exit_success. return # chars in DestBuffer
    return result;
}

#endif
