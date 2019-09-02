#ifndef PRFIELD_C
#define PRFIELD_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <errno.h>		// for __errno_location()
#include <ctype.h>		// isspace() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

short _prfield(FILE *stream, PRTAB *prtab, FLDdesc *fld, char *Buffer, int fld_width, int align_centre, int align_left, int align_right)
{
    PRTAB	*v23;
    char	*v10;
    char	*j;
    char	*v19;
    char	*MessageString;

    signed int v20;
    int		MsgLen;
    short	LeadingSpaces;
    bool	addQuotes;
    char	src[2];
    char	v24;
    char	v34[16408];		// 16384 probably sufficient

	//printf("\n_prfield : Buffer = %s, fld_width = %d, align_left = %d, align_centre = %d, align_right = %d\n",Buffer,fld_width,align_left, align_centre,align_right);
    if ( rawprint )
        MessageString = Buffer;
    else
        MessageString = convstr(Buffer, true);		// encode control chars

//------------------------------------------------------

	if (align_left || align_centre || align_right)	// ensure data fills the field, and pad with spaces as required
	{
		if (align_left)					// align_left
			LeadingSpaces = 0;
		else if (align_centre || align_right)
		{
			LeadingSpaces = fld_width - strOnPaperLen(MessageString);	// align right
			if ( LeadingSpaces < 0 )
				LeadingSpaces = 0;

			if ( align_centre )			// align_centre
				LeadingSpaces /= 2;
		}

		for ( v10 = v34 ; LeadingSpaces ; LeadingSpaces-- )			// fill start spaces
			*v10++ = ' ';
		
		j = MessageString;
		while ( *j )												// copy message into buffer
			*v10++ = *j++;
		
		j = &v34[strOffPaperLen(MessageString, fld_width)];
		while ( v10 < j )											// fill trailing spaces
			*v10++ = ' ';

		*v10 = 0;
		MessageString = v34;
	}
//-----------------------------------------------------
    if ( pr_out & 1 ) // true if output stream is going to the screen (not a file).
    {
        if ( TAB )		// Add a TAB seperator char if we are the last print element *OR* <CR> doesn't follow us
        {
			if (!prtab || !prtab->NextPR || ((v23 = PRARR(prtab->NextPR)) && v23->ReservedFlags != 0x04))
            {
                src[0] = TAB;
                src[1] = 0;
                strcat(MessageString, src);
            }
        }
        toscreen(_cx, _cy, 0, lenstr(MessageString), MessageString, 0, 64, 1);
    }
	else	// we are sending output to a file!
	{
		//printf("file out, msg = %s\n",MessageString);
		if ( csv )
		{
			if ( *MessageString )
			{
				MsgLen = strlen(MessageString);
				if (isspace(*MessageString) || isspace(MessageString[MsgLen - 1]) || strchr(MessageString, '"') || strchr(MessageString, ',')	|| strchr(MessageString, '\n') )
				{
					addQuotes = true;
					if ( fputc('"', stream) == -1 )
						return *(int*)__errno_location();
				}
				else
					addQuotes = false;
				
				v19 = MessageString;				// output the string.  Any " double quote chars are shown as *two* double quotes ""
				for (v20 = 0 ; MsgLen > v20 ; v19++, v20++)
				{
					if ( (*v19 == '"' && fputc('"', stream) == -1) || fputc(*v19, stream) == -1 )
						return *(int*)__errno_location();
				}
				if ( addQuotes )
				{
					if ( fputc('"', stream) == -1 )
						return *(int*)__errno_location();
				}
			}
		}
		else
		{	
			if (MessageString[0] == '\\' && MessageString[1] == '0')	// fudge for outputting CHR(0)
			{
				//printf("file out, ZERO fudge\n");
				if ( fputc(0, stream) == -1 )
						return *(int*)__errno_location();

			}
			else if (*MessageString)
			{
				if ( fputs(MessageString, stream) == -1 )
					return *(int*)__errno_location();
			}
		}

		if ( csv )
		{
			// if next print item is a <CR>, return without printing seperator char
			if ( prtab && prtab->NextPR && (v23 = PRARR(prtab->NextPR)) && v23->ReservedFlags == 0x04)
				return 0;

			if ( TAB && TAB != ' ')
				v24 = TAB;	// if TAB set, use it as the separator, otherwise
			else
				v24 = ',';	// use comma

			if ( fputc(v24, stream) == -1 )
				return *(int*)__errno_location();
		}
		else	// if TAB is not set, *or* next print item is a <CR>, return without printing seperator char
		{
			if ( !TAB || prtab && prtab->NextPR && (v23 = PRARR(prtab->NextPR)) && v23->ReservedFlags == 0x04)
				return 0;
			if ( fputc(TAB, stream) == -1 )
				return *(int*)__errno_location();
		}
	}

	return 0;	// exit_success
}

short prfield(PRTAB *prtab)
{
    PRTAB *prtb;
	ENTAB *entb;
	FILE *stream;
	FLDdesc *fld;

    double v27;
	
    unsigned int v4;
	unsigned int v38;
	int FLDlen;
	int width_save;
	int v12;
	
	short Repeat_Width;
	short Repeat_Count;
    short v3;
	short elemno;
	
    bool pr4_bit_11;	// flag bits in prtab->WIDTH_FLAGS
    bool align_centre;
    bool align_right;
    bool align_left;
    bool varPrinted;
    bool eop_checked;
	
	char WidthVarType;
	char VarType;
	
    FLDdesc fld2;
    char fmt_mask[112];		// space to store any string mask for variable formatting
	char StrBuffer[8216];
	
	//char debugbuff[20];

    prtb = prtab;
    eop_checked = false;

	if ( pr_out & 1 )		// console out, save to screen buffer
        stream = stderr;
    else if ( pr_out & 2 )	// Output is being re-directed to a file, don't save screen output
		stream = stdout;
    else
		stream = op;		// Assume this is piped output?

    while ( prtb )			// prtab can define multiple items. Chains on prtb->NextPR
    {
        v4 = prtb->WIDTH_FLAGS;
										// 0000-0100-0000-0000   0x0400	// signifies numeric value?
        pr4_bit_11   = (v4 >> 11) & 1;	// 0000-1000-0000-0000   0x0800	// default?
        align_centre = (v4 >> 12) & 1;	// 0001-0000-0000-0000   0x1000	// centre aligned: ,c
        align_right  = (v4 >> 13) & 1;	// 0010-0000-0000-0000   0x2000	// right aligned:  ,r or ,$ 
        align_left   = (v4 >> 14) & 1;	// 0100-0000-0000-0000   0x4000 // left aligned:   ,l or ,^

//-------------------------------
        if ( prtb->ReservedFlags )		// is this PRTAB trying to output a 'reserved' value?
            fld = 0;
        else							// if not, get the variable data pointers
            fld = getftf(ENARR(prtb->VAR_ENTno), 1, &fld2);
		
		//printf("\n_prfield 280: v4 = %s\n",DumpShortBits(v4, debugbuff));
		//printf("_prfield 285: (VAR_ENTno = %3d, WIDTH_ENTno = %3d, WIDTH_FLAGS = x%04X, ReservedFlags = x%04X\n",prtb->VAR_ENTno,prtb->WIDTH_ENTno,prtb->WIDTH_FLAGS,prtb->ReservedFlags);
//----------------------------
		// find the expression which defines what we are outputting.
		//if ( prtb->VAR_ENTno && (entb = &enarr.TableAddr[prtb->VAR_ENTno - 1]) != 0 )
		if ( (entb = ENARR(prtb->VAR_ENTno)) != 0 )
            VarType = getetype(entb);
        else
            VarType = 0;
//-----------------------------
		// Find the expression which defines the required width, or number of times to repeat (\n)
        //if ( prtb->WIDTH_ENTno && (entb = &enarr.TableAddr[prtb->WIDTH_ENTno - 1]) != 0 )
        if ( (entb = ENARR(prtb->WIDTH_ENTno)) != 0 )
            WidthVarType = getetype(entb);
        else
            WidthVarType = 0;
//------------------------------        
		if ( WidthVarType == 'C' )				// string variable (or static) usually means a "mask" type
        {
            //exptobuf(fmt_mask, ENARR(prtb->WIDTH_ENTno), 0);
            exptobuf(fmt_mask, entb , 0);
            Repeat_Width = 0;
        }
        else
        {
            if ( prtb->WIDTH_ENTno )			// variable describes the width, evaluate as integer
                Repeat_Width = evalint( entb );
                //Repeat_Width = evalint(&enarr.TableAddr[prtb->WIDTH_ENTno - 1]);
            else
                Repeat_Width = prtb->WIDTH_FLAGS & 0xFFFF83FF;       // 1000-0011-1111-1111	mask out bits 14 downto 10
            
			if ( !Repeat_Width && prtb->WIDTH_FLAGS & 0x0400 && fld )
                Repeat_Width = getdwid(fld);
        }
//----------------------------
        width_save = Repeat_Width;
        if ( align_centre || align_left || align_right || !justify && pr4_bit_11 )
            Repeat_Width = 0;

//----------------------------
        varPrinted = false;
        v12 = prtb->ReservedFlags;
		switch (v12)
		{
		// These are all reserved name special flags:
		case 0x0008:	// 'date'				= sysdate()
		case 0x0010:	// 'time'				= systime()
		case 0x0020:	// 'pname'
		case 0x0040:	// 'uname'
		case 0x0080:	// 'pid'
		case 0x0100:	// 'uid'
		case 0x0200:	// 'login'
		case 0x0400:	// 'tty'
		case 0x0800:	// 'nname'				= getnodename()
		case 0x4000:	//          ?????
            if ( Repeat_Width )
                sprintf(StrBuffer, "%-*.*s", Repeat_Width, Repeat_Width, getresv(v12));
            else
                cdbcpystr(StrBuffer, getresv(v12), 0);
			break;

		case 0x0001:				// 'cl' or clear line	*** doesn't trigger end of page processing ***
            erase_line();
            for ( Repeat_Count = Repeat_Width - 1; Repeat_Count > 0; --Repeat_Count )	// in this case, Repeat_Width is a repeat counter
            {
                _cx = 1;
                ++_cy;
                if ( _li < _cy )	// ran off the end of the page, reset to start of screen.  *** should stay on last line instead?? ***
                    _cy = 1;
                erase_line();
            }
            fflush(stderr);
            varPrinted = true;
			break;

		case 0x0002:				//	"\f" <Form Feed> 
			if ( pr_out & 1 )						// ignore form feed if we are talking to a console
				varPrinted = true;
			else if ( fputc('\f', stream) == -1 )
			    return *(int*)__errno_location();	// error_exit
			break;

		case 0x0004:				// "\n" <New Line>
			if ( !eop_checked )
				chkeop();

			if ( pr_out & 1 )		// output is a console
			{
				
				++_cy;				// bump current line number
				_cx = 1;			// reset column to left hand side of screen

				incvar(v_LCOUNT, 1);		// LCOUNT 19
				for ( Repeat_Count = Repeat_Width - 1; Repeat_Count > 0; --Repeat_Count )
				{
					chkeop();
					++_cy;
					if ( _li <= _cy )
						_cy = 1;
					incvar(v_LCOUNT, 1);	// "LCOUNT"
				}
				eop_checked = false;
				if ( _li <= _cy )
				    _cy = 1;
			    at();
			    varPrinted = true;
			}
			else					// output is a file or piped
			{
				if ( dos && fputc('\r', stream) == -1 || fputc('\n', stream) == -1 )	// dos mode. \n == (\r + \n)
					return *(int*)__errno_location();
				incvar(v_LCOUNT, 1);	// "LCOUNT"
				
				Repeat_Count = Repeat_Width - 1;		// Repeat_Width == number of times to repeat here. ZERO if not specified
				if ( Repeat_Count <= 0 )
				{
					eop_checked = false;
					varPrinted = true;
					goto LABEL_188;
				}
				while ( 1 )
				{
					chkeop();
					
					if ( dos && fputc('\r', stream) == -1 || fputc('\n', stream) == -1)	// <cr> and <\n> for dos, only <\n> for Linux/unix/osx
						return *(int*)__errno_location();	// error exit
					incvar(v_LCOUNT, 1);	// "LCOUNT"
					
					--Repeat_Count;
					if ( Repeat_Count <= 0 )
					{
						eop_checked = false;
						varPrinted = true;
						goto LABEL_188;
					}
				}
			}
			break;

		case 0x1000:			// = 'cr' Carriage return sequence
            fflush(stream);
            if ( !eop_checked )
            {
                chkeop();
                eop_checked = true;
            }
            if ( pr_out & 1 )						// talking to a console, move cursor to left hand side of screen
            {
                _cx = 1;
			    at();
            }
			else if ( fputc('\r', stream) == -1 )	// talking to file/pipe
				return *(int*)__errno_location();	// error_exit
            
			varPrinted = true;
			break;
		
		case 0x2000:
            return 0x2000u;			// Passed up to _write(). closes the open FILE*
			break;

		default:
			if ( fld && fld->FLDstat & fld_ARRAY )	// $0002; Trying to print whole Array (no subscript entered)
			{
				FLDlen = (fld->FLDtype == 'C') ? fld->FLDlen + 1 : sizeof(double);
				
				fld->FLDdata = fld->FLDname;	// Array vars hang off FLDname
				elemno = 0;
				while (fld->FLDelemID > elemno)	// fld->FLDelemID == size of array
				{
					switch (VarType)
					{
					case 'D':
					    if ( WidthVarType == 'C' )		// format mask
					        dfmt(StrBuffer, fmt_mask, *(double *)fld->FLDdata);
					    else
					        fldtobuf(StrBuffer, fld, Repeat_Width);
					    if ( csv )
					        trim(StrBuffer);
						break;

					case 'T':
				        if ( WidthVarType == 'C' )		// format mask
				            tfmt(StrBuffer, fmt_mask, *(double *)fld->FLDdata);
						else
							fldtobuf(StrBuffer, fld, Repeat_Width);
						break;

					case 'C':
						fldtobuf(StrBuffer, fld, strOffPaperLen(fld->FLDdata, Repeat_Width));
						break;

					default:
				        if ( !csv && WidthVarType == 'C' )		// **** this means that you can't use a format mask with csv files ****!!!
							nfmt(StrBuffer, fmt_mask, *(double *)fld->FLDdata);		// **** no rounding to array variables ****
						else
							fldtobuf(StrBuffer, fld, Repeat_Width);
						break;
					}

					if ( !eop_checked )
				    {
						chkeop();
				        eop_checked = true;
					}
				    v3 = _prfield(stream, 0, fld, StrBuffer, Repeat_Width, align_centre, align_left, align_right);
				    if ( v3 )
						return v3;
				    elemno++;
				    fld->FLDdata = (char *)fld->FLDdata + FLDlen;	// bump to next array element's data area
				}
				varPrinted = true;
			}
			else	// Normal variable or Table field
			{
				//printf("default variable, VarType = %C\n",VarType);
				switch (VarType)
				{
				case 'D':
				    if ( WidthVarType == 'C' )
				        dfmt(StrBuffer, fmt_mask, evalnum(ENARR(prtb->VAR_ENTno), VarType));
				    else
				        exptobuf(StrBuffer, ENARR(prtb->VAR_ENTno), Repeat_Width);

					if ( csv )
				        trim(StrBuffer);
					break;
				
				case 'T':
				    if ( WidthVarType == 'C' )
				        tfmt(StrBuffer, fmt_mask, evalnum(ENARR(prtb->VAR_ENTno), VarType));
					else
						exptobuf(StrBuffer, ENARR(prtb->VAR_ENTno), Repeat_Width);
					break;

				case 'C':
					if ( csv || !Repeat_Width)
					{
					    exptobuf(StrBuffer, ENARR(prtb->VAR_ENTno), 0);
					}
					else
					{
						v38 = 3 * Repeat_Width;
						if ( v38 >= 8192 )	// ensure we don't overrun buffer!
						    v38 = 8191 ;
						exptobuf(StrBuffer, ENARR(prtb->VAR_ENTno), v38);
						StrBuffer[strOffPaperLen(StrBuffer, Repeat_Width)] = 0;
					}
					break;
				default:					// all other variable types
					//printf("default, fmt_mask = %s, WidthType=%C, Width=%d\n",fmt_mask,WidthVarType,Repeat_Width);
					if ( csv )													// **** this means that you can't use a format mask with csv files ****!!!
					{
					    exptobuf(StrBuffer, ENARR(prtb->VAR_ENTno), 0);
					}
					else if ( WidthVarType == 'C' )	// format mask
					{
						v27 = evalnum(ENARR(prtb->VAR_ENTno), VarType);
						nfmt(StrBuffer, fmt_mask, clround(v27, VarType));		// apply mask and rounding.
					}
					else
					{
					    exptobuf(StrBuffer, ENARR(prtb->VAR_ENTno), Repeat_Width);
					}
					break;
				}
			}
			break;
		}

LABEL_188:
        if ( !varPrinted )
        {
            if ( !eop_checked )
            {
                chkeop();
                eop_checked = true;
            }
            v3 = _prfield(stream, prtb, fld, StrBuffer, width_save, align_centre, align_left, align_right);
            if ( v3 )
                return v3;	// error_exit. normally -1
        }
        prtb = PRARR(prtb->NextPR);
    } // end while(prtb)
	fflush(stream);
	return 0;			// exit_success!
}

#endif

