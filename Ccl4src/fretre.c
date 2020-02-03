#ifndef FRETRE_C
#define FRETRE_C

#include <stdio.h>
#include "DBdefs.h"
#include "cl4.h"

EXPR* fretre(EXPR *expr, EXPR *a2)
{

  if ( expr != a2 && expr )
  {
    if ( expr->NextEXPR )
      fretre(expr->NextEXPR, a2);
    
    if ( expr->PrevEXPR )
      fretre(expr->PrevEXPR, a2);

	nfree(expr, 0LL);
  }
  return a2;
}

#endif
