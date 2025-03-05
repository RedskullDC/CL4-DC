#ifndef MODFLD_C
#define MODFLD_C

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

bool fcheck(char *a1, XTAB *xtb, int noScreenOut)
{
    char v3;
    char a2[2];

//    if ( noScreenOut || SLOBYTE(xtb->Flags) >= 0 || (v3 = isfname(a1), *(short *)a2 = (unsigned char)v3 | *(short *)a2 & 0xFF00, !v3) )
    //if ( noScreenOut || !(xtb->Flags & 0x80) || (v3 = isfname(a1), *(short *)a2 = (unsigned char)v3 | *(short *)a2 & 0xFF00, !v3) )
    if ( noScreenOut || !(xtb->Flags & xtb_NAME) || (v3 = isfname(a1), *(short *)a2 = (unsigned char)v3 | *(short *)a2 & 0xFF00, !v3) )
        return true;	// exit_success

	else
    {
        a2[1] = 0;
        prerr("invalid character [%s]", a2, 1);
        return false;	// exit_failure
    }
}

void dstrip(char *a1)
{
    char *v1;
	char *v2;
	
    v1 = a1;
    if ( *a1 )
    {
        if ( *a1 == RADIX_CHAR[0] )
        {
LABEL_5:
            if ( *v1 )
            {
                v2 = &a1[lenstr(a1) - 1];
                while ( v1 <= v2 && (*v2 == '0' || *v2 == RADIX_CHAR[0]) )
	                *v2-- = 0;
            }
        }
        else
        {
            while ( 1 )
            {
                ++v1;
                if ( !*v1 )
                    break;
                if ( *v1 == RADIX_CHAR[0] )
                    goto LABEL_5;
            }
        }
    }
}

void dispatt(unsigned short a1)
{
  unsigned short v1;
  const char *v2;

  v1 = getattr(a1, 1);
  switch ( v1 )
  {
    case 0x01:
      v2 = _fs;
      break;
    case 0x02:
      v2 = _so;
      break;
    case 0x04:
      v2 = _us;
      break;
    case 0x08:
      v2 = _bo;
      break;
    case 0x20:
      v2 = _rs;
      break;
    default:
      v2 = 0;
      break;
  }
  if ( v2 )
    fputs(v2, stderr);
}

int modfld(BTAB *btb, PTAB *ptab, FLDdesc *fld, int mySkip, char **a5, int a2)
{
    RTAB *rtab;
	XTAB *xt;
	
    double v25;
	double v37;
	
	int		fno;
	int		fld_type;
	int		v39;
	int		v47;
    int		v51;
	int		forceUpdate;
	int		v56;
	short	MaxLen;
	int		v61;
	int		TTno;
	
	short	Column;
	short	Line;
    
	char	LineBuff[1008];
	char	format[1024];
	char	v65[64];
	
	char	WildChar;
	char	FLDtype;
	
	bool	validNum; // esi@169
	bool	v48;
    bool	AllowLookup;
	bool	ClrMsgLine = false;
    bool	v52 = false;
	
//eprint("modfld(BTAB *btb, PTAB *ptab, FLDdesc *fld,mySkip = %d, char **a5, a2 = %d)\n", mySkip, a2);

    v47 = 0;
    ++no_fields;	// a lovely global
    
	if ( skipping )
        mySkip = skipping == 1;
    
	*a5 = LineBuff;	// out of scope by time we return?

    xt = XTARR(ptab->TABno);

	v56 = 0;
    WildChar = 0;

	fno = gettdfno(ENARR(xt->VarExpNo), &TTno, &FLDtype, 0);
	assert(fno >= 0);

//printf("modfld #215 - fno = %d, TTn0 = %d, FLDtype = %C\n",fno,TTno,FLDtype);
//printf("modfld btb->TTno = %d, fld->FLDstat = x%04X, btb->Curr_Mode = %C\n",btb->TTno,fld->FLDstat,btb->Curr_Mode);

	//Maintain block for this Table && Key Field && Mode == Change, Delete or View 
    if ( btb->TTno == TTno && fld->FLDstat & fld_KEY && (btb->Curr_Mode == 'c' || btb->Curr_Mode == 'd' || btb->Curr_Mode == 'v') )
        v52 = true;

//------------------   
	AllowLookup = false;
    //if ( btb->Curr_Mode != 'a' || btb->TTno != TTno || !(fld->FLDstat & 0x8000) || xt->Flags & 0x0010 )
    if ( btb->Curr_Mode != 'a' || btb->TTno != TTno || !(fld->FLDstat & fld_KEY	) || xt->Flags & xtb_JOIN )
    {
        if ( xt->Flags & xtb_NOJOIN )
            AllowLookup = false;
		else
			AllowLookup = true;
    }
//------------------
    if ( btb->Curr_Mode != 'a' || xt->Flags >= 0 || btb->TTno != TTno )
        v48 = false;
    else
    {
        v48 = true;
        //if ( (xt->Flags & 0x110) == 0x0010 )
        if ( (xt->Flags & (xtb_JOIN|xtb_NOJOIN)) == xtb_JOIN )
        {
            rtab = RTARR(xt->RangeID);
            gettdfno(ENARR(rtab->enleft), &v61, &FLDtype, 0);
			//x86 checked, doesn't test return code of this gettdfno() call.
            //if ( fno < 0 )	// should be unreachable fno is tested above.
            //    __assert_fail("fno >= 0", "modfld.c", 118, "modfld");
			//assert(fno >= 0);

			v48 = TTno == v61;
            if ( v48 )
                AllowLookup = false;
        }
    }

//printf("modfld #248 - v52 = %d, AllowLookup = %d, v48 = %d \n",v52,AllowLookup,v48);
//------------------------------------
    MaxLen = getwid(xt);
    clgetyx(xt, &Column, &Line, yx_answer);		// get the column/line for where the *answer* shall be
    whereat(Column, Line, 1, MaxLen);
    
	if ( fld->FLDtype == 'C' && fld->FLDlen < (signed short)MaxLen )
        MaxLen = fld->FLDlen;
//printf("modfld #188 : fld->FLDlen = %d, MaxLen = %d, Column = %d, Line = %d\n",fld->FLDlen,MaxLen, Column, Line);
    
	xt->C_X = _cx;	// update current coords. clgetyx() may have evaluated an expression!
    xt->C_Y = _cy;
    
	if ( mySkip && incvar(v_BYPASS, 0) )		// Skip in ON && "BYPASS" !=0, decrement the bypass count
        incvar(v_BYPASS, -1);

//=====================================    
	fld_type = fld->FLDtype;
    if ( fld_type == 'D' || fld_type == 'T' )
    {
		//printf("modfld Date/Time Handler \n");
        while ( 1 )
        {
            while ( 1 )
            {
                v51 = 0;
                forceUpdate = 0;
				memset(LineBuff, 0, 20u);
                fldtobuf(LineBuff, fld, 0);
                if ( fld->FLDtype == 'D' && strlen(LineBuff) > MaxLen )
                {
                    MaxLen = 10;
                    xt->width = 10;
                }
                if ( !mySkip )
                {
                    while ( 1 )
                    {
                        forceUpdate = 1;
                        if ( _sg )
                            dispfld(xt, fld, 1, 0, 0, 0);
                        else
                            dispatt(xt->Attr);
                        
						whereat(Column, Line, 0, MaxLen);
                        at();
                        
						v56 = 0;
                        WildChar = 0;
                        
						v47 = linput(LineBuff, _YA ? -MaxLen : MaxLen, xt->onesc);  // YA allows "auto-accept fields". (Return when buffer full)

                        if ( v47 > 0)			// v47 +ve == number of chars in the buffer
                        {
                            WildChar = LineBuff[v47 - 1];	// Look at last char in Buffer for wildcard
                            if ( WildChar != '*' && WildChar != '+' && WildChar != '-' )
	                            WildChar = 0;
                        }
                        else if ( v47 < 0 )	// v47 -ve == error, or exception condition
						{
							switch (v47)	// Error Handler
							{
								case -27:
								    v56 = 1;
								    break;
								case -26:
								    v56 = 1;
								    v47 = 0;
									WildChar = 0;
								    break;
								case -30:				// key# 2712			- pressed, cursor invisible
								case -22:				// key# 2010,2011,2672  - <ctrl>v or up-arrow
								case -17:				// key# 2042,2651		- <ctrl>a
								case -5:				// key# 2046,2655		- <ctrl>e
									dispfld(xt, fld, v47 == -22, 1, 0, 0);
									skipping = 0;
									return v47;			// exit
								    break;
								default:				// unknown error code
								    v51 = 1;
								    break;
						    }
						}
						// v47 == 0 falls straight through here 
                        if ( WildChar )					
							LineBuff[v47 - 1] = 0;	// Remove valid Wildcards (*,+,-) from end of Buffer. 
                        
						v25 = 0.0;
						if ( LineBuff[0] )	// Look for fudgey single char override commands
                        {
                            if ( fld->FLDtype == 'D' )
                            {
                                if ( v47 == 1 && LineBuff[0] == '.')
                                    LineBuff[0] = 't';						// 'today'
                                v25 = (double)clgetdate(LineBuff);
                            }
                            else
                            {
                                if ( v47 == 1 && LineBuff[0] == '.')
                                    LineBuff[0] = 'n';						// 'now'
                                v25 = gettime(LineBuff);
                            }
                        }

                        if ( v25 < 0.0 )
                        {
							prerr("invalid %s", fld->FLDtype == 'D' ? "date" : "time", 1);
                            ClrMsgLine = true;
                        }
						else
						{
                            if ( !mySkip )
                            {
                                *(double *)fld->FLDdata = v25;
                                if ( v25 == 0.0 )
                                    fld->FLDstat |= fld_ZERO;	// 0x0004;
                                else
                                    fld->FLDstat &= ~fld_ZERO;	// 0xFFFB;
                            }
                            goto LABEL_149;
                        }
                        if ( mySkip )		// never set to true in this loop!
                            goto LABEL_149;
                    }
                }
LABEL_149:
                if ( !v51 )
                    break;
                
				if ( ClrMsgLine )
                    prerr(0, 0, 0);
                
				if ( chkesc(xt->onesc, -v47, &btb->Curr_Mode) == -5 )
                    return -5;
            }
            if ( (!WildChar || AllowLookup) && (v52 && WildChar || v48 || jcheck(ptab, fld, WildChar, 1)) && (v52 && WildChar || rcheck(xt, fld, a2)) )
                break;
            if ( WildChar && !AllowLookup )
                prerr("table lookup not allowed", 0, 1);
            recalt = true;
            ClrMsgLine = true;
            mySkip = 0;
        }
        fld->FLDstat |= fld_DATA_AVAIL;	// 0x0010u;
        dispfld(xt, fld, 1, forceUpdate, 0, 0);

		if ( WildChar )
	        cdbcpystr(LineBuff, LineBuff, (WildChar == '-' ? "-" : "+"), 0);
    }
//===========================================    
	else if ( fld_type == 'C' )	// string handler
    {
		//printf("modfld String Handler \n");
        while ( 1 )
        {
            v51 = 0;
			memset(LineBuff, 0,MaxLen + 1);
            
			if ( rawdisplay )
				cdbcpystr(LineBuff, (char *)fld->FLDdata, 0);
            else
				cdbcpystr(LineBuff, convstr((char *)fld->FLDdata, true), 0);	// encode control chars
    
			if ( mySkip )
                goto LABEL_85;
            if ( _sg )
                dispfld(xt, fld, 1, 0, 0, 0);
            else
                dispatt(xt->Attr);
            whereat(Column, Line, 0, MaxLen);
            at();
            
			v56 = 0;
            WildChar = 0;

			// YA allows "auto-accept fields". (Return when buffer full)
			v47 = linput(LineBuff, _YA ? -MaxLen : MaxLen, xt->onesc);
            if ( v47 > 0)				// v47 +ve == number of chars in the buffer
            {
				WildChar = LineBuff[v47 - 1];	// Look at last char in Buffer for wildcard
                if ( WildChar != '*' && WildChar != '+' && WildChar != '-' )
					WildChar = 0;
            }
            else if ( v47 < 0 )		// v47 -ve == error, or exception condition
            {
                switch (v47)
                {
                    case -27:
                        v56 = 1;
                        break;
                    case -26:
                        v56 = 1;
                        v47 = 0;
                        WildChar = 0;
                        break;
                    case -30:
                    case -22:
                    case -17:
                    case -5:
						dispfld(xt, fld, v47 == -22, 1, 0, 0);
						skipping = 0;
						return v47;			// exit
						break;
                    default:
                        v51 = 1;
                        break;
                }
            }
			// v47 == 0 falls straight through here 
			if ( rawdisplay )
				cdbcpystr((char *)fld->FLDdata, LineBuff, 0);
            else
				cdbcpystr((char *)fld->FLDdata, convstr(LineBuff, false), 0);	// decode control chars?

            if ( LineBuff[0] )	// quick zero flag
                fld->FLDstat &= ~fld_ZERO;	// 0xFFFBu;
            else
                fld->FLDstat |= fld_ZERO;	// 0x0004u;

            if ( v51 )
            {
                if ( ClrMsgLine )
                    prerr(0, 0, 0);	// clear message line

                if ( chkesc(xt->onesc, -v47, &btb->Curr_Mode) == -5 )
                    return -5;
            }
            else
            {
                dispfld(xt, fld, 1, 1, 0, 0);
LABEL_85:
                if ( (!WildChar || AllowLookup) && (v52 && WildChar || v48 || jcheck(ptab, fld, WildChar, 1)) 
					&& (v52 && WildChar || rcheck(xt, fld, a2)) && (v52 && WildChar || fcheck((char *)fld->FLDdata, xt, a2)) )
                {
                    fld->FLDstat |= fld_DATA_AVAIL;	// indicate that field now has valid data in buffer area
                    dispfld(xt, fld, 1, 0, 0, 0);	// copy field data to screen
					break;							// success_ exit out
                }
                if ( WildChar && !AllowLookup )
                {
                    sprintf(format, "'%s' table lookup not allowed", fldtobuf(v65, fld, 0));
                    prerr(format, 0, 1);
                    *(char *)fld->FLDdata = 0;
                }
                recalt = true;
                ClrMsgLine = true;
                mySkip = 0;
            }
        }
    }
//======================================
	else	// default numeric field handler
	{
		while ( 1 )
		{
			//printf("modfld Default number Handler \n");
			// default handler, all others must be numbers
			while ( 1 )
			{
				v51 = 0;
				forceUpdate = 0;
				
				memset(LineBuff, 0,MaxLen + 1);
				fldtobuf(LineBuff, fld, 0);
				
				dstrip(LineBuff);				// remove any thousands seperator chars, leading zeroes
				
				validNum = false;
		        while ( !validNum )
				{
				    if ( !mySkip )
				    {
				        forceUpdate = 1;
				        if ( _sg )										// number of blank characters left by smso or rmso
				            dispfld(xt, fld, 1, 0, 0, 0);
				        else
				            dispatt(xt->Attr);
				        
						whereat(Column, Line, 0, MaxLen);				// Locate cursor on screen
				        at();
				        v56 = 0;
				        WildChar = 0;

						v47 = linput(LineBuff, _YA ? -MaxLen : MaxLen, xt->onesc);	// YA allows "auto-accept fields". (Return when buffer full)
			            if (v47 > 0)					// v47 +ve == number of chars in the buffer
			            {
			                WildChar = LineBuff[v47 - 1];	// Look at last char in Buffer for wildcard
			                if ( WildChar != '*' && WildChar != '+' && WildChar != '-' )
			                    WildChar = 0;
			            }
			            else if (v47 < 0)			// v47 -ve == error, or exception condition
			            {
			                switch (v47)
			                {
			                    case -27:
			                        v56 = 1;
			                        break;
			                    case -26:
			                        v56 = 1;
			                        v47 = 0;
									WildChar = 0;
			                        break;
			                    case -30:
			                    case -22:
			                    case -17:
			                    case -5:
									dispfld(xt, fld, v47 == -22, 1, 0, 0);
						            skipping = 0;
				                    return v47;
									break;
		                        default:
		                            v51 = 1;		// unknown error code
		                            break;
		                    }
		                }
						// v47 == 0 falls straight through here 
		                if ( WildChar )
							LineBuff[v47 - 1] = 0;
		            }
					//printf("modfld 573 - LineBuff = %s\n",LineBuff);
		            if ( echknum(LineBuff, fld->FLDtype))	// Is this a valid number?
		                validNum = true;
		            else
		                ClrMsgLine = true;
		        }	// end while ( !validNum )
//-------------------------------
		        if ( !mySkip )
		        {
		            v37 = clround(strtod(LineBuff, 0), fld->FLDtype);
					//printf("modfld 583 - v37 = %G\n",v37);
	                *(double *)fld->FLDdata = v37;
	                if ( v37 == 0.0 )
	                    fld->FLDstat |= fld_ZERO;	// 0x0004u;
	                else
	                    fld->FLDstat &= ~fld_ZERO;	// 0xFFFBu;
	                fld->FLDstat |= fld_DATA_AVAIL;	// 0x0010u;
	            }
	            if ( !v51 )
	                break;	// success, jump out
	            if ( ClrMsgLine )
	                prerr(0, 0, 0);	// clear message line
	
			    if ( chkesc(xt->onesc, -v47, &btb->Curr_Mode) == -5 )
			        return -5;
			}
			if ( (!WildChar || AllowLookup) && (v52 && WildChar || v48 || jcheck(ptab, fld, WildChar, 1)) && (v52 && WildChar || rcheck(xt, fld, a2)) )
			    break;	// success, jump out.
        
			if ( WildChar && !AllowLookup )
			    prerr("table lookup not allowed", 0, 1);

		    recalt	= true;
		    ClrMsgLine = true;
		    mySkip	= 0;
		}
		dispfld(xt, fld, 1, forceUpdate, 0, 0);

		if ( WildChar )
	        cdbcpystr(LineBuff, LineBuff, (WildChar == '-' ? "-" : "+"), 0);
	}

//------------------------------
    if ( ClrMsgLine )
        prerr(0, 0, 0);	// quick way to blank the message line
    
	if (v56)
		return v47 ? -27 : -26 ;
    
	return 1;			// exit_success
}

#endif

