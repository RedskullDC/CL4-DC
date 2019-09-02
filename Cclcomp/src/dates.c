#ifndef DATES_C
#define DATES_C

#include <langinfo.h>		// for nl_langinfo functions
#include <time.h>			// for struct tm
#include <string.h>
#include <ctype.h>          // for islower() and toupper() functions
#include <stdlib.h>			// for strtol()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void _check_YD()
{
	char *v0;

	//eprint("_checkYD(): dateDone = %d, _YD = %s\n",dateDone, _YD);
	if ( !dateDone && _YD )
	{
		v0 = strchr(_YD, '-');
		if ( v0 )
		{
			fromYear	= strtol(_YD, 0, 10);
			toYear		= strtol(v0 + 1, 0, 10);
			if ( fromYear <= 1899 || toYear > 2099 || fromYear > toYear )
			{
				fromYear	= 1950;	// def values *were* 1900 and 1999 respectively
				toYear		= 2050;
			}
		}
		// look for a default width specifier
		v0 = strchr(_YD, ':');
		if ( v0 )
		{
			defDateWidth = strtol(v0 + 1, 0, 10);
			if ( (defDateWidth - 8) > 2 )
				defDateWidth = 10;
		}
		//printf("_checkYD(): fromYear = %d, toYear = %d\n", fromYear, toYear);
		dateDone = 1;
	}
}

void loadMonthArrays(bool SetAvars)
{
	char	*v15;
	int		Day;
	int		Month;
	int		v16;

	// see  <langinfo.h> for constants
	strcpy(THOU_SEP, nl_langinfo(THOUSEP));
	strcpy(RADIX_CHAR, nl_langinfo(RADIXCHAR));

	wday = mmalloc(28u);	// 4 * 7

	for (Day = 0; Day < 7 ; Day++)
	{
		wday[Day] = mstrcpy(nl_langinfo(Day + DAY_1), 0);
		if ( SetAvars )
		{
			setavar(v_DAYNAMES, nl_langinfo(Day + DAY_1), Day);
			setavar(v_ABDAYNAMES, nl_langinfo(Day + ABDAY_1), Day);
		}
	}

	fmth = mmalloc(52u);	// 4 * 13
	mth = mmalloc(52u);

	for ( Month = 0; Month < 12 ; Month++ )
	{
		fmth[Month + 1] = mstrcpy(nl_langinfo(Month + MON_1), 0);
		mth[Month + 1] = mstrcpy(nl_langinfo(Month + ABMON_1), 0);
		if ( SetAvars )
		{
			setavar(v_MONTHNAMES, nl_langinfo(Month + MON_1), Month);
			setavar(v_ABMONTHNAMES, nl_langinfo(Month + ABMON_1), Month);
		}
	}

	localeconv();
	if ( !*_YL && !_DF )	// ***ERROR*** DF is ignored if no second language set!
	{
		v15 = nl_langinfo(D_FMT);	// 131113
		v16 = -1;
		while ( *v15 )
		{
			switch ( *v15++ )	// Examine the system DateFormat string to see what comes first 
			{
				case 'D':
				case 'E':
				case 'd':
				case 'e':
					v16 = 0;	// DD-MM-YY[YY]
					break;
				case 'M':
				case 'm':
					v16 = 1;	// MM-DD-YY[YY]
					break;
				case 'Y':
				case 'y':
					v16 = 2;	// YYYY-MM-DD
					break;
				default:
					break;
			}
			if ( v16 >= 0 )
			{
				_DF = v16;
				if ( SetAvars )
					setvar(v_DATEFORM, _DF);
				return;
				
			}
		}
	}
}

void unloadMonthArrays(void)
{
    int count;

    count = 0;
    do
        mfree_0(wday[count++]);
    while ( count < 7 );
    mfree_0(wday);
    
	count = 0;
    do
    {
        mfree_0(fmth[count + 1]);
        mfree_0(mth[count++ + 1]);
    }
    while ( count < 12 );
	mfree_0(fmth);
    mfree_0(mth);
}

void dateCheck(int a1)
{
    char *v2;

    _check_YD();
    if ( !a1 )
    {
        eprint("Year Range = %d - %d, default Date width = %d\n", fromYear, toYear, defDateWidth);
        if ( *_YL )
        {
            eprint("Seconday language - ");
			switch (toupper(*_YL))
			{
			case 'J':
                v2 = "Japanese";
				break;
			case 'K':
				v2 = "Korean";
				break;
			case 'T':
				v2 = "Thai";
				break;
			case 'C':
				v2 = "Chinese";
				break;
			default:
                v2 = "Unknown";
				break;
			}
            eprint(v2);
            eprint("\n");
            return;
        }
    }
}

char* sysdate(char *s)
{
	char *v1;
	time_t v7;
	time_t timer;

	_check_YD();
	timer = 0;
	v7 = time(&timer);
	v1 = ctime(&v7);
	//printf("ctime returned: %s", v1);	// has \n embedded already!
	//printf("sysdate: _DF = %d\n",_DF);

	switch (_DF)
	{
	case 0:
		sprintf(s, "%2.2s %3.3s %4.4s", &v1[8], &v1[4], &v1[20]);
		break;
	case 1:
		sprintf(s, "%3.3s %2.2s %4.4s", &v1[4], &v1[8], &v1[20]);
		break;
	default:		// _DF == 2, or any other value
		sprintf(s, "%4.4s %3.3s %2.2s", &v1[20], &v1[4], &v1[8]);
		break;
	}
	return s;
}

// called by clgetdate()
char* gmtdate(char *s)
{
	time_t v4;
	time_t timer;
	
	timer = 0;
	v4 = time(&timer);
	strftime(s, 20u, nl_langinfo(D_FMT), gmtime(&v4));
	return s;
}

int clgetdate(const char *src)
{
	int v2;
	char v3;
	char *v4;
	char *v13;
	char *v14;
	const unsigned short **v5;
	signed int v8;
	signed int v10;
	signed int v11;
	signed int v12;
	short v18;
	int v20;
	int v23;
	short v24;
	int v28;
	signed int v29;
	signed int v30;
	int v31;
	int v32;
	signed int v33;
	signed int v34;
	int v35;
	signed int v36;
	signed int v38;
	signed int v46;
	signed int v48;
	
	char DateAlias;		// Single letter command
	//bool JanFeb;
	int LeapYear;
	int Day;
	int Year;
	int Month;
  
	char s[30];			// src is copied here, max 29chars + \0
	char v51[32];
	char v52[32];		// sysdate string stored here
	char curDays[16];
	char curMonth[16];
	char curYear[16];
	char sYear[16];
	char sMonth[10];		// not sure of length
	char sDays[10];		// not sure of length  = Year value is read in here, assume 4 chars + \0

	//printf("clgetdate(%s)\n" ,src);

	v48 = 1;
	zap(s, 30u);
	strncpy(s, src, 29u);
	sysdate(v52);	//sysdate returns string like [23 Jan 2013]
	DateAlias = 0;

	v4 = s;
	v2 = -1;
	do
	{
		if ( !v2 )
			break;		// theoretically not reachable, since v2 set to -1 prior to block
		v3 = *v4++ == 0;
		--v2;
	}
	while ( !v3 );

//-----------------
	if ( v2 == -3 )		// was the passed string a single char command?
	{
		//eprint("v2 = %d\n",v2);
		switch ( s[0] )
		{
			case 'E':	// end of the month
			case 'e':
			case 'L':
			case 'l':

			case 'F':	// First of the month
			case 'f':
			
			case 'N':	// Next day == Tomorrow
			case 'n':
			
			case 'P':	// Previous/Yesterday
			case 'p':
			case 'Y':
			case 'y':
				DateAlias = s[0];	// fall through to case T:
			case 'T':	// Today
			case 't':					
		        cdbcpystr(s, v52, 0);	// Copy sysdate() to buffer
				break;
			default:
				break;
		}
	}
	else if ( !compare(s, "gmt") )
	{
		gmtdate(v52);	//	gmtdate returns something like [23/01/13]   DD/MM/YY
		cdbcpystr(s, v52, 0);
	}
//printf("#301 s = %s, v52 = %s, DateAlias = %c\n",s,v52, DateAlias);

//---------------
	v4 = v52;
	v5 = __ctype_b_loc();
	while ( *v4)
	{
		// code is faster leaving as direct pointer!!
		if ( !((*v5)[*v4] & 0x0008) )		// isalnum()  ==	0x0008
			*v4 = ' ';
		v4++;
	}
//printf("#312 s = %s, v52 = %s\n",s,v52);

//---------------
	v4 = s;
	while ( *v4 )
	{
		if ( !((*v5)[*v4] & 0x0008) )		// isalnum()  ==	0x0008
			*v4 = ' ';
		v3 = ((*v5)[*v4] & 0x0800) == 0;	// 0x0800 == isdigit()
		v8 = 0;
		if ( !v3 )
			v8 = v48;
		v48 = v8;
		++v4;
	}
//printf("#312 s = %s, v48 = %d\n",s,v48);
//---------------
	if ( v48 )	// Enter this block is there was nothing but digits in the date string
	{
		if ( _DF != 2 )
			goto LABEL_125;
		v4 = s;
		v10 = -1;
		do
		{
			if ( !v10 )
				break;
			v3 = *v4++ == 0;
			--v10;
		}
		while ( !v3 );
		if ( v10 != -10 )
		{
LABEL_125:
			v11 = 2;	// ddmmyy[yy] or mmddyy[yy]
			v46 = 4;
		}
		else
		{
			v11 = 4;	// yyyymmdd
			v46 = 6;
		}
		
		v12 = 1;
		v13 = s;
		v14 = v51;
		if ( s[0] )
		{
			do
			{
				*v14++ = *v13++;
				if ( v12 == v11 || v12 == v46 )
					*v14++ = ' ';	// insert a space
				v12++;
			}
			while ( *v13 );
		}
		*v14 = 0;
		cdbcpystr(s, v51, 0);
	}
//---------------
//	Try and parse what we can from the string

	sYear[0]	= 0;
	sMonth[0]	= 0;
	sDays[0]	= 0;
	switch (_DF)
	{
	case 0 :			// _DF == 0, format = dd-mm-yy[yy]
		sscanf(s, "%s%s%s", sDays, sMonth, sYear);
		sscanf(v52, "%s%s%s", curDays, curMonth, curYear);
		break;
	case 1:				// _DF == 1, format = mm-dd-yy[yy]
		sscanf(s, "%s%s%s", sMonth, sDays, sYear);
		sscanf(v52, "%s%s%s", curMonth, curDays, curYear);
		if ( !sDays[0])	// Assume if only one value is found, it is a days value
		{
			cdbcpystr(sDays, sMonth, 0);
			sMonth[0] = 0;
		}
		break;
	default:			// _DF == 2, format = yy[yy]-mm-dd
		sscanf(s, "%s%s%s", sYear, sMonth, sDays);
		sscanf(v52, "%s%s%s", curYear, curMonth, curDays);
		if ( sMonth[0] )
		{
			if ( sDays[0] )
				break;
			cdbcpystr(sDays, sMonth, 0);	// Assume if only two values found, they are month and days
			cdbcpystr(sMonth, sYear, 0);
		}
		else	// Assume if only one value is found, it is a days value
		{
			cdbcpystr(sDays, sYear, 0);
		}
		sYear[0] = 0;
		break;
	}
	// insert current day values for any missing values
	if ( !sMonth[0] )
		cdbcpystr(sMonth, curMonth, 0);
	if ( !sYear[0] )
		cdbcpystr(sYear, curYear, 0);

//---------------
	v4 = sYear;
	if ( sYear[0] )
	{
		while ( (*v5)[*v4] & 0x0800 )	// looks wrong    ,  x0800 == isdigit()
		{
			++v4;
			if ( !*v4 )
				goto LABEL_55;
		}
		return -1;	// error exit
	}

LABEL_55:
	Year = strtol(sYear, 0, 10);
	if ( Year < 0 || (Year - 100) <= 900u )
		return -1;	// error exit
	if ( Year <= 99 )	// 2 digit year value, do a range check
	{
		v20 = Year + 1900;
		if ( v20 < fromYear || v20 > toYear )
		{
			v20 = Year + 2000;
			if ( v20 < fromYear || v20 > toYear )
				return -1;	// error exit
		}
		Year = v20;
	}
//---------------
//eprint("#449 Year = %d\n", Year);	
	if ( (Year - 1801) <= 398u )	// Ensure year >= 1801 and <= 2199
	{

		Month = strtol(sMonth, 0, 10);	// should fail on linux, sMonth is Jan,Feb,etc.
		if ( Month <= 0 || Month > 12 )
		{
			sMonth[3] = 0;		// Limit sMonth to 3 chars, Jan,Feb,etc.
			Month = 0;
			v23 = 0;
			while ( 1 )
			{
				if ( !compare(sMonth, mth[v23 + 1]) ) // compare strings, ignore case
					Month = (v23 + 1);
				v23++;
				if ( Month > 0 )
					break;
				if ( v23 > 11 )
				{
					if ( Month <= 0 )
						return -1; // Error;
					break;
				}
			}
		}

		v24 = strtol(sDays, 0, 10);
		Day = v24;
		if ( v24 > 0 )
		{
			LeapYear = 0;
			if ( !(Year & 3) && Year % 100 || !(Year % 400) )
				LeapYear = 1;
			if ( day_tab[Month + 13 * LeapYear] >= v24 )	// check for sanity on Day value
			{
				switch ( DateAlias )
				{
					case 'P':	// Yesterday
					case 'p':
					case 'Y':
					case 'y':
						Day--;
						if ( !Day)	// true if we were 1st day of the month
						{
							Month--;
							if ( !Month )	// true if we were 1st month of the year
							{
								Month = 12;
								Year--;
								LeapYear = 0;	// re-calc LeapYear flag, as we crossed a year boundary!
								if ( !(Year & 3) && Year % 100 || !(Year % 400) )
									LeapYear = 1;
							}
							Day = day_tab[Month + 13 * LeapYear]; // set Day to last day of prev month
						}
						break;
					case 'N':	// tomorrow
					case 'n':
						Day = (v24 + 1);
						break;
					case 'E':	// Last day of the month
					case 'e':
					case 'L':
					case 'l':
						// No need to re-calc LeapYear here, year didn't change.
						//LeapYear = 0;
						//if ( !(Year & 3) && Year % 100 || !(Year % 400) )
						//	LeapYear = 1;
						Day = day_tab[Month + 13 * LeapYear];
						break;
					case 'F':	// First day of the month
					case 'f':
						Day = 1;
						break;
					default:
						break;	// default == do nothing if no command found
				}
//eprint("#530 Day = %d, Month = %d, Year = %d\n", Day, Month, Year);	
//--------------------------------------------------
				//JanFeb = Month <= 2;

				if ( Year == 2100 && Month > 2 )
					goto LABEL_126;
				if ( Year <= 2100 )
				{
					if ( Year == 1900 && Month <= 2 )
						goto LABEL_127;
					if ( Year > 1899 )
					{
						if ( Month > 2 )
							v35 = 1461 * Year;
						else
							v35 = 1461 * (Year - 1);
						v36 = v35 + 3;	// unnecessary code to trap /0 error. Year can't be zero, tested above.
						if ( v35 > -1 )
							v36 = v35;
						if ( Month > 2 )
							v38 = 153 * (Month + 1);
						else
							v38 = 153 * Month + 1989;
						return Day + v38 / 5 + (v36 >> 2) - 443;
					}
				
					if ( Month > 2 )
						v32 = 1461 * Year;
					else
LABEL_127:
						v32 = 1461 * (Year - 1);
					v33 = v32 + 3;	// unnecessary code to trap /0 error. Year can't be zero, tested above.
					if ( v32 > -1 )
						v33 = v32;
					v34 = v33 >> 2;
					if ( Month > 2 )
						v31 = Day + (153 * (Month + 1)) / 5 + v34 - 442;
					else
						v31 = Day + (153 * Month + 1989) / 5 + v34 - 442;
					return v31;
				}
				else
				{
					// (Year == 2100 && Month > 2 )|| year > 2100
					if ( Month > 2 )
LABEL_126:
						v28 = 1461 * Year;
					else
						v28 = 1461 * (Year - 1);
					v29 = v28 + 3;	// unnecessary code to trap /0 error. Year can't be zero, tested above.
					if ( v28 > -1 )
						v29 = v28;
					v30 = v29 >> 2;
					if ( Month > 2 )
						v31 = Day + (153 * (Month + 1)) / 5 + v30 - 444;
					else
						v31 = Day + (153 * Month + 1989) / 5 + v30 - 444;
					return v31;
				}
			}
		}
	}
	//printf("#616 Year = %d\n",Year);
	return -1; // Error;
}

void convdate(double a1, int *Day, int *Month, int *Year)
{
	int v4; // edx@3
	int v5; // eax@8
	int v6; // eax@9
	signed int v7; // ecx@12
	int v8; // ecx@12
	signed int v9; // eax@12
	signed int v10; // ecx@14

	*Day	= 0;
	*Month	= 0;
	*Year	= 0;

	if ( a1 > 0 )
	{
//-----------------------------------------------
// Work out the Century we are in.
// No Sanity checks beyond 2199!!

		if ( a1 <= 766644 )			// 31-12-2099
		{
			if ( a1 <= 693595 )		// 31-12-1899
			{
				v4 = a1 - 657072;	// 1-01-1800
				*Year = 1800;
			}
			else	// Year = 01-01-1900 to 31-12-2099
			{
				v4 = a1 - 693596;	// 1-01-1900
				*Year = 1900;
			}
		}
		else
		{
			v4 = a1 - 766645;		// 1-01-2100
			*Year = 2100;
		}
		//printf("convdate: Year = %d, v4 = %d\n",*Year, v4);
//-----------------------------------------------
		if ( v4 > 58 )
		{
			//printf("convdate: v4 = %d\n",v4);
			v7 = 4 * v4 - 233;
			*Year += v7 / 1461;
			//printf("convdate: v7/1461 = %d\n",(v7/1461));
			v8 = v7 % 1461;
			v9 = v8 + 7;
			if ( v8 + 4 > -1 )
				v9 = v8 + 4;
			v10 = 5 * (v9 >> 2) - 3;
			*Month = v10 / 153;
			*Day = (v10 % 153 + 5) / 5;
			if ( *Month > 9 )
			{
				*Month -= 9;
				++*Year;
			}
			else
			{
				*Month += 3;
			}	
		}
		else
		{
			//printf("convdate: v4 = %d\n",v4);
			v5 = (v4 > 30) + 1;
			*Month = v5;
			if ( v5 == 1 )
				v6 = v4 + 1;
			else
				v6 = v4 - 30;
			*Day = v6;
		}
	}
}

char* makedate(char *s, double DaysSinceZero)
{
	int Day; // [sp+2Ch] [bp-1Ch]@3
	int Month; // [sp+30h] [bp-18h]@3
	int Year; // [sp+34h] [bp-14h]@3

	//printf("makedate(%G) - _DF = %d\n",DaysSinceZero, _DF);

	if ( DaysSinceZero >= 657071.0 )	// 01-01-1800
	{
		_check_YD();
		Year	= 0;
		Month	= 0;
		Day		= 0;
		convdate(DaysSinceZero, &Day, &Month, &Year);

		if ( Year < fromYear || Year > toYear )	// show 4 digits of year
		{
			switch (_DF)
			{
				case 0:
					sprintf(s, "%2d-%2.2d-%4.4d", Day, Month, Year);
					break;
				case 1:
				    sprintf(s, "%2d/%2.2d/%4.4d", Month, Day, Year);
					break;
				default:	// _DF == 2, and all others
				    sprintf(s, "%4.4d-%2.2d-%2.2d", Year, Month, Day);
					break;
			}
		}
		else
		{
			Year %= 100;	// divide by 100, only show last two year digits
			switch (_DF)
			{
				case 0:
					sprintf(s, "%2d-%2.2d-%2.2d", Day, Month, Year);
					break;
				case 1:
				    sprintf(s, "%2d/%2.2d/%2.2d", Month, Day, Year);
					break;
				default:	// _DF == 2, and all others
				    sprintf(s, "%2.2d-%2.2d-%2.2d", Year, Month, Day);
					break;
			}
		}
	}
	else
	{
		*s = 0;
	}
	return s;
}

void daysarr(signed int a1, int *Date_array)
{
	int DayOfYear;
	int CurrMonth;
	int LeapYear;
	int Day;
	int Month;
	int Year;
	struct tm tp;

	memset(&tp, 0, sizeof(tp));

	convdate(a1, &Day, &Month, &Year);
	Date_array[0] = Day;						// Day of the Yonth
	Date_array[1] = Month;						// Month of the Year
	Date_array[2] = Year;						// the Year  4 digits
	
	LeapYear = 0;
	if ( !(Year & 3) && Year % 100 || !(Year % 400) )
		LeapYear = 1;
	
	DayOfYear = Day;	// Days in current month + days in prior months
	for ( CurrMonth = 1; CurrMonth < Month ; CurrMonth++ )
		DayOfYear += day_tab[CurrMonth + 13 * LeapYear];
  
	Date_array[3] = DayOfYear;				// Day of the Year
	Date_array[4] = a1 % 7;					// Day of the Week
	Date_array[5] = a1 - 693595;			// Days since 01-01-1900
	Date_array[6] = LeapYear;				// LeapYear? 1=Yes, 0=No
	Date_array[7] = 0;						// Seconds offset from UTC
	Date_array[8] = 0;						// Seconds offset from UTC when daylight saving
	Date_array[9] = 0;						// Daylight Saving? 1=Yes, 0=No
	// seems pointless blanking 7,8,9 above, as they are set below.

// last three vars introduced in v4.2?
	timezone	= 0;
	tp.tm_year	= Year - 1900;
	tp.tm_mon	= Month - 1;
	tp.tm_mday	= Day;
	tp.tm_hour	= 4;
	tp.tm_min	= 0;
	tp.tm_sec	= 0;
	tp.tm_isdst = -1;
	tp.tm_zone	= 0;
	mktime(&tp);
  
	Date_array[7]	= timezone;
	Date_array[8]	= timezone - 3600;
	Date_array[9]	= tp.tm_isdst;
}

int arrdays(int *Date_array)
{
	int MonthOfYear;
	int Year;
	int LeapYear;
	int v4;
	int v5;
	int v6;
	int v8;
	int DayOfYear;
	int CurrMonth;
	int v13;
	int DayOfMonth;

	//printf("arrdays(x%08X)\n",Date_array);

/*
	Date Array values.
	** CL4 uses 1-based Array sub-scripts!!  So Array is dates[1]-dates[10] **
	dates[0] = 11		day of the month
	dates[1] = 2		month of the year
	dates[2] = 1986		the year
	dates[3] = 42		day of the year
	dates[4] = 2		day of the week (0 for Sunday)
	dates[5] = 31453	days since 01-01-1900
	dates[6] = 0		leap year 1=yes, 0=no
	dates[7]			Seconds offset from UTC
	dates[8]			Seconds offset from UTC when daylight saving
	dates[9]			Daylight Saving? 1=Yes, 0=No
*/
/*
	int		DateArray[] = {28,8,1987,241,0,32382,0};
	real routine returns :
	
	arrdays(DateArray) returned 725611
	Larrdays(DateArray) returned 60095
	
	int		DateArray[] = {1,1,2000,0,0,0,0};
	
	arrdays(DateArray) returned 730120
	Larrdays(DateArray) returned 60464


*/
	DayOfMonth  = Date_array[0];		// day of month
	MonthOfYear = Date_array[1];		// Month
	Year		= Date_array[2];		// Year

	// adjust year based on month value passed in
	for ( ; MonthOfYear <= 0; MonthOfYear += 12 )	
		Year--;
	for ( ; MonthOfYear > 12; MonthOfYear -= 12 )
		Year++;

	LeapYear = 0;
	if ( !(Year & 3) && Year % 100 || !(Year % 400) )
		LeapYear = 1;
	// Range check the day of month value passed in
	v4 = MonthOfYear + 13 * LeapYear;
	if ( day_tab[v4] < DayOfMonth )
		DayOfMonth = day_tab[v4];
	
	Date_array[0] = DayOfMonth;
	Date_array[1] = MonthOfYear;
	Date_array[2] = Year;

	// calc number of days since 01-00-0000
	if ( MonthOfYear > 2 )
		v5 = 1461 * Year;
	else
		v5 = 1461 * (Year - 1);
	v6 = v5 + 3;
	if ( v5 > -1 )
		v6 = v5;
	v6 >>= 2;	// divide by 4

	if ( MonthOfYear > 2 )
		v8 = (153 * (MonthOfYear + 1)) / 5 + v6;
	else
		v8 = (153 * MonthOfYear + 1989) / 5 + v6;
	v13 = DayOfMonth + v8 - 443;
  
	// Don't need to re-calc LeapYear. Year hasn't changed from above!
	//LeapYear = 0;
	//if ( !(Year & 3) && Year % 100 || !(Year % 400) )
	//	LeapYear = 1;
  
	//Calc day in year
	DayOfYear = DayOfMonth;
	CurrMonth = 1;
	while ( CurrMonth < MonthOfYear )
	{
		DayOfYear += day_tab[CurrMonth + 13 * LeapYear];
		CurrMonth++;
	}

	Date_array[3] = DayOfYear;
	Date_array[4] = v13 % 7;			// day of week 0-6
	Date_array[5] = v13 - 693595;		// Days since 01-00-0000 - days since 01-01-1900 
	Date_array[6] = LeapYear;
	return v13;							// return Days since 01-00-0000
}

char* dfmt(char *s, char *format, double DaysSinceZero)
{
	char *v3;
	char *Dest;
	char *l;
	char *v30;
	const char *v7;
	const char *v13;
	signed int v8;
	unsigned int v11;
	unsigned int v15;
	int j;
	unsigned int v18;
	unsigned int v31;
	int v23;
	int v24;
	int v25;
	int v28;
	
	signed int State = 0;
	int YearChars = 0;
	int MonthChars = 0;
	int WeekChars = 0;
	int DayChars = 0;
	char v6;
	struct tm tp;
	double v37;

	//printf("dfmt(%s, %G)\n",format, DaysSinceZero);

	v3 = format;
	
	// Check valid year ranges   01-01-1800 <-> 31-12-2099
	v37 = DaysSinceZero;
	if ( DaysSinceZero <= 657071.0 || DaysSinceZero >= 766644.0 )     
		v37 = 0.0;
	
	if ( lenstr(format) <= 0 )
	{
		makedate(s, v37);
		return s;
	}
	
	daysarr(v37, darr);
	
	if ( *format != '%' )
	{
		Dest = s;
		// Simple state machine steered by State
		while ( 1 )
		{
			switch ( State )	// always \0 on first run through
			{
				case 0:	// next char handler
					v6 = *v3;
					printf("dfmt: #0 v6 = %c\n",v6);
					switch (v6)
					{
						case 0:			// we hit end of format string. Terminate and return
							*Dest = 0;
							return s;
							break;
						case 'm':
							MonthChars++;
							State = 4;
							v3++;
							break;
						case 'w':
							WeekChars++;
							State = 2;
							v3++;
							break;
						case 'y':
							YearChars++;
							State = 8;
							v3++;
							break;
						case 'd':
							DayChars++;
							State = 1;
							v3++;
							break;
						default:		// non-zero default handler
							if ( v6 != '_' && v37 != 0.0 )
							{
								if ( *v3 == '\\' && v3[1] && v3[1] != '\\' )
									++v3;
							}
							else
							{
								*v3 = ' ';
							}
							*Dest++ = *v3++;
							break;
					}
					break;
//-------------------------------------
				case 1:		// look for 'd'chars -  day handler
					if ( *v3 == 'd' )
					{
						DayChars++;
					}
					else
					{
						--v3;
						if ( v37 == 0.0 )
						{
							sprintf(Dest, "%*s", DayChars, " ");
						}
						else
						{
							if ( DayChars == 3 )
							{
								v28 = darr[3];			// 3 'd' digits = day of the year
							}
							else
							{
								if ( DayChars == 4 )	// day with alpha st, nd, rd, etc.
								{
									v8 = darr[0];		// day of month
									
									if ( v8 > 9 )
										sprintf(Dest, "%d", v8);
									else
										sprintf(Dest, " %d", v8);
									
									switch ( v8 )
									{
										case 1:
										case 21:
										case 31:
											Dest[2] = 's';
											Dest[3] = 't';
											Dest[4] = 0;
											break;
										case 2:
										case 22:
											Dest[2] = 'n';
											Dest[3] = 'd';
											Dest[4] = 0;
											break;
										case 3:
										case 23:
											Dest[2] = 'r';
											Dest[3] = 'd';
											Dest[4] = 0;
											break;
										default:
											Dest[2] = 't';
											Dest[3] = 'h';
											Dest[4] = 0;
											break;
									}
									Dest += DayChars;
									DayChars = 0;
									State = 0;
									++v3;
									break;
								}
	
								v11 = 1;
								for ( j = DayChars; j; --j )
									v11 *= 10;
								v28 = darr[0] % v11;
							}
							sprintf(Dest, "%0*ld", DayChars, v28);
						}
						Dest += DayChars;
						DayChars = 0;
						State = 0;
					}
					++v3;
					break;
//-------------------------------------
				case 2:		// days of the week 'w' handler
					if ( *v3 == 'w' )
					{
						WeekChars++;
					}
					else
					{
						--v3;
						if ( v37 == 0.0 )
						{
							sprintf(Dest, "%*s", WeekChars, " ");
						}
						else
						{
							sprintf(Dest, "%-*s", WeekChars, wday[darr[4]]);
						}
						Dest += WeekChars;
						WeekChars = 0;
						State = 0;
					}
					v3++;
					break;
//-------------------------------------
				case 4:	// month of the year handler 'm'
					if ( *v3 == 'm' )
					{
						MonthChars++;
					}
					else
					{
						--v3;
						if ( v37 == 0.0 )
						{
							sprintf(Dest, "%*s", MonthChars, " ");
						}
						else
						{
							if ( MonthChars <= 2 )
							{
								v15 = 1;
								for ( j = MonthChars; j; --j )
									v15 *= 10;
								sprintf(Dest, "%0*ld", MonthChars, (darr[1] % v15));
							}
							else
							{
								sprintf(Dest, "%-*s", MonthChars, fmth[darr[1]]);
							}
						}
						Dest += MonthChars;
						MonthChars = 0;
						State = 0;
					}
					v3++;
					break;
//-------------------------------------
				case 8:		// year handler 'y' 
					if ( *v3 == 'y')
					{
						YearChars++;
					}
					else
					{
						--v3;
						if ( v37 == 0.0 )
						{
							sprintf(Dest, "%*s", YearChars, " ");
						}
						else
						{
							v18 = 1;
							for ( j = YearChars; j; --j )
								v18 *= 10;
							v31 = darr[2] % v18;
							sprintf(Dest, "%0*ld", YearChars, v31);
						}
						Dest += YearChars;
						YearChars = 0;
						State = 0;
					}
					++v3;
					break;
//-------------------------------------
				default:	// bump to next char and continue
					++v3;
					break;
			}
		}
	}
	
	// format = '%', show time string instead!
	tp.tm_sec	= 0;
	tp.tm_min	= 0;
	tp.tm_hour	= 0;
	tp.tm_mday	= darr[0];			// Day
	tp.tm_mon	= darr[1] - 1;		// Month
	tp.tm_year	= darr[2] - 1900;	// Year
	tp.tm_wday	= darr[4];
	tp.tm_yday	= darr[3] - 1;
	tp.tm_isdst = darr[9];
	tp.tm_zone	= 0;
	strftime(s, 0x30u, format, &tp);	// Assume buffer s is 48 bytes long
	if ( v37 <= 0.0 )
	{
		for ( l = s; *l; ++l )
			*l = ' ';
	}
	return s;
}
#endif
