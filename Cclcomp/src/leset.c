#ifndef LESET_C
#define LESET_C

#include <stdlib.h>		// for strtol()

int leset(int *a3)
{
    PTAB *pt;
	char arg[128];

	symbol = getsym();
	while ( symbol != 930 )	// multiple options can be included in one set command
    {
		cdbcpystr(arg, sym, 0);
        if ( !compare(arg, "error") )
			symbol = 410;                   // why is ERROR not in the symbol table????
											// so as not to mask out the ERROR variable!!
		pt = PTARR(getptabp((*a3)++));
        pt->SrcLineNo = lla;
        
		switch (symbol)
		{
		case 1450:							// skip
			pt->OpCode	= 669;
			pt->Operand = 0;				// skip is normally set to 'default' behaviour 
			symbol = getsym();
			if ( symbol == 180 )			// "," comma
			{
				symbol = getsym();
				if ( symbol == 960 )		// off
					pt->Operand = 2;
				else if ( symbol == 1030 )	// on
					pt->Operand = 1;
				symbol = getsym();
			}
			break;

		case 1050:							// output
			pt->OpCode	= 667;
	        pt->Operand = 2;				// default == stdout
	        symbol = getsym();
	        if ( symbol == 180 )            // ","  comma
	        {
				symbol = getsym();
	            if ( symbol == 200 )        // console
					pt->Operand = 1;
				symbol = getsym();
			}
			break;
		
		case 450:							// TAB and FILL
		case 1600:
			if ( symbol == 1600 )
				pt->OpCode = 652;			// tab
			else
				pt->OpCode = 654;			// fill
			pt->Operand = 32;				// default == <SPC> ' '
            symbol = getsym();
            if ( symbol == 180 )
			{
				symbol = getsym();
				if ( symbol != 930 && syml == 1 )	// should check for string literal type?
				{
					pt->Operand = sym[0];	// new TAB/FILL character
					symbol = getsym();
				}
			}
			else
				pt->Operand = 0;			// default = \0 <null>
			break;
		
		case 60:							// align
            pt->OpCode	= 684;				// 60 "align" handler
            pt->Operand = 0;				// default left align
            symbol = getsym();
            if ( symbol == 180 )            // "," comma
            {
				symbol = getsym();
                if ( symbol == 2200 )       // numeric literal
				{
					pt->Operand = (short)strtol(sym, 0, 10);
					symbol = getsym();
				}
				else
				{
					loaderr(33, sym);		// "unexpected symbol"
					return 0;
				}
			}
			break;

		case 305:							// dateform
			pt->OpCode = 673;
			pt->Operand = 0;				// default = 0  'dd-mm-yy'
			symbol = getsym();
			if ( symbol == 180 )            // ","  comma
			{
				symbol = getsym();
			    if ( symbol == 2200 )       // numeric literal
			    {
					pt->Operand = (short)strtol(sym, 0, 10);		// *** no sanity check on numeric value **
					symbol = getsym();		// should be 1 or 2
				}
				else
				{
					loaderr(33, sym);		// "unexpected symbol"
					return 0;
				}
			}
			break;

		case 10:
		case 20:
		case 100:
		case 220:
		case 255:
		case 280:
		case 355:
		case 470:
		case 610:
		case 640:
		case 700:
		case 920:
		case 1270:
		case 1280:
		case 1290:
		case 1330:
		case 1625:
		case 1630:
		case 1760:
            switch ( symbol )
            {
            case 10:                    // aauto
				pt->OpCode = 676;
                break;
			case 20:                    // abort
				pt->OpCode = 670;
                break;
			case 100:                   // "E"
				pt->OpCode = 665;
                break;
			case 220:                   // convert
				pt->OpCode = 686;
                break;
			case 255:                   // csv
				pt->OpCode = 630;
                break;
			case 280:                   // debug
				pt->OpCode = 672;
                break;
			case 355:                   // dos
				pt->OpCode = 640;
                break;
			case 470:                   // fnum
				pt->OpCode = 674;
                break;
			case 610:                   // goback
				pt->OpCode = 682;
                break;
			case 640:                   // hangup
				pt->OpCode = 681;
                break;
			case 700:                   // justify
				pt->OpCode = 666;
                break;
			case 920:                   // null_exit
				pt->OpCode = 659;
                break;
			case 1270:                  // rauto
				pt->OpCode = 678;
                break;
			case 1280:                  // rawdisplay
				pt->OpCode = 687;
                break;
			case 1290:                  // rawprint
				pt->OpCode = 688;
                break;
			case 1330:                  // repeat
				pt->OpCode = 679;
                break;
			case 1625:                  // timing
				pt->OpCode = 690;
                break;
			case 1630:                  // trim
				pt->OpCode = 680;
                break;
			case 1760:                  // xml
				pt->OpCode = 651;
                break;
			}
            pt->Operand = 1;			// default == on
            symbol = getsym();
            if ( symbol == 180 )		// "," comma
			{
				symbol = getsym();
				if ( symbol == 960 )             // off
				{
					pt->Operand = 2;
					symbol = getsym();
				}
				else if ( symbol == 1030 )       // on (default set above)
					symbol = getsym();
			}
			break;

		case 30:
		case 410:
		case 630:
		case 870:
		case 1190:
		case 1200:
		case 1610:
			switch ( symbol )		// following screen oriented statements can have attributes attached:
            {
			case 30:               // accept
				pt->OpCode = 656;
				break;
			case 410:              // error
				pt->OpCode = 660;
                break;
			case 630:              // head
				pt->OpCode = 662;
                break;
			case 870:              // message
				pt->OpCode = 650;
                break;
			case 1190:             // print
				pt->OpCode = 668;
                break;
			case 1200:             // prompt
				pt->OpCode = 658;
                break;
			case 1610:             // text
				pt->OpCode = 664;
                break;
			}
            pt->Operand = 0x003F;				// default to normal intensity
            
			symbol = getsym();
            if ( symbol != 180 || (symbol = getsym(), symbol == 930) || syml != 1 )
				break;
				
			switch ( sym[0] )
            {
			case 'f':
				pt->Operand = 0x0001u;			// feint or half-intensity
				break;
			case 's':
				pt->Operand = 0x0002u;			// standout
				break;
			case 'u':
				pt->Operand = 0x0004u;			// underscore
                break;
			case 'r':
                pt->Operand = 0x0020u;			// ??
                break;
            case 'B':
				pt->Operand = 0x0010u;			// ??
                break;
			case 'b':
				pt->Operand = 0x0008u;			// bold
                break;
			case 'x':
			case 'n':
				pt->Operand = 0x003Fu;			// normal
                break;
			default:
				loaderr(29, arg);				// "invalid set statement"
				return 0;
			}
		    symbol = getsym();
			break;

		default:
			pt->OpCode = 400;				// 400 = nop
			loaderr(29, arg);				// "invalid set statement"
			return 0;
			break;
		}
	} // end while ( symbol != 930 )
	return 1;	// exit_success
}

#endif
