#ifndef TIMES_C
#define TIMES_C

#include <math.h>			// for fmod in time functions
#include <ctype.h>			// for isdigit() in gettime()
#include <sys/time.h>		// for gettimeofday() etc.
#include <stdlib.h>			// for strtol()
#include "DBdefs.h"
#include "cl4.h"

char* systime(char *s)
{
	char *v1;
	time_t v3;
	time_t timer;
	
	timer = 0;
	v3 = time(&timer);
	v1 = ctime(&v3);
	sprintf(s, "%8.8s", v1 + 11);		// Www Mmm dd hh:mm:ss yyyy  [v1+11 = hh:mm:ss]
	return s;
}

char* gmttime(char *s)
{
	struct tm *v1;
	time_t v3;
	time_t timer;
	
	timer = 0;
	v3 = time(&timer);
	v1 = gmtime(&v3);
	sprintf(s, "%02d:%02d:%02d", v1->tm_hour, v1->tm_min, v1->tm_sec);
	return s;
}

int getmsecs()
{
	struct timeval tv;

	gettimeofday(&tv, 0);
	return tv.tv_usec / 1000;
}

double gettime(const char *src)
{
	char *v2;
	int PMflg;
	int v6;
	int v7;
	int MilliSecs;
	int Hours;
	int Mins;
	int Secs;
	char s[32];
	char Millis[32];

	if ( !src || !*src || strlen(src) > 31 )	// Null buffer, or too long
		return 0.0;

	strcpy(s, src);	// max 32chars due to test above

	MilliSecs = -1;
	if( !compare(s, "n"))	// look for short string command overrides!
	{
		systime(s);
		MilliSecs = getmsecs();
	}
	else if( !compare(s, "gmt") )
	{
		gmttime(s);
		MilliSecs = getmsecs();
	}
	else if( !s[0] || isalpha(s[0]))	// First char not a number? Signal error condition
		return -1.0;

	v2 = &s[strlen(s) - 1];	// Look for last digit as AM/PM specifier
	PMflg = 0;
	if ( *v2 == 'p' || *v2 == 'P' || *v2 == '-' )
		PMflg = 1;

	// replace all non-numerics with SPC
	v2 = s;
	while ( *v2 )
	{
		if ( !isdigit(*v2))
			*v2 = ' ';
		v2++;
	}
	
	Hours = 0;
	Mins = 0;
	Secs = 0;
	Millis[0] = 0;	// String value of milliseconds read in
	
	if ( s[0] == ' ' )	// assume no hours value entered
		sscanf(s, "%d %d %s", &Mins, &Secs, Millis);
	else
		sscanf(s, "%d %d %d %s", &Hours, &Mins, &Secs, Millis);

	if ( PMflg )	// convert to 24hr time, range check.
	{
		if ( Hours <= 12 )	// logic incorrect in original . Can't enter an AM/PM value from Midnight to 1:00am !!
		{
			Hours += 12;
			if ( Hours == 24 )
			{
				if ( Mins || Secs )
					Hours = 12;
			}
		}
	}
	if ( Hours > 24 || Mins > 59 || Secs > 59 || Hours == 24 && (Mins || Secs) )
		return -1.0;
	
	if ( MilliSecs < 0 )	// Set above if string was 'n' or 'gmt'
	{
		if ( v6 = strlen(Millis))
		{
			switch (v6)
			{
				case 1 :
					v7 = 100;
					break;
				case 2 :
					v7 = 10;
					break;
				default:
					Millis[3] = 0;
					v7 = 1;
					break;
			}
			MilliSecs = v7 * strtol(Millis, 0, 10);
		}
		else
			MilliSecs = 0;
	}
	return clround(Hours + Mins / 60.0 + Secs / 3600.0 + MilliSecs / 3600000.0, '7');
}

int convtime(double a1, int *Hours, int *Mins, int *Secs, int *Millisecs)
{
	int		result;
	int		v7;
	int		v9;
	double	v8;

	*Millisecs = 0;
	*Secs = 0;
	*Mins = 0;
	*Hours = 0;
	result = 0;
	if ( a1 > 0.0 )
	{
		v7 = (int)a1;
		v8 = (a1 + 0.00000005) - (int)a1 ;
		v9 = fmod(v8 * 3600.0, 60);

		*Hours	= v7;
		*Mins	= ((v8 * 3600.0) - v9) / 60;
		*Secs	= v9;
		*Millisecs = ((int)(v8 * 3600000.0)) % 1000;
		result = 1;
	}
	return result;
}

char *maketime(char *s, double a2)
{
	int		Hours;
	int		Mins;
	int		Secs;
	int		Millisecs;

	*s = 0;
	if ( convtime(a2, &Hours, &Mins, &Secs, &Millisecs) )
		sprintf(s, "%2.2d:%2.2d", Hours, Mins);
	return s;
}

char *maketimehms(char *s, double a2)
{
	int		Hours;
	int		Mins;
	int		Secs;
	int		Millisecs;

	*s = 0;
	if ( convtime(a2, &Hours, &Mins, &Secs, &Millisecs))
		sprintf(s, "%02d:%02d:%02d.%03d", Hours, Mins, Secs, Millisecs);
	return s;
}

char* tfmt(char *s,char *format, double dbTime)
{
	char *i; // edi@11
	char *v5; // esi@15
	char *v6; // edi@15
	char v8; // dl@26
	int v9; // edx@42
	int v14;
	signed int v10; // eax@46
	unsigned int v11; // edx@61
	unsigned int v12; // edx@71
	
	double v13;
	double v15;
	
	signed short AM_PM; // [sp+30h] [bp-78h]@1

	int v18; // [sp+34h] [bp-74h]@1
	short m; // [sp+34h] [bp-74h]@78
	int v20; // [sp+38h] [bp-70h]@1
	short l; // [sp+38h] [bp-70h]@68
	int v22; // [sp+3Ch] [bp-6Ch]@1
	short k; // [sp+3Ch] [bp-6Ch]@58
	int v24; // [sp+40h] [bp-68h]@1
	short j; // [sp+40h] [bp-68h]@50
	
	signed int v26; // [sp+44h] [bp-64h]@1
	int Hours; // [sp+50h] [bp-58h]@2
	int Mins; // [sp+54h] [bp-54h]@2
	int Secs; // [sp+58h] [bp-50h]@2
	int Millisecs;
	struct tm tp;

	//printf("tfmt(Format = %s, TimeValue = %G\n",format, dbTime);

	v26 = 0;
	v24 = 0;
	v22 = 0;
	v20 = 0;
	v18 = 0;
	AM_PM = 0;
	if ( strlen(format) <= 0 )
	{
		//shouldn't be reachable in libcl4()
		// _getfldno always sets the format. default = "hh:mm:ss"
		maketime(s, dbTime);
		return s;
	}
	
	convtime(dbTime, &Hours, &Mins, &Secs, &Millisecs);
	if ( Hours > 24 || Hours == 24 && (Mins || Secs || Millisecs) )
		Hours %= 24;

// if first char is a '%', use ctime's strftime formatting function:
// Not all specifiers will make sense, as many fields set to zero below!
	if ( *format == '%' )	
	{
		tp.tm_sec	 = Secs;
		tp.tm_min	 = Mins;
		tp.tm_hour	 = Hours;
		tp.tm_mday	 = 1;
		tp.tm_mon	 = 0;
		tp.tm_year	 = 0;
		tp.tm_wday	 = 0;
		tp.tm_yday	 = 0;
		tp.tm_isdst  = 0;
		tp.tm_gmtoff = 0;
		tp.tm_zone	 = 0;
		strftime(s, 48u, format, &tp);
		if ( dbTime < 0.0 )
		{
			for ( i = s; *i; ++i )
				*i = ' ';
		}
		return s;
	}

	v5 = s;
	v6 = format;
	while ( 2 )
	{
		switch ( v26 )	// always zero on first run through
		{
			case 0:
				switch ( *v6 )	// current char in format buffer
				{
					case 'H':
						AM_PM = 1;	// fall through to 'h'
					case 'h':
						v24++;
						v26 = 1;
						++v6;
						continue;
					case 'm':
						v22++;
						v26 = 2;
						++v6;
						continue;
					case 's':
						v20++;
						v26 = 4;
						++v6;
						continue;
					case 'S':
						v18++;
						v26 = 8;
						++v6;
						continue;
					default:
						if ( *v6 )
						{
							if ( *v6 != '_' && dbTime > 0.0 )
							{
								if ( *v6 == '\\' && v6[1] && v6[1] != '\\' )
									++v6;
							}
							else
							{
								*v6 = ' ';
							}
							*v5++ = *v6++;	// copy byte to output buffer and bump
							continue;
						}
						// reached the end of the format string!
						if ( AM_PM )
						{
							if ( dbTime <= 0.0 )
							{
								*v5++ = ' ';
								*v5++ = ' ';
							}
							else
							{
								v8 = 'a';
								if ( AM_PM != 1 )
									v8 = 'p';
								*v5++ = v8;
								*v5++ = 'm';
							}
						}
						*v5 = 0;
						return s;
				}
			case 1:		// Hours handler
				if ( *v6 == 'h' || *v6 == 'H' )
				{
					v24++;
				}
				else
				{
					--v6;
					if ( AM_PM )
					{
						AM_PM = 2 - ((Hours - 12) >= 12);
						v9 = Hours % 12;
						if ( !(Hours % 12) )
							v9 = 12;
					}
					else
					{
						v9 = Hours;	// 24hour format
					}
					
					if ( v9 > 9 )
					{
						v10 = 2;
						if ( v24 > 1 )
							v10 = v24;
						v24 = v10;
					}
					if ( dbTime > 0.0 )
					{
						sprintf(v5, "%*d", v24, v9);
						v5 += v24;
					}
					else
					{
						for ( j = v24 - 1; j != -1; --j )
							*v5++ = ' ';
					}
					v24 = 0;
					v26 = 0;
				}
				v6++;
				continue;
			case 2:		// Minutes handler
				if ( *v6 == 'm' )
				{
					v22++;
				}
				else
				{
					--v6;
					if ( dbTime > 0.0 )
					{
						v11 = 100;
						if ( v22 == 1 )
							v11 = 10;
						sprintf(v5, "%0*ld", v22, Mins % v11);
						v5 += v22;
					}
					else
					{
						for ( k = v22 - 1; k != -1; --k )
							*v5++ = ' ';
					}
					v22 = 0;
					v26 = 0;
				}
				v6++;
				continue;
			case 4:		// Seconds handler
				if ( *v6 == 's' )
				{
					v20++;
				}
				else
				{
					--v6;
					if ( dbTime > 0.0 )
					{
						v12 = 100;
						if ( v20 == 1 )
							v12 = 10;
						sprintf(v5, "%0*ld", v20, Secs % v12);
						v5 += v20;
					}
					else
					{
						for ( l = v20 - 1; l != -1; --l )
							*v5++ = ' ';
					}
					v20 = 0;
					v26 = 0;
				}
				v6++;
				continue;
			case 8:		// Milliseconds handler
				if ( *v6 == 'S' )
				{
					v18++;  // Number of Milliseconds to show
				}
				else
				{
					--v6;
					if ( dbTime > 0.0 )
					{
						if ( v18 == 1 )		
							v13 = 100.0;
						else if ( v18 == 2 )
							v13 = 10.0;
						else		// Show xxx millisecs
						{
							v13 = 1.0;
							v14 = v18 - 3;
							if ( v18 != 3 )
							{
								do
								{
									v13 = v13 / 10.0;
									--v14;
								}
								while ( v14 );
							}
						}
						v15 = clround(((double)Millisecs / v13), '0');
						printf("tfmt v15 = %G\n",v15);
						sprintf(v5, "%0*.0f", v18, v15);
						v5 += v18;
					}
					else
					{
						for ( m = v18 - 1; m != -1; --m )
							*v5++ = ' ';
					}
					v18 = 0;
					v26 = 0;
				}
				v6++;
				continue;
			default:
				v6++;
				continue;
		}
	}
}
#endif
