#ifndef READPG_C
#define READPG_C

#include "DBdefs.h"
#include "cl4.h"

bool _readpg(DBinfo *DBptr,char *BUFF, int PageNo)
{
	unsigned int DBpagesize;
	off64_t ByteFilePos;

    //printf("_readpg(DBptr: %0X,Buff: %0X,PageNo: %d) Called.\n" ,DBptr, BUFF, PageNo);
	DBpagesize	= DBptr->DBpgsize;
	ByteFilePos = PageNo * DBpagesize;
	if ( lseek64(DBptr->DBfilehandle,ByteFilePos, 0) < 0 || (DBpagesize != readfull(DBptr->DBfilehandle, BUFF, DBpagesize)))
		return false;
	return true;
}

#endif
