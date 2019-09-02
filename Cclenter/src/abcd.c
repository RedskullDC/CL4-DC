#ifndef ABCD_C
#define ABCD_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

bool hasass(int TTno)
{
    FLDdesc *fld; // eax@1

	for ( fld = ttab[TTno].TTfields ; fld->FLDelemID ; fld++ )
	{
        if ( fld->FLDstat & fld_SUBREC)		// 0x0008 Does this TT have any sub-keys?
			return true;
    }
    return false;
}

int abcd(BTAB *btb, short num_fields)
{
    SCR		*scrn;
	BTAB	*btb1;
	PTAB	*ptb1;
	//ENTAB	*v25;
	FLDdesc *FLD_reply; 
    FLDdesc *FLD_mode;
    
	char	*v15;
	char	*v27;
	char	*v28;
	char	*mode_txt;
	
    int		CurrMODE;
	int		TDno;
	int		a7;
	int		v44;
	
    short	v4;
	signed short v9;
	short	PT_End;
	short	PT_Start;
	short	PT_End2;
	short	PT_Start2;
	short	ScreenNum;
	bool	v40;
	bool	v41;
	bool	v42;
	
    char	v45;
	char	SrchChar;
    char	v29;
	char	v31;
	short	ScreenFields[10];	// should be an array?
	
    v31 = 0;
    a7  = 0;
    v40 = false;
    v41 = false;
    v42 = true;

    FLD_mode	= &ttab->TTfields[v_MODE];			// Points to MODE variable
	FLD_reply	= &ttab->TTfields[v_REPLY];			// Points to REPLY variable
    
	ScreenFields[0]	= num_fields;
    TDno		= btb->TTno;

    PTARR(getptabp(btb->StartLine));	// appears pointless, but will throw an assert if StartLine is out of bounds

    ScreenNum = 0;
    v4 = 0;
    do
    {
        while ( 1 )
        {
            while ( 1 )
            {
LABEL_2:
                var_cr = false;
                v29 = *(char *)FLD_mode->FLDdata;
                if ( v29 )
                {
                    if ( isupper(v29) )
                    {
                        btb->BTFlags = false;
                        if ( v29 == 'E' )
                        {
                            *(char *)FLD_mode->FLDdata = 'e';
                            FLD_mode->FLDstat &= ~fld_ZERO;	// 0xFFFBu;
                            return 0;
                        }
                        btb->Curr_Mode = tolower(v29);
                    }
                    else if ( btb->Curr_Mode != *(char *)FLD_mode->FLDdata )
                    {
						if ( ScreenNum )
							dscreen(btb, 0, 0);		// looks pointless, but sets/updates global nflds variable
                        btb->BTFlags = false;
						return 0;
                    }
                }
				//eprint("abcd107\n");

                CurrMODE = btb->Curr_Mode;
				switch (CurrMODE)
				{
				case 'c':
					mode_txt = "CHANGE";
					break;
				case 'd':
					mode_txt = "DELETE";
					break;
				case 'v':
					 mode_txt = "VIEW";
					break;
				case 'a':
					mode_txt = "ADD";
					break;
				default:
					mode_txt = "";
					break;
				}
                exit_type = 0;
                ScreenNum = 0;
                if ( (btb->Scrs & 0xBFFFu) > 0 )	// 1011 1111 1111 1111   0x4000
                    break;

LABEL_167:
                if ( btb->Curr_Mode == 'r' && !(btb->Scrs & 0x4000) )
                    endtran();
            } // end 1st inner loop

LABEL_25:
//========================================
//eprint("abcd: LABEL_25 \n");

            if ( !ScreenNum )
            {
                v40 = false;
                v41 = false;
            }
            *(char *)FLD_mode->FLDdata = btb->Curr_Mode;
            FLD_mode->FLDstat &= ~fld_ZERO;	// 0xFFFBu;
            
			scrn = (SCR*)btb->Screens;
			for (v9 = 0; v9 < ScreenNum ; v9++)
                scrn = scrn->NextSCR;
            
			PT_Start	= scrn->PT_start;
            PT_Start2	= scrn->PT_start;
            PT_End		= scrn->PT_end;
            PT_End2		= scrn->PT_end;

			if ( PTARR(getptabp(PT_Start2))->OpCode != 600 ) // 600 == format block_name
                break;

			while ( 1 )
            {
LABEL_48:
				if ( exit_type == 882 )				// exit_block		** exit_type global var **
				{
					exit_type = 0;
					setcvar(v_REPLY, "Q");	// "REPLY"
					setcvar(v_MODE, "E");	// "MODE"
					v31 = 'q';
				}
				else if ( exit_type == 886 )		// exit_process		** exit_type global var **
				{
					setcvar(v_REPLY, "E");
					v31 = reply(0, "e");
				}
				else if ( !exit_type )
				{
					//printf("abcd #182 v4 = %d, btb->Curr_Mode = %C, ScreenNum = %d\n",v4,btb->Curr_Mode,ScreenNum);
					if ( v4 == 128 )
						goto LABEL_2;	// continue?

					else if ( v4 == -22 )
					{
						a7 = 0;
						go_back = -22;
						no_fields = 0;
						v4 = ploop(btb, ScreenNum, PT_Start2, PT_End2, PT_Start, PT_End, a7);
						if ( exit_type || v4 && (ScreenNum || no_fields || isupper(*(char *)FLD_reply->FLDdata)) )
							goto LABEL_48;
						return 0;
					}
					else if ( v4 == -30 )
					{
						ScreenNum = ScreenNum - 1;
						ScreenNum = ScreenNum + 1;
						if ( ScreenNum < (btb->Scrs & 0xBFFF) )		// 1011 1111 1111 1111      0x4000 flag for what??
							goto LABEL_25;
						goto LABEL_167;
					}
					else if ( PTARR(getptabp(PT_Start2))->OpCode == 600 )	// format block_name
						v31 = 'y';

					else if ( v4 == -17 || v4 == -5 )
					{
						setcvar(v_LREPLY, "q");
						if ( btb->Curr_Mode == 'a' && (signed short)ScreenNum > 0 )
							delhier(TDno, 1);
						if ( btb->Curr_Mode != 'r' )
						{
							ulckrec(TDno);
							rfree(TDno, 0);
						}
						if ( !(btb->Scrs & 0x4000) )
							endtran();
						if ( !ScreenNum && !var_cr )
						{
							disprec(btb, TDno, ScreenNum, 0, 0);
							v42 = true;
						}
						goto LABEL_2;
					}
					else if ( btb->Curr_Mode == 'd' )
					{
						setcvar(v_REPLY, "");
						v31 = reply("\aare you sure <y,n>", "ny");	// embedded bell char
					}
					else if ( btb->Curr_Mode == 'r' )
					{
						if ( ScreenFields[ScreenNum] <= 1 )
							v31 = reply("<c>ontinue or <q>uit", "cq");
						else
							v31 = reply("<y>es <m>odify or <q>uit", "ymq");
					}
					else if ( btb->Curr_Mode != 'a' && !ScreenFields[ScreenNum] || btb->Curr_Mode == 'v' )
						v31 = reply("<y>es or <q>uit", "yq");

					else if ( ScreenNum && btb->Curr_Mode != 'c' )
						v31 = reply("<y>es <m>odify or <q>uit", "ymq");

					else if ( a7 && btb->Curr_Mode == 'c' )
						v31 = reply("<y>es <m>odify or <q>uit", "myq");

					else
						v31 = reply("<y>es <m>odify or <q>uit", "ymq");
				}
//================================================

				a7 = 0;

				if ( v31 == 'q' || v31 == 'n' )	// 'n' only valid in delete mode where user answers NO to "are you sure <y,n>?"
					goto LABEL_108;
				if ( v31 == 'm' )
				{
					// if v31 == 'm'
					if ( btb->Curr_Mode != 'r' )
						recalt = true;

					PT_Start = eafr(TDno, PT_Start2, PT_End2, &SrchChar);	// look for user to enter a field number to modify
					if ( PT_Start >= 0 )
					{
						if ( PT_Start > 0 )	// true if a field number or wildcard was entered
						{
							PT_End = ((unsigned char)(SrchChar - 42) > 1u ) ? PT_Start : PT_End2;	// '*' or '+'
						}
						else
						{
							PT_Start = PT_Start2;
							PT_End	 = PT_End2;
						}
						v4 = ploop(btb, ScreenNum, PT_Start2, PT_End2, PT_Start, PT_End, a7);
						if ( exit_type || v4 && (ScreenNum || no_fields || isupper(*(char *)FLD_reply->FLDdata)) )
							goto LABEL_48;

						return 0;		// exit_done. This will break the calling loop in pblock.c
					}
				}
				else if ( v31 == 'y' )
				{
					//printf("abcd: #278 v31 = %C, currmode = %C\n",v31,btb->Curr_Mode);
					if ( btb->Curr_Mode == 'd' )
					{
						if ( !btb->On_delete )
							goto LABEL_127;
						btb1 = &btab[btb->On_delete->BlockNo];
						v4 = ploop( btb1, 0, btb1->StartLine, btb1->EndLine, btb1->StartLine, btb1->EndLine, 0);
						if ( v4 != -5 || exit_type != 886 )
						{
							if ( *(char *)ttab->TTfields[5].FLDdata == 'y' )
							{
								setcvar(v_REPLY, "");	// "REPLY"
	LABEL_127:
								delhier(TDno, 1);
							}
							else
							{
								setcvar(v_REPLY, "");	// "REPLY"
								prerr("record NOT deleted, <CR> to continue ", 0, 1);
								while (linput(&v45, 0, 0))
									;
								prerr(0, 0, 0);	// clear message line
							}
							if ( !(btb->Scrs & 0x4000) )
								endtran();

							rfree(TDno, 0);
							if ( !ScreenNum )
							{
								if ( var_cr )
								{
									dscreen(btb, 0, 0);	// update global nflds var
									var_cr = false;
								}
								else
								{
									disprec(btb, TDno, ScreenNum, 0, 0);
									v42 = true;
								}
								PT_Start = PT_Start2;
								PT_End	 = PT_End2;
								goto LABEL_38;
							}
							goto LABEL_2;
						}
					}
	// end delete -------------------------------------------------------
					else
					{
						if ( (ptb1 = PTARR(getptabp(PT_Start2)))->OpCode == 600 )	// 600 == format block_name
						{
							//ptb1 = PTARR(getptabp(PT_Start2));
							btb1 = &btab[ptb1->Operand];

							if ( btb->Scrs & 0x4000 || btb->Curr_Mode == 'r' || ScreenNum != (btb->Scrs & 0xBFFF) - 1 )
								btb1->Scrs |= 0x4000;
							else
								btb1->Scrs &= 0xBFFF;

							v4 = pblock(btb1, ENARR(ptb1->TABno));	// call format block with MODE variable specifier
							if ( v4 != -5 || exit_type != 886 )
								goto LABEL_154;
						}
						else
						{
							if ( btb->Curr_Mode != 'r' && btb->Curr_Mode != 'v' )
							{
								v44 = ScreenNum == (btb->Scrs & 0xBFFF) - 1;
								if ( recalt || hasass(TDno) )// check for record change, or sub-keys
								{
									repkey(TDno, 0);
									reprec(TDno, v44);
									v40 = true;
								}
								if ( v44 )
								{
									ulckrec(TDno);
									if ( v40 )
									{
										if ( !(btb->Scrs & 0x4000) )
										{
											endtran();
											v41 = true;
										}
									}
								}
							}
	LABEL_154:
							if ( ScreenNum != (btb->Scrs & 0xBFFF) - 1 || btb->Curr_Mode == 'r' )
								v42 = false;
							else
							{
								ulckrec(TDno);
								if ( !v41 && !(btb->Scrs & 0x4000) )
									endtran();
								
								rfree(TDno, 0);
								if ( !ScreenNum )
								{
									if ( var_cr )
									{
										dscreen(btb, 0, 0);
										var_cr = false;
									}
									else
									{
										disprec(btb, TDno, ScreenNum, 0, 0);
										v42 = true;
									}
									PT_Start = PT_Start2;
									PT_End	 = PT_End2;
									goto LABEL_38;
								}
							}

							ScreenNum++;	// looks like 'default' behaviour again?
							if ( ScreenNum < (btb->Scrs & 0xBFFF) )
								goto LABEL_25;
							goto LABEL_167;
						}
					}
				}
				else	// looks like 'default' behaviour?   Maybe 'C' continue?
				{
					ScreenNum++;
					if ( ScreenNum < (btb->Scrs & 0xBFFF) )
						goto LABEL_25;
					goto LABEL_167;
				}
            } // end loop - while(1)

LABEL_108:
			// v31 == 'q' or 'n' land here
//printf("abcd: LABEL_108 v31 = %C, currmode = %C\n",v31,btb->Curr_Mode);

            if ( btb->Curr_Mode != 'r' )
            {
                if ( btb->Curr_Mode != 'a' || (signed short)ScreenNum <= 0 )
                    ulckrec(TDno);
                else
                    delhier(TDno, 1);
                rfree(TDno, 0);
                if ( !ScreenNum )
                {
                    if ( var_cr )
                    {
                        dscreen(btb, 0, 0);
                        var_cr = false;
                    }
                    else
                    {
                        disprec(btb, TDno, ScreenNum, 0, 0);
                        v42 = true;
                    }
                    PT_Start = PT_Start2;
                    PT_End	 = PT_End2;
                    goto LABEL_38;
                }
            }
        } // end second loop - while (1)

		// 157 if ( PTARR(getptabp(PT_Start2))->OpCode != 600 ) // 600 == format block_name lands here

        if ( v42 || v4 == 128 )
            v42 = false;
        else
            ScreenFields[ScreenNum] = dscreen(btb, ScreenNum, 0);
        
		if ( v4 == -5 )
		{
			btb->BTFlags = false;
            return 0;
		}
        if ( fld_nbrs == 1 && btb->Curr_Mode != 'r' )
            prmsg("%s %s", mode_txt, ttab[TDno].TableName);	// Show current mode and tablename

LABEL_38:
        v29 = *(char *)FLD_mode->FLDdata;
    } // end outer do loop
    while (isupper(v29) && btb->Curr_Mode != v29 && !ScreenNum );

    v31 = 0;
    a7 = 1;
    recalt = btb->Curr_Mode == 'a';
    no_fields = 0;
    v4 = ploop(btb, ScreenNum, PT_Start2, PT_End2, PT_Start, PT_End, a7);
    if ( exit_type || v4 && (ScreenNum || no_fields || isupper(*(char *)FLD_reply->FLDdata)) )
        goto LABEL_48;

    return 0;
}


#endif
