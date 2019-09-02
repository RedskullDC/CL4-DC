#ifndef PRPIPE_C
#define PRPIPE_C

#include <ctype.h>	// for isctrl() etc.
#include "lvarnames.h"		// for flag bits

void prpipe(SFTAB *sftab)
{
	const char *v3;
	short cmd;
	
    if ( sftab )
    {
		cmd = sftab->cmd_flags;
        if ( cmd & 0x0010 )
		{
			if ( cmd & 0x0040 )				// 0x40 == Pipe flag
			{
				switch (cmd & 0xFFAF)		// 1111 1111 1010 1111 
				{
				case 1:
					v3 = "popenin ";
					break;
				case 2:
					v3 = "popenout ";
					break;
				case 3:
					v3 = "popenio ";
					break;
				default:
					v3 = "popen?? ";
					break;				
				}
			}
			else
			{
				switch (cmd & 0xFFEF)		// 1111 1111 1110 1111
				{
				case 1:
					v3 = "openin ";
					break;
				case 2:
					v3 = "openout ";
					break;
				case 4:
					v3 = "openapp ";
					break;
				default:
					v3 = "open?? ";
					break;
				}
			}
		}
		else
        {
			if ( cmd & 0x0008 )
            {
				v3 = "close ";
			}
            else
            {
				switch (cmd & 0xFFBF)		// 1111 1111 1011 1111
				{
				case 1:
					v3 = "read ";
					break;
				case 2:
					v3 = "write ";
					break;
				default:
					v3 = "r/w????? ";
					break;
				}
			}
		}

		printf(v3);
        prtfid(sftab->AliasEnt);
        if ( sftab->FileNameEnt )			// takes precedence over field 2
			prtfid(sftab->FileNameEnt);
        else if ( sftab->PRTno )
			prpf(sftab->PRTno);
	}
}


#endif
