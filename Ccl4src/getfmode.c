#ifndef GETFILEMODE_C
#define GETFILEMODE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include <sys/stat.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values


unsigned short procstat(struct stat64 *s64)
{
    unsigned short v1; // edx@2

    if ( s64->st_uid == geteuid() )			// check if target file/dir owned by our UID
    {
        v1 = s64->st_mode & 0x1C0;
    }
    else if ( s64->st_gid == getegid() )	// check if target file/dir owned by our GID
	{
		v1 = (s64->st_mode & 0x38) << 3;
	}
	else									// not ours, go with default access settings
	{
		v1 = (s64->st_mode & 0x7) << 6;
	}
    return v1 | s64->st_mode & 0xFE00;		// include dir/file bits
}

unsigned short clgetmode(char *path)
{
	unsigned short v2 = 0;
	struct stat64 v4;
	
    if ( __xstat64(_STAT_VER, path, &v4) >= 0 )
        v2 = procstat(&v4);

	return v2;
}

/*

		printf("clgetmode(%s), sizeof(stat64) = %d\n",path,sizeof(v4));
		DumpBlock(&v4, sizeof(v4));
		printf("geteuid() = x%04X [%d]\n",geteuid(),geteuid());
		printf("getegid() = x%04X [%d]\n",getegid(),getegid());

		printf("v4.st_mode  = x%04X [%d] %s \n",v4.st_mode, v4.st_mode, DumpShortBits(v4.st_mode, bits));
		printf("v4.st_uid   = x%04X [%d] \n",v4.st_uid, v4.st_uid);
		printf("v4.st_gid   = x%04X [%d] \n",v4.st_gid, v4.st_gid);


leslie:x:7822:100:Leslie Ayling:/home/leslie:/bin/bash
geteuid() = x1E8E [7822]
getegid() = x0064 [100]

[leslie@apollo Cclenter]$ ls -al /usr/cl4/etc/sample.cl
-rwxrwxrwx. 1 root bin 1429 Jun 20  2014 /usr/cl4/etc/sample.cl

clgetmode(/usr/cl4/etc/sample.cl), sizeof(stat64) = 96
FFE04938 = 01 08 00 00 00 00 00 00 A8 59 E0 FF 66 2A 90 00  .........Y..f*..
FFE04948 = FF 81 00 00 01 00 00 00 00 00 00 00 01 00 00 00  ................
FFE04958 = 00 00 00 00 00 00 00 00 03 00 00 00 95 05 00 00  ................
FFE04968 = 00 00 00 00 00 10 00 00 08 00 00 00 00 00 00 00  ................
FFE04978 = 3F E0 96 56 D3 30 F0 1F 30 DB A3 53 F6 39 4F 26  ?..V.0..0..S.9O&
FFE04988 = 30 DB A3 53 76 06 56 28 66 2A 90 00 00 00 00 00  0..Sv.V(f*......

struct stat64 {
	dev_t st_dev;				ID of device containing the file.
	ino64_t st_ino;				Serial number for the file.
	mode_t st_mode;				Access mode and file type for the file (see Flags).
	nlink_t st_nlink;			Number of links to the file.
	uid_t st_uid;				User ID of file owner.
	gid_t st_gid;				Group ID of group owner.
	dev_t st_rdev;
	off64_t st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
	blksize_t st_blksize;
	blkcnt64_t st_blocks;
	mode_t st_attr;
}; 

*/

#endif
