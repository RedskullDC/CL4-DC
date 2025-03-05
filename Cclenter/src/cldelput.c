#ifndef CLDELPUT_C
#define CLDELPUT_C

#include <string.h>
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"

void wrlog(TDesc *TTptr, char *WorkArea, int LockLog);
void locklog(int a1);
void startran(void);

void setNoLogging()
{
    logging = false;
}

int cldelr(int TTno, char *WorkArea1, POS* Pos, TDesc *TTptr)
{
	tl.TransType = 8;
	return clcdb(delr, TTno, WorkArea1, Pos, TTptr, 0);
}

int clputr(int TTno, char *WorkArea, POS *Pos, TDesc *TTptr, char *a6)
{
	tl.TransType = 16;
	return clcdb(putr, TTno, WorkArea, Pos, TTptr, a6);
}
/*  Not used 
int clclrtbl(int TTno)
{
	tl.TransType = 32;
	return clcdb(clrtable, TTno, 0, 0, 0, 0);
}*/

int noLogging(int TDno)
{
    TDesc *i; // [sp+10h] [bp-28h]@6
    char *v1; // eax@2
    char *v3; // ST28_4@10
    char *v4; // esi@10
    char *v5; // esi@13
    char *j; // esi@17
    char *ptr; // [sp+20h] [bp-18h]@13
    const char *s2; // [sp+24h] [bp-14h]@10

    signed int v2; // eax@3
    size_t n; // [sp+1Ch] [bp-1Ch]@13
    bool v9; // [sp+18h] [bp-20h]@9

    if ( !nlDbases_1 )
    {
        v1 = getevar("CLTNOLOG");
        nlDbases_1 = strdup(v1);
        if ( !tdArr_0[0] )
        {
            v2 = 0;
            do
                tdArr_0[v2++] = -1;
            while ( v2 <= 3003 );
        }
    }
    if ( tdArr_0[TDno] < 0 )
    {
        for ( i = ttab; i != (TDesc *)-24; ++i )	// ??????
        {
            if ( i->TTrtd == TDno )
                break;
        }
        v9 = 0;
        if ( i->TTrtd == TDno )
        {
            v3 = dtab[i->DBnumber].FullDBname;
            v4 = strrchr(v3, '/');
            s2 = v4 ? (v4 + 1) : v3;

			n = strlen(s2);
            ptr = strdup(nlDbases_1);
            v5 = strtok(ptr, ":");
            if ( v5 )
            {
                while ( strncmp(v5, s2, n) )
                {
                    v5 = strtok(0, ":");
                    if ( !v5 )
                        goto LABEL_23;
                }
                if ( v5[n] )
                {
                    if ( v5[n] == ',' )
                    {
                        for ( j = strtok(v5, ":,"); j && strcmp(j, i->TableName); j = strtok(0, ":,") )
                            ;
                        v9 = j != 0;
                    }
                }
                else
                {
                    v9 = 1;
                }
            }
LABEL_23:
            free(ptr);
        }
        tdArr_0[TDno] = v9;
    }
    return tdArr_0[TDno];
}

void startplog(int a1)
{
	short buf[16]; // enough space to read 32 byte header!!

    if ( lfd >= 0 )
    {
        if ( lseek64(lfd, 0LL, 0) >= 0 && read(lfd, buf, 32u) == 32 && (buf[1] & 0x1000))
        {
            buf[1] &= 0xEFFFu;
            locklog(1);
            if ( lseek64(lfd, 0LL, 0) < 0 || write(lfd, buf, 32u) != 32 )
                dberror(-111, -1, 0);
            locklog(0);
        }
        l_tty[0]	 = 0;
        tl.PID		 = getpid();
        tl.LPID		 = lpid | (a1 << 16);
        tl.RecSize	 = 0;
        tl.TransType = 0x40u;
        tl.UID		 = getuid();
        time(&tl.TimeT);
        wrlog(0, 0, 1);
    }
}

void rdLogHeader()
{
    short BytesRead;
	
    BytesRead = read(lfd, &tl, 32u);
    if ( BytesRead != 32 )
    {
        if ( !BytesRead )
            return;
        dberror(-112, -1, 0);
    }
    if ( tl.Version != 3576 )
    {
        if ( tl.Version == 3575 )
			eprint("Wrong verion of transaction logfile(v3)\n");
        else
			eprint("Invalid transaction logfile\n");
        exit(1);
    }
}

int opentlog(int Context)
{
    //char	*v1;
	//char	*v2;
    char	*v5;
	int		v3;
	signed int v6;
	
    __mode_t umask_sav; // esi@13
    short trantype_sav; // [sp+1Ch] [bp-11Ch]@3
    char a3[128]; // [sp+20h] [bp-118h]@11
    char dest[128]; // [sp+A0h] [bp-98h]@3

    if ( lfd < 0 && logging )
    {
        trantype_sav = tl.TransType;
        //v1 = getevar("CLTBEFORELOG");
        //before_logging = strtol(v1, 0, 10);
        //v2 = getevar("CLTLOG");
        //strncpy(dest, v2, 128u);
        before_logging = strtol(getevar("CLTBEFORELOG"), 0, 10);
        strncpy(dest, getevar("CLTLOG"), 128u);

        if ( !dest[0] || (v3 = lenstr("NOLOG"), cmpbuf(dest, "NOLOG", v3)) )
        {
            logging = 0;
            return 0;
        }
        v5 = getenv("CLTMUSTEXIST");
        if ( v5 && *v5 == '1' )
        {
            v6 = 0;
            while ( 1 )
            {
                lfd = open64(dest, 2);
                if ( lfd >= 0 )
                    break;
                if ( !v6 )
                {
                    sprintf(a3, "CLTMUSTEXIST is set to 1, waiting for [%s] to appear", dest);
                    cllogerr(0, pname, a3, 0);
                    v6 = 1;
                    eprint("\n%s\n", a3);
                    fflush(stderr);
                }
                sleep(1u);
            }
        }
        else
        {
            umask_sav = umask(0);
            //lfd = open64(dest, 0x42);
            lfd = open64(dest, 0102,0666);
            umask(umask_sav);
            if ( lfd < 0 )
            {
                syserror("can't open logfile - %s", dest, 438);
                exit(1);
            }
        }
        rdLogHeader();
        startplog(Context);
        tl.TransType = trantype_sav;
    }
    return 1;
}

void fmtlog(int TDno)
{
    tl.UID		= getuid();
    tl.PID		= getpid();
    tl.LPID		= lpid;
    tl.RecSize	= recsize(TDno);
    tl.field_12 = 0;
    tl.TDno		= TDno;
    tl.cl_tx	= cl_tx;
    time(&tl.TimeT);
}

int clcdb(int (*a1)(int, char *, POS *), int TDno, char *WorkArea1, POS *pos, TDesc *TTptr, char *WorkArea)
{
    short	v6;
	int		v7;
	int		v8;
	short	v10;

    v10 = 0;
    if ( logging )
    {
        if ( TTptr && (TTptr->TDlocked & 0x0200 || noLogging(TDno)) )
        {
            logging = 0;
            v10 = 1;
        }
        if ( logging )
        {
            if ( lfd < 0 )
                opentlog(TTptr->DBcontext);
            if ( logging )
            {
                if ( !prev_tx )
                {
                    startran();
                    prev_tx = cl_tx;
                }
                if ( before_logging && tl.TransType == 0x10 && WorkArea )
                {
                    tl.TransType = 0x100u;
                    fmtlog(TDno);
                    wrlog(TTptr, WorkArea, 1);
                    tl.TransType = 16;
                }
                fmtlog(TDno);
                locklog(1);
                wrlog(TTptr, WorkArea1, 0);
            }
        }
    }
    
	v6 = a1(TDno, WorkArea1, pos);	// Call the passed in function   **** NOT RIGHT FOR putr ****
    v7 = v6;
    if ( v6 < 0 )
    {
        if ( !logging )
            goto LABEL_22;
        tl.RecSize		= 0;
        tl.cl_tx		= v6;
        tl.TransType	= 4;
        wrlog(0, 0, 0);
    }
    if ( logging )
        locklog(0);

LABEL_22:
    v8 = 1;
    if ( !v10 )
        v8 = logging;
    logging = v8;
    return v7;
}

void startran(void)
{
	short trantype_sav; // di@1

	trantype_sav	= tl.TransType;
    tl.PID			= getpid();
    tl.LPID			= lpid;
    time(&tl.TimeT);
    tl.RecSize		= 0;
    tl.TransType	= 1;
    tl.cl_tx		= cl_tx;
    wrlog(0, 0, 1);
    tl.TransType	= trantype_sav;
    tx_started		= 1;
}

void endtran(void)
{
	if ( lfd >= 0 && tx_started)
	{
		tl.PID			= getpid();
		tl.LPID			= lpid;
		time(&tl.TimeT);
		tl.RecSize		= 0;
		tl.TransType	= 2;
		wrlog(0, 0, 1);
		cl_tx++;
		prev_tx			= 0;
		tx_started		= 0;
	}
}

int closetlog(int a1)
{
    if ( lfd >= 0 )
    {
        if ( a1 >= 0 )
            endtran();
        tl.cl_tx		= a1;
        tl.PID			= getpid();
        tl.LPID			= lpid;
        tl.UID			= getuid();
        time(&tl.TimeT);
        tl.RecSize		= 0;
        tl.TransType	= 0x80u;
        wrlog(0, 0, 1);
        close(lfd);
        lfd = -1;
    }
    return 1;
}

void locklog(int a1)
{

    if ( lseek64(lfd, 0LL, 0) < 0 || lockf64(lfd, a1, 1LL) < 0 )
        dberror(-113, -1, 0);
}

void wrlog(TDesc *TTptr, char *WorkArea, int LockLog)
{
    char *v4;
    char *v5;
    char *v6;
    char *v7;
    char *v11; 
    size_t v12;
    short v14;
	
	char buf[32];
	char Dest[1024];			// wrong!!!! **************
	
    v14 = 0;
    tl.Version = 3576;
    
	cpybuf(buf, (char *)&tl, sizeof(TRANSLOG));		// 32 on x86

	if ( l_tty[0] )
        v5 = cdbcpystr(Dest, "\n\n", 0);
    else
    {
        v4 = cdbcpystr(Dest, _pname, "\n", 0);
        v5 = cdbcpystr(v4, pname, "\n", 0);
    }
    if ( TTptr )
    {
        v6 = cdbcpystr(v5, dtab[TTptr->DBnumber].FullDBname, "\n", 0);
        v7 = cdbcpystr(v6, TTptr->TableName, "\n", 0);
    }
    else
        v7 = cdbcpystr(v5, "\n\n", 0);

    if ( l_tty[0] )
        v11 = cdbcpystr(v7, "\n", 0);
    else
    {
        cdbcpystr(l_tty, ttyname(2), 0);
        
		if ( !l_tty[0] )
            sprintf(l_tty, "bg%d", getpid());

		v11 = cdbcpystr(v7, l_tty, "\n", 0);
    }
	
    v12 = (signed short)(v11 - buf);
    if ( tl.RecSize )
    {
        cpybuf(v11, WorkArea, tl.RecSize);
        v12 = (signed short)(tl.RecSize + v12);
    }
    
	if ( LockLog )
        locklog(1);
    
    if ( lseek64(lfd, 0LL, 2) < 0 || write(lfd, buf, v12) != v12 )	// seek to end of file
        v14 = -111;
    
	if ( LockLog )
        locklog(0);
    
	if ( v14 )
        dberror(-111, -1, 0);
}


#endif



