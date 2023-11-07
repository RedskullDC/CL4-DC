#ifndef BUPSEM_C
#define BUPSEM_C

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include "DBdefs.h"
#include "cl4.h"

int cdbftok(char *FullPathName)
{
	int v1;
	int v2;
	struct stat64 buf;

    v1 = stat64(FullPathName, &buf);
	//DumpBlock((char*) &buf, sizeof(struct stat64));
	//printf("cdbftok: buf.st_ino   = x%08X\n",buf.st_ino);

	v2 = 0;
	if ( !v1 )
		//v2 = (v4[0] << 24) | v5;
		// low byte of device ID + low int of 64bit file serial number
		v2 = (buf.st_dev << 24) | (unsigned int) buf.st_ino;
	return v2;
}

int check_bup_sem(DBinfo *DBptr)		// bitmap update semaphore
{
	int v1 = 0;	// Must be set to ZERO, as tmp files will not satisfy the if statement below. (No Semaphore or DBName)
	int v2;
	int v3;
	struct sembuf sops;	// not typedef'ed?

	//eprint("check_bup_sem start  v1 = %d , DBptr = x%08X , DBptr->DBSemaphore = x%08X , DBptr->DBname = %s            \n",v1,DBptr,DBptr->DBSemaphore,DBptr->DBname);

	//memset(&sops, 0, 4u);	// this line, or next two
	sops.sem_num	= 0;
	sops.sem_op		= 0;
	sops.sem_flg	= 0;
	//if ( DBptr->DBSemaphore > 0 || (DBptr->DBname && (v2 = cdbftok(DBptr->DBname), DBptr->DBSemaphore = v2, v1 = 55, v2 != -1)) )
	if ( DBptr->DBSemaphore > 0 || (DBptr->DBname && (v1 = 55, (DBptr->DBSemaphore = cdbftok(DBptr->DBname)) != -1)) )
	{
		v3 = semget(DBptr->DBSemaphore, 1, 0);	// get a semaphore on the Database being updated 
		v1 = 0;
		if ( v3 != -1 )
		{
			if ( semop(v3, &sops, 1u) != -1 || *(int*)__errno_location() == 43 )
				v1 = 0;		// success
			else
				v1 = 57;	// error
		}
	}
	return v1;
}

#endif

