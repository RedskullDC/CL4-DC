#ifndef KEY_DEBUG_C
#define KEY_DEBUG_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

void cleanup(int val)
{
    Tty_Destruct();
    mexit(val);
}

void Key_Debugem()
{
    int ReadChar;
	int NumTabs;
	
    Tty_Construct();				// sets stdio to return after each char
    Key_Construct();
    signal(2, cleanup);				// set <CTRL>-C handler (signal 2) to cleanup()
    while ( 1 )
    {
        print("actual..keydef..macro\n");
        ReadChar = Key_GetChar();	// without Tty_Construct(), will block till <RTN>

        for ( NumTabs = 2; ; NumTabs = 0 )
        {
            Key_Print(ReadChar, NumTabs);
            if ( !Key_Ready() )
                break;
            ReadChar = Key_GetChar();
            _IO_putc(' ', stdout);
        }
        _IO_putc('\n', stdout);
        if ( ReadChar == 'q' )
            cleanup(0);                         // time to get out of here......
    }
}

#endif
