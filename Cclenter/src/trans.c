#ifndef LOGGING_C
#define LOGGING_C

#include <string.h>
#include "DBdefs.h"
#include "cl4.h"

// libcl4 versions!!!


void startran();
void endtran();

void startran(int Context)
{
  short trantype_sav; // di@1

  trantype_sav		= tl.TransType;
  tl.PID			= Context;
  tl.PID			= getpid();
  tl.field_C		= Context + 1000 * lpid;
  time(&tl.TimeT);
  tl.RecSize		= 0;
  tl.TransType		= 1;
  tl.Context		= Context;
  wrlog(Context, 0, 0, 1);
  tl.TransType		= trantype_sav;
  tx_started[Context - 10000] = 1;
}

void endtran(int Context)
{
	if ( lfd[Context - 10000] >= 0 )
	{
		if ( tx_started[Context - 10000] )
		{
			tl.PID = getpid();
			tl.field_C = Context + 1000 * lpid;
			tl.Context = prev_tx[Context - 10000];
			time(&tl.TimeT);
			tl.RecSize = 0;
			tl.TransType = 2;
			wrlog(Context, 0, 0, 1);
			++cl_tx;
			prev_tx = 0;
			tx_started  = 0;
		}
	}
}

#endif



