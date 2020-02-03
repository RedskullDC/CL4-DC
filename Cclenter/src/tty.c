#ifndef TELETYPE_C
#define TELETYPE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <termios.h>
//#include <curses.h>		// term stuff
//#include <term.h>		// ditto
#include "DBdefs.h"
#include "cl4.h"


void Tty_Construct()
{
    if ( !term_saved )	// only possible when Key debugging "clenter -k"
    {
        ioctl(fileno(stdin), 0x5405u, &save_term);         // TCGETA	get current system attributes for stdin
        term_saved = true;

		memcpy(&new_term,&save_term,sizeof(struct termio));		// copy to our new structure, and make few changes

		// with optimisation active, creates the following code:
        //*(int *)&new_term.c_iflag = *(int *)&save_term;// copies both c_iflag and c_oflag variables
        //*(int *)&new_term.c_line = *(int *)&save_term.c_line;// copies c_line and c_cc[0-2]
        //*(int *)&new_term.c_cc[3] = *(int *)&save_term.c_cc[3];// copies c_cc[3-6]
        //*(short *)&new_term.c_cc[7] = *(short *)&save_term.c_cc[7];// copies c_cc[7-8]

        new_term.c_lflag = save_term.c_lflag & 0xFFF5;				// controls various terminal functions
        new_term.c_cflag = (save_term.c_cflag | 0x30) & 0xFEFF;		// hardware control flag
        new_term.c_cc[6] = 1;
        new_term.c_cc[5] = 0;
        new_term.c_iflag = save_term.c_iflag & 0xFECF;				// terminal input control
    }
    ioctl(fileno(stdin), 0x5407u, &new_term);				// TCSETAW	update stdin with our new settings
}                                     

// #define NCC 8 ??
// struct termio {
//         unsigned short c_iflag;         /* input mode flags */
//         unsigned short c_oflag;         /* output mode flags */
//         unsigned short c_cflag;         /* control mode flags */
//         unsigned short c_lflag;         /* local mode flags */
//         unsigned char c_line;           /* line discipline */
//         unsigned char c_cc[NCC];        /* control characters */
// };

void setblock(int fd, int a2)
{
    int v2; // ecx@2
    int v4; // eax@5

    if ( first_2 )
    {
        first_2 = 1;					// *** check this **** ??
        v2 = fcntl(fd, 3, 0);
        if ( v2 == -1 )
        {
            perror("setblock(.. fcntl");
            return;
        }
        blockf_0	= v2 & ~ndelay_3;
        nonblockf_1 = v2 | ndelay_3;
    }
    
	v4 = blockf_0;
    if ( !a2 )
        v4 = nonblockf_1;
    
	if ( fcntl(fd, 4, v4) == -1 )
    {
        perror("setblock fcntl2");
        return;
    }
}

void Tty_Destruct(void)
{

    ioctl(fileno(stdin), 0x5407u, &save_term);             // TCSETAW 
    setblock(fileno(stdin), 1);
}

// readcond()
// Tty_CharReady()
// Tty_CharGet() 

bool Tty_KeyIsReady(void)
{
    int nextc;

    setblock(fileno(stdin), false);
    
	nextc = getc(stdin);
    if ( nextc != -1 )
        ungetc(nextc, stdin);
    
	setblock(fileno(stdin), true);
    
	return nextc != -1;
}

#endif
