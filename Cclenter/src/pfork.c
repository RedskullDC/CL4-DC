#ifndef PFORK_C
#define PFORK_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void pfork(PTAB *ptab)
{
    char **v8; // esi@26
    __pid_t v9; // eax@32
    __pid_t child_PID; // eax@5
    signed int v6; // esi@22
    int v10;
	int v11;
	short prout_save;
	char tab_save;
    
	__sighandler_t hnd15_sve;
	__sighandler_t hnd3_sve;
	__sighandler_t hnd2_sve;
	int stat_loc;
	char a1[8192];
	
    fflush(stdout);
    fflush(stderr);
    fflush(op);
    
	if ( ptab->OpCode )
    {
        if ( !ptab->TABno )
        {
            if ( !have_forked_0 )
            {
                if ( cllock(getpid()))		// cllock(self)
                {
                    child_PID = fork();
                    if ( child_PID )		// parent process
                    {
                        if ( child_PID != -1 )
                        {
                            flagcdb(0, child_PID);		// mark all DB's as belonging to the child PID
                            longjmp(sjbuf, 1);			// jump back up to main() and exit
                        }
                    }
                    else					// this is the child process
                    {
                        cllock(0);			// stop the parent trying to fork() twice?
                        clunlock();
                        flagcdb(1, 0);		// clear the Pagelist1 table on all our DB's
                        
						signal(SIGINT, SIG_IGN);	// default sig handlers to ignore
                        signal(SIGQUIT, SIG_IGN);
                        signal(SIGHUP, SIG_IGN);
                        have_forked_0 = true;
                        
						if ( isatty(2) )
                            freopen64("/dev/null", "w", stderr);
                        if ( isatty(0) )
                            freopen64("/dev/null", "r", stdin);
                        if ( isatty(1) )
                            freopen64("/dev/null", "w", stdout);
                    }
                }
            }
            return;	// exit_success
        }
        goto LABEL_17;
    }
    
	if ( ptab->TABno )	// fork 'do this command' or fork variable_name
    {
LABEL_17:
        exptobuf(a1, ENARR(ptab->TABno), 0);	// get the fork expression
    }
	else
		cdbcpystr(a1, "sh", 0);

	prout_save	= pr_out;			// save values in case child modifies, restore below
    tab_save	= TAB;

    child_PID = fork();
    if ( !child_PID )				// 0 == this is the child process handler.
    {
        closescr();
        v6 = 3;						// not sure what these FD's are for....
        do
            close(v6++);
        while ( v6 <= 19 );
        
		chklt(a1);
        if ( isalist(a1) )
        {
            execlp("/bin/sh", "sh", "-c", a1, NULL);	// *** does not return on success ***
			syserror("pfork:execlp[%s]", a1);
        }
        else
        {
            v8 = camake(a1, 32);	// split on space chars
			execvp(v8[0], v8);		// *** does not return on success ***
			syserror("pfork:execvp[%s]", *v8);
        }
		_exit(127);
    }
	else if ( child_PID == -1 )	// error! We couldn't fork() for some reason
    {
        stat_loc = *__errno_location() << 8;
    }
    else					// parent process handler
    {
        hnd2_sve = signal(SIGINT, SIG_IGN);		// replace sig handlers with ignore
        hnd3_sve = signal(SIGQUIT, SIG_IGN);
        hnd15_sve = signal(SIGTERM, SIG_IGN);
        
		do
        {
            v9 = wait(&stat_loc);				// wait for the child started above to complete
        }
        while ( v9 != child_PID && v9 != -1 );	// v9 == 1 means parent received a signal

		//printf("pfork() v9 = %d, stat_loc = %d\n",v9,stat_loc);
		signal(SIGINT, hnd2_sve);					// restore sig handlers and tab/screen state
        signal(SIGQUIT, hnd3_sve);
        signal(SIGTERM, hnd15_sve);
        openscr();
        pr_out	= prout_save;
        TAB		= tab_save;
    }
    
	//printf("pfork - stat_loc = x%08X\n",stat_loc);
	v10 = (unsigned char)stat_loc;
    v11 = 0;

    if ( !v10 )
    {
        v11 = (unsigned char)((stat_loc >> 8) & 0xFF);
        if ( v11 & 0x80 )
        {
            v10 = (unsigned char)(~v11 + 1);	// *** not sure this is correct ***
            v11 = 0;
        }
    }
    
	setvar(v_ERROR, v10);	// "ERROR"
    setvar(v_EXIT, v11);	// "EXIT"
}

#endif

