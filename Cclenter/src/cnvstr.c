#ifndef SCONV_C
#define SCONV_C

#include <stdio.h>
#include <stdarg.h>			// for var args stuff
#include <ctype.h>			// tolower() etc.
#include <stdlib.h>			// strtod() etc.
#include <string.h>			// for strncmp()
#include <sys/stat.h>		// for stat64
#include <sys/types.h>
#include <grp.h>			// getgrgid(gid_t gid);
#include <openssl/sha.h>	// for SHA1()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// called by sconv() handler in evalstr()

// base64 encode/decode tables:

static const char nch[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/.0";

static const unsigned int nint[] = 
{
	0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 	0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
	0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
	0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x3F,  0x3E,
	0x34,  0x35,  0x36,  0x37,  0x38,  0x39,  0x3A,  0x3B,	0x3C,  0x3D,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
	0x00,  0x00,  0x01,  0x02,  0x03,  0x04,  0x05,  0x06,	0x07,  0x08,  0x09,  0x0A,  0x0B,  0x0C,  0x0D,  0x0E,
	0x0F,  0x10,  0x11,  0x12,  0x13,  0x14,  0x15,  0x16,	0x17,  0x18,  0x19,  0x00,  0x00,  0x00,  0x00,  0x00,
	0x00,  0x1A,  0x1B,  0x1C,  0x1D,  0x1E,  0x1F,  0x20,	0x21,  0x22,  0x23,  0x24,  0x25,  0x26,  0x27,  0x28,
	0x29,  0x2A,  0x2B,  0x2C,  0x2D,  0x2E,  0x2F,  0x30,	0x31,  0x32,  0x33,  0x00,  0x00,  0x00,  0x00,  0x00
};


char *base64de(char *src)
{
	char	*v6;
	char	*result;
	int		v4;
	int		v13;
	bool	v5;
	
	unsigned long v8;
	unsigned short CRC;
	unsigned short oldcrc;
	unsigned char v17[4];
	
	CRC = 0;
	v6 = getenv("CGIURLSEED");
	if ( v6 )
		CRC = strtol(v6, 0, 10);
	
	if ( *src && !strchr(src, '=') )
	{
		v6 = src;
		v4 = -1;
		do
		{
			//if ( !v4 )	// not possible. v4 set to -1 above
			//	break;
			v5 = *v6++ == 0;
			--v4;
		}
		while ( !v5 );
		
		if ( v4 << 30 == 0x80000000 )	// buffer must hold even number of chars?
		{
			v6 = src;
			for ( v13 = 0; *v6; )
			{
				v8 = nint[*v6++ & 0x7F] & 0x3F;
				v8 = (nint[*v6++ & 0x7F] & 0x3F) + (v8 << 6);
				v8 = (nint[*v6++ & 0x7F] & 0x3F) + (v8 << 6);
				v8 = (nint[*v6++ & 0x7F] & 0x3F) + (v8 << 6);
				
				ltoms((int *)v17, v8 << 8);
				buf_0[v13++] = v17[0];
				buf_0[v13++] = v17[1];
				buf_0[v13++] = v17[2];
			}
			buf_0[v13] = 0;
			
			oldcrc = (unsigned char)buf_0[1] | ((unsigned char)buf_0[0] << 8);
			
			CRC += acrc(0, &buf_0[3], strlen(&buf_0[3]));
			
			if ( CRC != oldcrc )
				sprintf(buf_0, "   E=htstart&cgierr=1&crc=%#x&oldcrc=%#x&refqs=%s", CRC, oldcrc, src);
		}
		else
			sprintf(buf_0,"   E=htstart&cgierr=2");

		result = trim(&buf_0[3]);
	}
	else
	{
		strcpy(buf_0, src);
		result = buf_0;
	}
	return result;
}


char *base64en(char *src)
{
	char *v6;
	
	int len;
	unsigned short CRC;
	
	int v7;
	unsigned long v12;
	
	short url_seed;				// allow an extra seed value for more variation
	unsigned char v16[4];
	unsigned char v17[4];
	
	char wk_buff[1048]; // [sp+20h] [bp-418h]@3

	url_seed = 0;
	v6 = getenv("CGIURLSEED");
	if ( v6 )
		url_seed = strtol(v6, 0, 10);

	strcpy(wk_buff, src);
	if ( strchr(src, '=') )		// at least one parameter is in the URL
	{
		len = strlen(src);
		for ( v6 = &wk_buff[len]; len != 3 * (len / 3); ++len )// pad with space to 4 byte boundary
			*v6++ = ' ';
		*v6 = 0;                                     // 
													// 
		CRC = acrc(0, wk_buff, len);
		itoms((short *)v17, url_seed + CRC);
		v17[2] = '=';                               // 61
		v17[3] = 0;
		
		v6 = wk_buff;
		v7 = 0;
		while (*v6)
		{
			if ( v7 )
			{
				v16[0] = *v6++;
				v16[1] = *v6++;
				v16[2] = *v6++;
				v16[3] = 0;
				v12 = mstol((int *)v16);
			}
			else
			{
				v12 = mstol((int *)v17);			// first time around save the CRC at start of buffer
			}
			buf_0[v7++] = nch[v12 >> 26];
			buf_0[v7++] = nch[(v12 >> 20) & 0x3F];
			buf_0[v7++] = nch[(v12 >> 14) & 0x3F];
			buf_0[v7++] = nch[(v12 >> 8) & 0x3F];
		}
		buf_0[v7] = 0;
	}
	else
	{
		strcpy(buf_0, src);		// return source unchanged
	}
	return buf_0;

}

unsigned char hash[SHA_DIGEST_LENGTH];

char *tabsub(char *Src, int max)
{
	char	*v11;
	char	*v3;
	char	*i;

    int		v2;
	int		prev;
	int		v10;
	char	v5;
	
	// LSCONV contains the length previously processed. 
	// SCONVERR is set to 1 when no more string is left.

    v10 = 0;
    prev = incvar(v_LSCONV, 0);
    v11 = &Src[prev];
    
	v2 = strlen(&Src[prev]);
    if ( v2 > max )
        v2 = max;

    strncpy(buf_0, &Src[prev], v2);
    buf_0[v2] = 0;

    if ( (v3 = strchr(buf_0, '\n')) != 0 || (v3 = strchr(buf_0, '\r')) != 0 )	// string contains a newline or carriage return!
    {
        v10 = 1;
        if ( *v3 == '\r' && v3 - buf_0 == max - 1 && v11[max] == '\n' )
            v10 = 2;
        
		*v3 = 0;
        if ( v3 > buf_0 && *(v3 - 1) == '\r' )
        {
            *--v3 = 0;
            ++v10;
        }
        v2 = v3 - buf_0;		// pointer arithmetic
    }
	else	// string does *not* contain a NEWLINE or CR
	{
		for ( i = &v11[v2]; *i; ++i )
		{
			v5 = *i;
			if ( v5 != TAB && v5 != '\r' && v5 != '\n' )
				break;
			++v10;
			if ( *i == '\n' )
				break;
		}
		if ( max == v2 && !v10 )
		{
			v3 = &buf_0[v2 - 1];
			if ( v3 > buf_0 )
			{
				if ( *v3 == TAB )
				{
					*v3 = 0;
					v2 = v3 - buf_0;
					v10 = 1;
					goto LABEL_27;
				}
				do
					--v3;
				while ( v3 > buf_0 && *v3 != TAB );
			}
			if ( *v3 == TAB )
			{
				*v3 = 0;
				v2	= v3 - buf_0;
				v10 = 1;
			}
		}
	}

LABEL_27:
    if ( *(&Src[v10] + v2 + prev) )	// is current char non-zero?
    {
        incvar(v_LSCONV, v2 + v10);
		setvar(v_SCONVERR, 0);		// Yes, there are string chars remaining 
    }
    else
    {
        setvar(v_LSCONV, v2);
		setvar(v_SCONVERR, 1);		// No, there are no more string chars left, possible error
    }
    return buf_0;
}


char *cnvstr(char *dest, char *command, const char *OpType, char *src)
{

	char	*v66;
	char	*v67;
	char	*v69;
	char	*v71;
	int		v74;
	char	*i;
	char	*v117;
	char	*sa;
	
	char	v120[10];
	struct stat64 buf;

	// sconv(url|soundex|tabsub|stat, en|de|ign|unign|0|1-9, string3)

	if (!strncmp(command,"stat",5))
	{
		*dest = 0;	// default return blank string
		if (!__xstat64(3, src, &buf))								// in this case, Src is a pathname
		{
			if (!strncmp(OpType,"mtime",6))
				sprintf(dest, "%ld", buf.st_mtime);	

			else if (!strncmp(OpType,"size",5))
				sprintf(dest, "%lld", buf.st_size);

			else if (!strncmp(OpType,"mode",5))
				sprintf(dest, "%0o", buf.st_mode);

			else if (!strncmp(OpType,"inode",6))
				sprintf(dest, "%ld", buf.st_ino);
			
			else if (!strncmp(OpType,"uid",4))						// token filtered by clcomp
				sprintf(dest, "%d", buf.st_uid);
			
			else if (!strncmp(OpType,"uname",6))					// token filtered by clcomp
				strcpy(dest, getpwuid(buf.st_uid)->pw_name);
			
			else if (!strncmp(OpType,"gid",4))						// token filtered by clcomp
				sprintf(dest, "%d", buf.st_gid);
			
			else if (!strncmp(OpType,"gname",6))					// token filtered by clcomp
				strcpy(dest, getgrgid(buf.st_gid)->gr_name);
			
			else if (!strncmp(OpType,"nlink",6))
				sprintf(dest, "%d", buf.st_nlink);
		}
	}
//===============================================
	else if (!strncmp(command,"base64",7))
	{
		if (!strncmp(OpType,"en",3))		// encode
		{
			strcpy(dest, base64en(src));
		}		
		else if (!strncmp(OpType,"de",3))	// decode
		{
			strcpy(dest, base64de(src));
		}		
		else
			strcpy(dest, src);									// neither, return unmodified
	}
//=============================================================================
	else if (!strncmp(command,"url",4))
	{
		v66 = 0;
		sa = strchr(src, '?');
		if ( sa )
		{
			sa++;
			strncpy(dest, src, sa - src);
		}
		else
			sa = src;
		dest[sa - src] = 0;
		
		v67 = strchr(sa, '#');
		v69 = v67;
		if ( v67 )
		{
			v66 = mstrcpy(v67, 0);
			*v69 = 0;
		}
		if ( first_0 )								// global in funcdefs.h
		{
			v69 = getenv("CL4_KEY");
			if ( v69 && *v69 )
				bfk_1 = 1;							// use blowfish mode if CL4_KEY set
			first_0 = false;
		}
		
		v71 = sa;	// default to do nothing, return to caller unchanged
		
		if (!strncmp(OpType,"de",3))					// decode
		{
			if ( bfk_1 )
				//v71 = Blowfish_StrDecrypt(sa, bfk_1);
				v71 = sa;
			else
				v71 = base64de(sa);

		}
		else if (!strncmp(OpType,"en",3))				// encode
		{
			if ( bfk_1 )
			{
				if ( strchr(sa, '=') )				// if has '=' decode, else return unchanged
					v71 = Blowfish_StrEncrypt(sa, bfk_1);
					//v71 = sa;
			}
			else
				v71 = base64en(sa);
		}
		strcat(dest, v71);
		if ( v66 )
		{
			strcat(dest, v66);
			mfree_0(v66);
		}
	}
//=================================================================
	else if (!strncmp(command,"crc",4))											// "crc" handler   - middle param ignored
	{																		// ex: sconv('crc','','testrsv.enc')
		sprintf(dest, "%d", (unsigned short)acrc(0, src, strlen(src)));
	}
//=================================================================
	else if (!strncmp(command,"xml",4))											// "xml" handler
	{
		if (!strncmp(OpType,"en",3))			// encode mode only
		{
			v117 = src;
			for ( i = dest; *v117; ++v117 )
			{
				switch ( *v117 )
				{
					case '<':
						strcat(i, "&lt;");
						i += 4;
						continue;
				   case '>':
						strcat(i, "&gt;");
						i += 4;
						continue;
					case '&':
						strcat(i, "&amp;");
						i += 5;
						continue;
					case '"':
						strcat(i, "&quot;");
						i += 6;
						break;
					case '\'':
						strcat(i, "&apos;");
						i += 6;
						break;
					default:
						if ( *v117 >= 0 && (signed int)(unsigned char)*v117 > 31 )
						{
							*i++ = *v117;
						}
						else	// control character 0-31, or 128-255
						{
							v74 = sprintf(v120, "#%d;", (unsigned char)*v117);
							strcat(i, v120);
							i += v74;
						}
						break;
				}
			}
		}
	}
//========================================================
	else if (!strncmp(command,"soundex",8))
	{
		if (!strncmp(OpType,"unign",6))
		{
			//soundexIgn(src, 0);	// unignore
		}
		if (!strncmp(OpType,"ign",4))
		{
			//soundexIgn(src, 1);	// ignore
		}
		else
		{
			//strcpy(dest, soundex(src, strtol(OpType, 0, 10)));
			strcpy(dest, src);		// *** debug code ***
		}
	}
//=====================================================================
	else if (!strncmp(command,"bf",3))
	{
		if (!strncmp(OpType,"en",3))
		{
			//strcat(dest, Blowfish_StrEncrypt(src, 0));
		}			
		else if (!strncmp(OpType,"de",3))
		{
			//strcat(dest, Blowfish_StrDecrypt(src, 0));
		}
		else
			strcat(dest, "");
	}
//=======================================================
	else if (!strncmp(command,"tabsub",7))
	{
		strcpy(dest, tabsub(src, strtol(OpType, 0, 10)));
	}
//===================================================================
	else if (!strncmp(command,"sha0",5))
	{
		sha0_encrypt(src, dest);
	}
//===========================================					*** DC Extension - SHA1 encode **
	else if (!strncmp(command,"sha1",5))
	{
		//printf("src = %s\n",src);
		memset(hash, 0x00, SHA_DIGEST_LENGTH);
		SHA1(src, strlen(src), hash);							// hash now contains the 20-byte SHA-1 hash
		register int idx;
		for (idx = 0; idx < SHA_DIGEST_LENGTH; idx++)					// ** no sanity check on size of dest buffer **
			sprintf((char*)&(dest[idx*2]), "%02x", hash[idx]);
	}
	else
		strcpy(dest, src);	// default action: return string unmodified

	return dest;
}

#endif
