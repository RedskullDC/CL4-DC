#ifndef RDHDR_C
#define RDHDR_C

#include <endian.h> // experimental swapping
#include "DBdefs.h"
#include "cl4.h"

// from /usr/include/bits/byteswap.h
//__bswap_constant_32(x)
//__bswap_constant_16(x)

short _rdhdr(short fd, DBinfo *DBptr)
{
	DBhdr hdr;

	//printf("_rdhdr(fd = x%04X, DBptr = x%08X)\n", fd,DBptr);

	if ( lseek64(fd, 0LL, 0) >= 0 && readfull(fd,(char*) &hdr, sizeof(DBhdr)) == sizeof(DBhdr))
	{
		DBptr->DBversion		= __bswap_constant_16(hdr.DBversion);
		DBptr->DBpgsize			= __bswap_constant_16(hdr.DBpgsize);
		DBptr->DBmaxpages		= __bswap_constant_32(hdr.DBmaxpages);
		DBptr->DBfreeaddress	= __bswap_constant_32(hdr.DBfreeaddress);
		DBptr->DBnumtables		= __bswap_constant_16(hdr.DBnumtables);
		DBptr->DBtableaddress	= __bswap_constant_32(hdr.DBtableaddress);
		DBptr->DBvflag			= __bswap_constant_16(hdr.DBvflag);
		DBptr->DBmaxtables		= __bswap_constant_16(hdr.DBmaxtables);
		DBptr->DBSemaphore		= 0;
		return DBptr->DBversion;			//return Version No.;

/*
		printf("_rdhdr: hdr.DBversion      = %d\n",DBptr->DBversion);
		printf("_rdhdr: hdr.DBpgsize       = %d\n",DBptr->DBpgsize);
		printf("_rdhdr: hdr.DBmaxpages     = %d\n",DBptr->DBmaxpages);	
		printf("_rdhdr: hdr.DBfreeaddress  = %d\n",DBptr->DBfreeaddress);
		printf("_rdhdr: hdr.DBnumtables    = %d\n",DBptr->DBnumtables);
		printf("_rdhdr: hdr.DBtableaddress = %d\n",DBptr->DBtableaddress);
		printf("_rdhdr: hdr.DBvflag        = %d\n",DBptr->DBvflag);
		printf("_rdhdr: hdr.DBmaxtables    = %d\n",DBptr->DBmaxtables);
*/
	}
	else
		return -1;		// error_exit
}

#endif

