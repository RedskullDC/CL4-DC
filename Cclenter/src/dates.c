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

	//printf("_checkYD(): dateDone = %d, _YD = %s, defDateWidth = %d\n",dateDone, _YD, defDateWidth);
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

		v0 = strchr(_YD, ':');
		if ( v0 )
		{
			defDateWidth = strtol(v0 + 1, 0, 10);
			if ( (defDateWidth - 8) > 2 )
				defDateWidth = 10;
		}
		//printf("_checkYD(): fromYear = %d, toYear = %d, defDateWidth = %d\n", fromYear, toYear, defDateWidth);
		dateDone = 1;
	}
}

void loadMonthArrays(bool SetAvars)
{
	char	*v15;
	int		Day;
	int		Month;
	int		dtform;

	// see  <langinfo.h> for constants
	strcpy(THOU_SEP, nl_langinfo(THOUSEP));
	strcpy(RADIX_CHAR, nl_langinfo(RADIXCHAR));

	wday = mmalloc(sizeof(char*) * 7);	// x64 compat

	for (Day = 0; Day < 7 ; Day++)
	{
		wday[Day] = mstrcpy(nl_langinfo(Day + DAY_1), 0);
		if ( SetAvars )
		{
			setavar(v_DAYNAMES, nl_langinfo(Day + DAY_1), Day);
			setavar(v_ABDAYNAMES, nl_langinfo(Day + ABDAY_1), Day);
		}
	}

	fmth = mmalloc(sizeof(char*) * 13);
	mth	 = mmalloc(sizeof(char*) * 13);

	for (Month = 0; Month < 12 ; Month++ )
	{
		fmth[Month + 1] = mstrcpy(nl_langinfo(Month + MON_1), 0);
		mth[Month + 1]	= mstrcpy(nl_langinfo(Month + ABMON_1), 0);
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
		dtform = -1;
		while ( *v15 )
		{
			switch ( *v15 )	// Examine the system DateFormat string to see what comes first 
			{
				case 'D':
				case 'E':
				case 'd':
				case 'e':
					dtform = 0;	// DD-MM-YY[YY]
					break;
				case 'M':
				case 'm':
					dtform = 1;	// MM-DD-YY[YY]
					break;
				case 'Y':
				case 'y':
					dtform = 2;	// YYYY-MM-DD
					break;
				default:
					break;
			}
			v15++;
			if ( dtform >= 0 )
			{
				_DF = dtform;
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
                v2 = "Japanese\n";
				break;
			case 'K':
				v2 = "Korean\n";
				break;
			case 'T':
				v2 = "Thai\n";
				break;
			case 'C':
				v2 = "Chinese\n";
				break;
			default:
                v2 = "Unknown\n";
				break;
			}
            eprint(v2);
            //eprint("\n");
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
	struct tm *v1; // esi@1
	char *v2; // eax@1
	time_t v4; // [sp+14h] [bp-14h]@1
	time_t timer; // [sp+18h] [bp-10h]@1

	timer = 0;
	v4 = time(&timer);
	v1 = gmtime(&v4);
	v2 = nl_langinfo(D_FMT);		// 131113
	strftime(s, 20u, v2, v1);
	return s;
}

int clgetdate(const char *src)
{
	char *v4;
	char *v13;
	
	int v11;
	int v12;
	int v20;
	int v23;
	int v35;
	int v46;
	int LeapYear;
	int Day;
	int Year;
	int Month;
	int Fudge;			// Gregoria fudge factor
  
	bool allDigits;

	char s[30];			// src is copied here, max 29chars + \0
	char v51[32];
	char v52[32];		// sysdate string stored here
	char curDays[16];
	char curMonth[16];
	char curYear[16];
	char sYear[16];
	char sMonth[10];		// not sure of length
	char sDays[10];		// not sure of length  = Year value is read in here, assume 4 chars + \0

	char DateAlias;		// Single letter command

	//printf("clgetdate(%s), _DF = %d\n" ,src, _DF);

	memset(s, 0, sizeof(s));
	strncpy(s, src, 29u);		// avoid buffer overrun
	sysdate(v52);	//sysdate returns string like [23 Jan 2013]
	DateAlias = 0;

	if (strlen(s) == 1)
	{
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
		        cdbcpystr(s, v52, 0);	// Copy sysdate() to buffer if we found a command char match
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

//---------------
	v4 = v52;			// v52 may have received gmtdate() which has '/' chars embedded, strip them
	while ( *v4)
	{
		if ( !isalnum(*v4))
			*v4 = ' ';
		v4++;
	}
//---------------
	allDigits = true;	// See if the user only passed in digits with no spaces: ddmmyy[yy] etc.
	v4 = s;
	while ( *v4 )
	{
		if ( !isalnum(*v4))
			*v4 = ' ';
		if (!isdigit(*v4))
			allDigits = false;
		v4++;
	}
//---------------
	if ( allDigits )	// Enter this block is there was nothing but digits in the date string
	{
		if ( _DF == 2 && strlen(s) == 8)
		{
			v11 = 4;	// yyyymmdd
			v46 = 6;
		}
		else
		{
			v11 = 2;	// ddmmyy[yy] or mmddyy[yy] or yymmdd
			v46 = 4;
		}

		v12 = 1;
		v13 = s;
		v4 = v51;
		while ( *v13 )
		{
			*v4++ = *v13++;
			if ( v12 == v11 || v12 == v46 )
				*v4++ = ' ';	// insert a space
			v12++;
		}
		*v4 = 0;
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
	for (v4 = sYear; *v4 ; v4++)	// Check that Year value has only "digits"
	{
		if (!isdigit(*v4))
			return -1;	// error exit
	}

//---------------
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

	// =======================================================================
	
	// Extracted whatever we can. Convert to a "Days since 01-01-0001" Integer

	if ( (Year - 1801) <= 398u )	// Ensure year >= 1801 and <= 2199
	{
		Month = strtol(sMonth, 0, 10);	// should fail on linux system time calls, sMonth is Jan,Feb,etc.
		if ( Month <= 0 || Month > 12 )
		{								// look for a string month name
			sMonth[3] = 0;				// Limit sMonth to 3 chars, Jan,Feb,etc.
			Month = 0;
			v23 = 0;
			while ( !Month )
			{
				if ( ++v23 > 12 )
					return -1;	// Error exit;
				if ( !compare(sMonth, mth[v23]) ) // compare strings, ignore case
					Month = v23;
			}
		}

		Day = strtol(sDays, 0, 10);
		if ( Day > 0 )
		{
			LeapYear = 0;
			if ( !(Year & 3) && Year % 100 || !(Year % 400) )
				LeapYear = 1;
			
			if ( day_tab[Month + 13 * LeapYear] >= Day )	// check for sanity on Day value
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
					case 'N':			// tomorrow
					case 'n':
						Day++;			// If Day overflows month, no drama. gets added in below regardless
						break;
					case 'E':			// Last day of the month
					case 'e':
					case 'L':
					case 'l':
						Day = day_tab[Month + 13 * LeapYear];		// No need to re-calc LeapYear here, year didn't change.
						break;
					case 'F':			// First day of the month
					case 'f':
						Day = 1;
						break;
					default:
						break;			// default == do nothing if no command found
				}

//eprint("#500 Day = %d, Month = %d, Year = %d\n", Day, Month, Year);	
//--------------------------------------------------

/*
Need to fudge the days a bit here

Conversion from Julian to Gregorian dates

Gregorian range				Julian range			Difference

From 1 March 1800			From 18 February 1800
to 28 February 1900			to 16 February 1900		12 days

From 1 March 1900			From 17 February 1900
to 28 February 2100			to 15 February 2100		13 days

From 1 March 2100			From 16 February 2100
to 28 February 2200			to 14 February 2200		14 days
*/


				if ( Year > 2100 || (Year == 2100 && Month > 2) )
					Fudge = 444;	// gregorian leap year fudgery
				
				else if ( Year > 1900 || (Year == 1900 && Month > 2) )	// March 1900 to Feb 2100
					Fudge = 443;
				
				else	// < March 1900
					Fudge = 442;

				if ( Month > 2 )
					v35 = 1461 * Year;
				else
					v35 = 1461 * (Year - 1);
				
				//if ( v35 > -1 )
				//	v36 = v35;
				//else
				//	v36 = v35 + 3;	// unnecessary code to trap /0 error?. Year can't be zero, tested above.

				if (v35 < 0 )		// unnecessary code to trap /0 error?. Year can't be zero, tested above.
					v35 += 3;

				if ( Month > 2 )
					return Day + (153 * (Month + 1)) / 5 + (v35 >> 2) - Fudge;
				else
					return Day + (153 * (Month + 13)) / 5 + (v35 >> 2) - Fudge;
			}
		}
	}
	//printf("#616 Year = %d\n",Year);
	return -1; // Error;
}

void convdate(double DaysSinceZero, int *Day, int *Month, int *Year)
{
	int v4;
	int v7;
	int v8;
	int v10;

	// default to "can't work it out" values :o)
	*Day	= 0;
	*Month	= 0;
	*Year	= 0;

	if ( DaysSinceZero > 0 )
	{
//-----------------------------------------------
// Work out the Century we are in.
// No Sanity checks beyond 2199!!

/*
Gregorian range				Julian range			Difference

From 1 March 1800			From 18 February 1800	12 days
to 28 February 1900			to 16 February 1900	

From 1 March 1900			From 17 February 1900	13 days
to 28 February 2100			to 15 February 2100	

From 1 March 2100			From 16 February 2100	14 days
to 28 February 2200			to 14 February 2200	

*/
		if ( DaysSinceZero > 766644 )			// 31-12-2099
		{
			v4 = DaysSinceZero - 766645;		// 1-01-2100
			*Year = 2100;
		}
		else if ( DaysSinceZero <= 693595 )		// 31-12-1899
		{
			v4 = DaysSinceZero - 657072;	// 1-01-1800
			*Year = 1800;
		}
		else	// Year = 01-01-1900 to 31-12-2099
		{		// year%400 == 0 is a leap year.  Trumps year%100.  2000 was a leap year
			v4 = DaysSinceZero - 693596;	// 1-01-1900
			*Year = 1900;
		}
		//printf("convdate: Year = %d, v4 = %d\n",*Year, v4);

//-----------------------------------------------

		if ( v4 > 58 )	// Normal handler
		{
			v4 -= 58;						// Now v4 = days from 01-March 1800,1900,2100 = Day#0
			//v7 = 4 * v4 - 233;			// 233 = 58*4 + 1
			v7 = 4 * v4 - 1;				// 3, 7, 11, 15, 19, 23 etc.		

			*Year += v7 / 1461;				// == 365.25 * 4 -> extra day each 4 years
			
			v8 = v7 % 1461;					// v8 = Day of the year from Mar 1st (== 0)
											// == 1460 on 29th Feb only

			if ( v8 + 4 > -1 )	// should always be true? v8 is always +ve
				v8 += 4;					// v8adj = 7, 11, 15, 19, 23 , 27, 31 etc.
			else
				v8 += 7;
			
			v10 = 5 * (v8 >> 2) - 3;		// v8 >> 2 = 1, 2, 3, 4 , 5 , 6 , 7 --  366  (29th FEB)
											// v10     = 2, 7, 12, 17, 22, 27, 32 etc.
											// 31-03 = 152, 30-04 = 302, 31-05 = 457, 30-06 = 607, 31-07 = 762 (765 = 153*5)
											// 01-04 = 157, 01-05 = 307, 01-06 = 462, 
			//printf("convdate: v8adj = %3d, v8 >>2 = %3d, v10 = %3d\n",v8, v8 >>2, v10);

			*Month = v10 / 153;				//  31  30  31  30  31
											// MAR-APR-MAY-JUN-JUL = 153 Days
											// AUG-SEP-OCT-NOV-DEC = 153 Days :o)
											// MAR = Month 0 at this point.
			
			//printf("convdate: Month = %d\n",*Month);

			*Day = (v10 % 153 + 5) / 5;
			//printf("convdate: Day = %d\n",*Day);

			if ( *Month > 9 )				// Jan/Feb of the following year
			{
				*Month -= 9;
				++*Year;
			}
			else
			{
				*Month += 3;				// Adjust so that Jan == Month#1
			}	
		}
		else	// We are in Jan/Feb at the turn of the century. == no leap day! Gregorian fudges!
		{		// year%100 == 0, not a leap year
			if (v4 > 30)
			{
				*Month = 2;
				*Day = v4 - 30;
			}
			else
			{
				*Month = 1;
				*Day = v4 + 1;
			}
		}
	}
}

char* makedate(char *s, double DaysSinceZero)
{
	int Day;
	int Month;
	int Year;

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

void daysarr(signed int DaysSinceZero, int *Date_array)
{
	int DayOfYear;
	int CurrMonth;
	int LeapYear;
	int Day;
	int Month;
	int Year;
	struct tm tp;

	memset(&tp, 0, sizeof(tp));

	convdate((double)DaysSinceZero, &Day, &Month, &Year);
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
	Date_array[4] = DaysSinceZero % 7;		// Day of the Week
	Date_array[5] = DaysSinceZero - 693595;	// Days since 01-01-1900
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
		v5 = 1461 * Year;		// 1461 = 365.25 * 4 == Days in 4 years
	else
		v5 = 1461 * (Year - 1);
	
	v6 = v5 + 3;
	if ( v5 > -1 )
		v6 = v5;
	v6 >>= 2;	// divide by 4

	if ( MonthOfYear > 2 )
		v8 = (153 * (MonthOfYear + 1)) / 5 + v6;
	else
		v8 = (153 * (MonthOfYear + 13)) / 5 + v6;		// 1989 = 153*13
	
	v13 = DayOfMonth + v8 - 443;
  
	// Don't need to re-calc LeapYear. Year hasn't changed from above!
	//LeapYear = 0;
	//if ( !(Year & 3) && Year % 100 || !(Year % 400) )
	//	LeapYear = 1;
  
	//Calc day in year
	DayOfYear = DayOfMonth;
	for ( CurrMonth = 1; CurrMonth < MonthOfYear ;CurrMonth++)
		DayOfYear += day_tab[CurrMonth + 13 * LeapYear];

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
	int j;
	
	signed int State = 0;
	int YearChars = 0;
	int MonthChars = 0;
	int WeekChars = 0;
	int DayChars = 0;
	char v6;
	struct tm tp;

	//printf("dfmt(%s, %G)\n",format, DaysSinceZero);

	v3 = format;
	
	//                             657072         766643
	// Check valid year ranges   01-01-1800 <-> 31-12-2099
	if ( DaysSinceZero < 657072 || DaysSinceZero > 766644.0 )  // 766644 is actually 31-Dec-2099
		DaysSinceZero = 0.0;

	if ( lenstr(format) <= 0 )
	{
		makedate(s, DaysSinceZero);
		return s;
	}
	
	daysarr(DaysSinceZero, darr);
	
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
					//printf("dfmt: #0 v6 = %c\n",v6);
					switch (v6)
					{
						case 0:			// we hit end of format string. Terminate and return
							*Dest = 0;
							return s;
							break;
						case 'd':
							DayChars++;
							State = 1;
							v3++;
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
						default:		// non-zero default handler
							if ( v6 != '_' && DaysSinceZero != 0.0 )
							{
								if ( *v3 == '\\' && v3[1] && v3[1] != '\\' )
									v3++;
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
						DayChars++;
					else
					{
						--v3;
						if ( DaysSinceZero == 0.0 )
							sprintf(Dest, "%*s", DayChars, " ");

						else if ( DayChars == 4 )	// day with alpha st, nd, rd, etc.
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
						}
						else
						{
							long v28;
							if ( DayChars == 3 )
							{
								v28 = darr[3];			// 3 'd' digits = day of the year **undocumented**
							}
							else
							{
								int v11 = 1;
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
					v3++;
					break;
//-------------------------------------
				case 2:		// days of the week 'w' handler
					if ( *v3 == 'w' )
						WeekChars++;
					else
					{
						--v3;
						if ( DaysSinceZero == 0.0 )
							sprintf(Dest, "%*s", WeekChars, " ");
						else
							sprintf(Dest, "%-*s", WeekChars, wday[darr[4]]);
						
						Dest += WeekChars;
						WeekChars = 0;
						State = 0;
					}
					v3++;
					break;
//-------------------------------------
				case 4:	// month of the year handler 'm'
					if ( *v3 == 'm' )
						MonthChars++;
					else
					{
						--v3;
						if ( DaysSinceZero == 0.0 )
							sprintf(Dest, "%*s", MonthChars, " ");
						else
						{
							if ( MonthChars <= 2 )
							{
								int v15 = 1;
								for ( j = MonthChars; j; --j )
									v15 *= 10;
								sprintf(Dest, "%0*ld", MonthChars, (long)(darr[1] % v15));
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
						YearChars++;
					else
					{
						--v3;
						if ( DaysSinceZero == 0.0 )
							sprintf(Dest, "%*s", YearChars, " ");
						else
						{
							int v18 = 1;
							for ( j = YearChars; j; --j )
								v18 *= 10;
							sprintf(Dest, "%0*ld", YearChars, (long)(darr[2] % v18));
						}
						Dest += YearChars;
						YearChars = 0;
						State = 0;
					}
					v3++;
					break;
//-------------------------------------
				default:	// bump to next char and continue
					v3++;
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
	if ( DaysSinceZero <= 0.0 )
	{
		for ( l = s; *l; ++l )
			*l = ' ';
	}
	return s;
}
#endif
