#ifndef FILE_OPS_C
#define FILE_OPS_C

#include <stdio.h>
#include <assert.h>		// for assert() naturlich!
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <sys/wait.h>	// for pclose() wait calls
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int _write(FCB *fcb, PRTAB *prtb)
{
    FILE	*op_save;
    short	pr_save;
    short	rtncode;

    if ( fcb->WR_FILE )
    {
        op_save	= op;				// save values
        pr_save	= pr_out;

		op		= fcb->WR_FILE;		// temp direct output to FILE*
        pr_out	= 0;
        
		rtncode	= prfield(prtb);
		if ( bp_flag )				// indicates that a broken pipe interrupt has occured
        {
            bp_flag = false;		// reset interrupt flag
            rtncode	= 28;
        }

		op		= op_save;			// restore old vals
        pr_out	= pr_save;
        if ( rtncode == 0x2000 )	// not an error, _prfield() indicates to close file
        {
            fclose(fcb->WR_FILE);
            fcb->WR_FILE	= 0;
            fcb->field_C	= 0;
            fcb->cmd_flags	&= 0xFFFDu;		// 1111-1111-1111-1101
            rtncode			= 0;
        }
        return rtncode;				// exit_success
    }
    else
        return 23;					// error_exit - no Write FILE*
}

int _read(FCB *fcb, PRTAB *prtb)
{
    FLDdesc *fld;
	PRTAB	*v10;
	char	*a1;
	
	short	FLDno;
	short	FLDlen;
	int		ErrVal;
	FLDdesc fld2;
    char	Buffer[16384];

    Buffer[0] = 0;
    if ( fcb->RD_FILE )			// FILE* for reading
		ErrVal = (fgets(Buffer, 16384, fcb->RD_FILE) == 0) ? 26 : 0 ;	// 0 means read OK, 26 == read error
    else
        ErrVal = 22;			// no read FILE*

    a1 = Buffer;
    while ( prtb )
    {
        fld = getftf(ENARR(prtb->VAR_ENTno), 0, &fld2);
        if ( fld->FLDstat & fld_ARRAY)			// Read *all* subscripts of an entire Array	0x0002			
		{
			if ( fld->FLDtype == 'C' )
				FLDlen = fld->FLDlen + 1;		// allow for \0 string terminator
			else
				FLDlen = sizeof(double);
            
			fld->FLDdata = fld->FLDname;		// Array variables hang of FLDname.
			FLDno = 0;
			while ( fld->FLDelemID > FLDno )	// FLDelemID is size of array subscript.
			{
				a1 = buftott(a1, fld);
				FLDno++;
				fld->FLDdata = (char *)fld->FLDdata + FLDlen;
			}
        }
        else	// read a single variable
		{
			a1 = buftott(a1, fld);				// returns pointer past buffer for easy chaining!
        }
        v10 = PRARR(prtb->NextPR);
		prtb = v10;
	}
    return ErrVal;
}

int _pclose(FCB *fcb)							// Pipe close
{
	int		stat_loc;
    short	v1;
	__sighandler_t hnd2_sve;
	__sighandler_t hnd3_sve;
	
    stat_loc		= 0;
    if ( fcb->RD_FILE )
        fclose(fcb->RD_FILE);
    fcb->RD_FILE	= 0;
    fcb->field_8	= 0;
    
	if ( fcb->WR_FILE )
        fclose(fcb->WR_FILE);
    fcb->WR_FILE	= 0;
    fcb->field_C	= 0;
    fcb->cmd_flags	= 0;
    mfree_0(fcb->ALIAS);
    fcb->ALIAS		= 0;

    hnd2_sve = signal(SIGINT, SIG_IGN);		// save sig handlers, replace with ignore defaults
    hnd3_sve = signal(SIGQUIT, SIG_IGN);
    do
    {
        v1 = wait(&stat_loc);						// wait for the child fcb->child_PID to complete
    }
    while ( v1 != fcb->child_PID && v1 != -1 );	// v1 == 1 means parent received a signal
    
	signal(SIGINT, hnd2_sve);							// restore saved handlers
    signal(SIGQUIT, hnd3_sve);

    //setvar(6, BYTE1(stat_loc));	// __WEXITSTATUS(status)   (((status) & 0xff00) >> 8)
	setvar(v_EXIT,__WEXITSTATUS(stat_loc));		// "EXIT"

    return (unsigned char)stat_loc;	// Not quite sure which flags we are looking at here ..... ??
}
//
// If WIFEXITED(STATUS), the low-order 8 bits of the status.  */
// #define __WEXITSTATUS(status)   (((status) & 0xff00) >> 8)

// * If WIFSIGNALED(STATUS), the terminating signal.  */
// #define __WTERMSIG(status)      ((status) & 0x7f)

int _popen(SFTAB *sftab)	// Pipe open
{
    FCB		*fcb;
	char	**v13;
	
    int		v11;
	int		MaxLen;
	int		fd2[2];
	int		fd[2];
	
    short	pno;
    short	cmd_bits;
	
    char	Aliasname[128];
	char	a1[8192];
	char	FileName[8192];
	
	exptobuf(FileName, ENARR(sftab->FileNameEnt), 0);	// Expression number describing Filename

    if ( !FileName[0] )
        return 21;
    cdbcpystr(a1, FileName, 0);		// need a second copy in allocated mem space as passed externally!
    
    exptobuf(Aliasname, ENARR(sftab->AliasEnt), 0);	// Expression number describing Aliasname

	MaxLen = (signed short)(lenstr(Aliasname) + 1);

    fcb = _files;
    pno = 0;
    if ( no_files <= 0 || !_files->cmd_flags )
    {
LABEL_13:
        if ( no_files == pno )
        {
            newfile();
			assert(pno < no_files);			// couldn't allocate mem for a new FCB
            fcb = &_files[pno];
        }
        
		cmd_bits = sftab->cmd_flags;		// command flag bits
        if ( cmd_bits & 0x01 && ( pipe(fd) < 0) || cmd_bits & 0x02 && ( pipe(fd2) < 0) )
            return 21;	// error_exit
        
		fflush(stdout);
        fflush(stderr);
        
        fcb->child_PID = fork();
        if ( !fcb->child_PID )				//  == 0 is the child process, parent process continues below
        {
            if ( cmd_bits & 0x01 )
            {
                close(fd[0]);
                close(1);
                dup(fd[1]);
                close(fd[1]);
            }
            if ( cmd_bits & 0x02 )
            {
                close(fd2[1]);
                close(0);
                dup(fd2[0]);
                close(fd2[0]);
            }
            signal(SIGINT, SIG_DFL);		// why set to default prior to ignore???
            signal(SIGINT, SIG_IGN);
            signal(SIGQUIT, SIG_IGN);
            
			v11 = 3;						// not sure why it closes these file descriptors....?
            do
                close(v11++);
            while ( v11 <= 19 );

            if ( isalist(a1) )
            {
                execlp("/bin/sh", "sh", "-c", a1, NULL);	// *** does not return on success ***
				syserror("pfile:execlp[%s]", a1);
            }
            else
            {
                v13 = camake(a1, 32);		// split on space chars
				execvp(*v13, v13);			// *** does not return on success ***
	            syserror("%s", *v13);
            }
            _exit(127);
        }
		else if ( fcb->child_PID == -1 )	// -1 == error in fork() above
		{
			return 21;						// error, we couldn't fork() for some reason!
		}
		else								// This is the parent process continuing if all was ok.
        {
            if ( cmd_bits & 0x01 )		
            {
                close(fd[1]);
                fcb->field_8	= fd[0];
                fcb->RD_FILE	= fdopen(fd[0], "r");
            }
            if ( cmd_bits & 0x02 )
            {
                close(fd2[0]);
                fcb->field_C	= fd2[1];
                fcb->WR_FILE	= fdopen(fd2[1], "w");
            }
            fcb->cmd_flags		= cmd_bits;
            fcb->ALIAS			= mstrcpy(Aliasname, 0);
            return 0;						// exit_success!
        }
    }
    while ( 1 )
    {
        if ( cmpbuf(fcb->ALIAS, Aliasname, MaxLen))
            return 27;	// file already open error - does nothing? (in real clenter too)
        pno++;
        fcb++;
        if ( no_files <= pno || !fcb->cmd_flags )
            goto LABEL_13;
    }
}


int _fclose(FCB *fcb)
{
    int ErrVal;
	FILE *ReadFile;
	FILE *WriteFile;
	
    ErrVal = 0;
    ReadFile = fcb->RD_FILE;                         // read FILE*
    if ( ReadFile != stdin )
    {
        WriteFile = fcb->WR_FILE;                     // write FILE*
        if ( WriteFile != stdout && WriteFile != stderr )
        {
			if ( ReadFile && fclose(ReadFile) < 0)		// Read takes precedence over WriteFile?
				ErrVal = 24;
			else if ( WriteFile && fclose(WriteFile) < 0)
				ErrVal = 24;
			else if (!WriteFile && !ReadFile)	// error, couldn't close one of the files properly!
				ErrVal = 25;
        }
    }
    fcb->RD_FILE	= 0;
    fcb->WR_FILE	= 0;
    fcb->cmd_flags	= 0;
    mfree_0(fcb->ALIAS);	// free alias name memory
    fcb->ALIAS		= 0;
    return ErrVal;
}

int _fopen(SFTAB *sftb)
{
    FCB		*fcb;
	FILE	*v11;
	short	fno;
	short	cmd_bits;
	short	v9;
	short	MaxLen;
	char	Alias[128];
	char	FileName[1024];
	
    exptobuf(FileName, ENARR(sftb->FileNameEnt), 0);	// Expression number describing filename
    exptobuf(Alias, ENARR(sftb->AliasEnt), 0);			// Expression number describing Aliasname


	MaxLen	= lenstr(Alias) + 1;
    fcb		= _files;
    fno		= 0;
    if ( no_files <= 0 || !fcb->cmd_flags )
    {
LABEL_12:
        if ( no_files == fno )
        {
            newfile();
			assert(fno < no_files);	// couldn't allocate mem for a new FCB
            fcb = &_files[fno];
        }

		cmd_bits = sftb->cmd_flags;		// command flag bits
        if ( cmd_bits & 1 )	// read mode
        {
            if ( (unsigned short)getresf(FileName) )
            {
                v11 = fopen64(FileName, "r");
                if ( !v11 )
                    return 21;		// error, couldn't open file
            }
            else
                v11 = stdin;
			fcb->RD_FILE = v11;
        }
        else if ( cmd_bits & 6 )	// 0x04 write/ 0x02 append mode
		{
			v9 = getresf(FileName);
			if ( v9 == 1 )
			    v11 = stdout;
			else if ( v9 == 2 )
			    v11 = stderr;
			else if ( !(cmd_bits & 4) || (v11 = fopen64(FileName, "a")) == 0 )	// Append trumps write?
			{
				if ( !(cmd_bits & 2) || (v11 = fopen64(FileName, "w")) == 0 )
					return 21;
			}
			fcb->WR_FILE = v11;
		}
		else
			return 21;					// error_exit
		
		fcb->cmd_flags = cmd_bits;				// copy sftab flag bits to the fcb structure
		fcb->ALIAS = mstrcpy(Alias, 0);
		return 0;						// exit_success
    }
    while ( 1 )
    {
        if ( cmpbuf(Alias, fcb->ALIAS, MaxLen) )
			return 27;			// file already open error - does nothing? (in real clenter too)
        fno++;
        fcb++;
        if ( no_files <= fno || !fcb->cmd_flags )
            goto LABEL_12;
    }
}

int getresf(char *FileName)
{
	if (cmpbuf(FileName, "stdin", 6))
		return 0;
	else if (cmpbuf(FileName, "stdout", 7))
		return 1;
	else if (cmpbuf(FileName, "stderr", 7))
		return 2;
	else
		return -1;	// not a reserved name
}

void newfile(void)
{
    int v0;

    v0 = no_files++;
    _files = (FCB *)mrealloc(_files, sizeof(FCB) * v0, sizeof(FCB) * no_files);	// sizeof(FCB) == 28
}

FCB *getfp(short AliasEntNo)
{
    FCB		*fcb;
	short	fno;
	short	MaxLen;
	char	AliasStr[256];
	
    exptobuf(AliasStr, ENARR(AliasEntNo), 0);

    MaxLen	= lenstr(AliasStr) + 1;
    fcb		= _files;
    fno		= 0;
	while ( fno < no_files )		// ** global no_files **
    {
		if ( fcb->cmd_flags && cmpbuf(AliasStr, fcb->ALIAS, MaxLen) )
			break;
		fno++;
        fcb++;
    }

    if ( no_files == fno ) // reached end of list with no match
        return 0;		// not found
    else
        return fcb;		// exit_success
}

void _closefp(short cmd_mask)
{
    FCB *fcb; // esi@1
    short fileno; // di@1

    fcb = _files;
    fileno = 0;

    while ( no_files > fileno )
	{
		if ( fcb->ALIAS && (cmd_mask == fcb->cmd_flags || !cmd_mask) )
		{
			if ( cmd_mask & 0x40 )	// pipe
				_pclose(fcb);
			else
				_fclose(fcb);
		}
		fileno++;
		fcb++;
	}
}

void closefp(void)		// called by main() on exit
{
    _closefp(0x41u);	// close all read-only pipes
    _closefp(0x42u);	// close all write-only pipes
    _closefp(0x43u);	// close all read/write pipes
    _closefp(0);		// close all other open files
}


void pfile(SFTAB *sftab)
{
    FCB *fcb;
	short cmd_bits;
	int ErrVal;
	
    if ( no_files < 0 )			// initialised to -1
    {
        _files = (FCB *)mmalloc(0);
        no_files = 0;
    }

    cmd_bits = sftab->cmd_flags;	// command flag bits

    if ( cmd_bits & 0x10 )			// open command
    {
        if ( cmd_bits & 0x40 )		// true if a pipe
            ErrVal = _popen(sftab);
        else
			ErrVal = _fopen(sftab);
    }
	else if ( cmd_bits & 0x20 )		// close 'tables' command
        ErrVal = closedb(0);		// *** closes databases, not FCB's ***
	else
	{
		fcb = getfp(sftab->AliasEnt);
		ErrVal = 25;
		if ( fcb )
		{
			if ( cmd_bits & 8 )		// 8 = close
			{
			    if ( fcb->cmd_flags & 0x40 )		// true if a pipe
			        ErrVal = _pclose(fcb);
			    else
					ErrVal = _fclose(fcb);
			}
			else if ( cmd_bits & 1 )	// 1 = read
			    ErrVal = _read(fcb, PRARR(sftab->PRTno));		// PRTAB no. describing variable to be read/written

			else if ( cmd_bits & 6 )	// 2 = write, 4 = append
				ErrVal = _write(fcb, PRARR(sftab->PRTno));		// PRTAB no. describing variable to be read/written

			else
				ErrVal = 29;	// invalid command bits?
		}
	}
    setvar(v_ERROR, ErrVal);                          // "ERROR" 1
}

#endif
