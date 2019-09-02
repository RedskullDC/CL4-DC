#ifndef GETYX_C
#define GETYX_C

#include "DBdefs.h"
#include "cl4.h"

void clgetyx(XTAB *xt, short *Column, short *Line, int ArgType)
{
	// clgetyx assumes that xtab variable is valid, unlike almost every other routine.
	// Not very OOP using that nasty global variable var_cr!!

    if ( ArgType == yx_answer )     // #1  return the *answer* values. yx_answer = 1, 
    {
        if ( xt->ACol_exp )			// Expression trumps fixed value
        {
            *Column = evalint(&enarr.TableAddr[xt->ACol_exp - 1]);
            var_cr = true;
        }
        else
            *Column = xt->ACol;		// Answer Column

		if ( xt->ALine_exp )
        {
            *Line = evalint(&enarr.TableAddr[xt->ALine_exp - 1]);
            var_cr = true;
        }
		else
			*Line = xt->ALine;		// Answer Line
    }
    else                          // else return the Prompt values.		 yx_prompt = 2
    {
        if ( xt->PCol_exp )
        {
            *Column = evalint(&enarr.TableAddr[xt->PCol_exp - 1]);
            var_cr = true;
        }
        else
            *Column = xt->PCol;	// Prompt Column
        
		if ( xt->PLine_exp )
        {
            *Line = evalint(&enarr.TableAddr[xt->PLine_exp - 1]);
            var_cr = true;
        }
		else
			*Line = xt->PLine;	// Prompt Line
    }
	return;
}


#endif

