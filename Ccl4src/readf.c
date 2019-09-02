#ifndef READFULL_C
#define READFULL_C

//#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> /* for mode_t */
#include "DBdefs.h"

unsigned int readfull(short fd, char *Buffer, unsigned int NumBytes)
{
	unsigned int TotalBytesRead;
	ssize_t BytesRead;

	//printf("readfull(FD: x%04X,Buff: x%08X, NumBytes : %d)\n" ,fd,Buffer,NumBytes);

	//Buffer = buf;
	*(int*)__errno_location() = 0;
	for ( TotalBytesRead = 0; TotalBytesRead < NumBytes; TotalBytesRead += BytesRead )
	{
		BytesRead = read(fd, Buffer, NumBytes - TotalBytesRead);
		if ( !BytesRead )
			break;
		Buffer += BytesRead;
	}
	return TotalBytesRead;
}
#endif

