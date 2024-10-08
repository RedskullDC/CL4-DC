#ifndef SUTIMES_C
#define SUTIMES_C

#include <stdlib.h>		// for mkstemp()
#include <unistd.h>		// for unlink()
#include <stdarg.h>		// for var args stuff
#include <string.h>
#include <errno.h>
#include <sys/times.h>	// for times()
#include "DBdefs.h"
#include "cl4.h"

int		rstime	= 0;
int		retime	= 0;
bool	sustart = false;

struct tms	tme;											// Time Start
struct tms	tms;											// Time End

void sutimes(char *Str, bool StartStop)
{
	double	Seconds;
	int		v5;
	int		v6;
	int		Minutes;
	int		Ticks_Per_Sec;

	if ( StartStop )										// Start counter
	{
		sustart = true;
		rstime = times(&tms);								// tms = time at start
	}
	else
	{
		if ( sustart )										// Been started already?
		{
			Ticks_Per_Sec = sysconf(_SC_CLK_TCK);			// ticks per second : Method is obsolete!
			retime	= times(&tme);							// tme = time at end
			Minutes = ((retime - rstime) / Ticks_Per_Sec) / 60;
			Seconds = ((double)(retime - rstime) / (double)Ticks_Per_Sec) - (60 * Minutes);

			v5 = tme.tms_cutime + tme.tms_utime - tms.tms_utime - tms.tms_cutime;
			v6 = tme.tms_cstime + tme.tms_stime - tms.tms_stime - tms.tms_cstime;
			if ( Str && *Str )
				eprint("%-12.12s ", Str);
			eprint("%.2fu %.2fs %d:%05.2f", ((double)v5 / (double)Ticks_Per_Sec), ((double)v6 /(double)Ticks_Per_Sec), Minutes, Seconds);

			//RedskullDC   0.00u 0.00s 0:04.01 0.0%
			if ( retime != rstime )
				eprint(" %.1f%%", (v6 + v5) * 100.0 / (retime - rstime));
			eprint("\n");
		}
		else
		{
			eprint("no start times\n");
		}
		
	}
}


#endif

