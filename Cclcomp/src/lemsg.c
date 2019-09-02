#ifndef LEMSG_C
#define LEMSG_C

bool lemsg(PTAB *pt)
{
     ENTAB	*ent;
	 short	v1;
	 short	v3;
	 short	ENTno;
	 
     symbol = getsym();
     v1 = loadexp(0, 1);
     if ( v1 )
     {
          v3 = loadexp(0, 1);
          if ( v3 )
          {
               ENTno		= getenmem();
               ent			= ENARR(ENTno);
               ent->enleft	= v1;
               ent->enright	= v3;
               pt->TABno	= ENTno;
               return true;				// exit success
          }
     }
     return false;	// error_exit
}

#endif
