#ifndef DBSPAC_C
#define DBSPAC_C

#include "DBdefs.h"
#include "cl4.h"

// Return free space in pages
int dbspace(int DB_no)
{
	DBinfo *DBptr;
	int *i; 
	int Space;
	int FreePages;	// Pages marked as Free in volume bitmap
	int ListPages;	// Pages currently marked as used, but sitting in Lists 

	int Grow;		// Dummy, not used.
	int UsedPages; 

	//printf("dbspace(DBno = %d)\n",DB_no);

	Space = 0;
	DBptr = _chkdb(DB_no);
	while ( DBptr )	// DBases can chain. Haven't actually seen this in practice.
	{
		_usage(DBptr, &UsedPages, &Grow);

		ListPages = 0;										// inlined nextra()
		FreePages = DBptr->DBmaxpages - UsedPages;
		for ( i = DBptr->SpareList ; *i; ListPages++ )
			i++;
		for ( i = DBptr->PageList1 ; *i; ListPages++ )
			i++;
		DBptr = DBptr->DBnextDB;
		Space += FreePages + ListPages - 17;				// fixed number of 17 pages for system tables/bitmap etc.
															// end nextra()
	}
	//printf("dbspace(%d) == %d\n",DB_no,Space);
	return (Space >= 0) ? Space : 0;
}

/* not called in clenter
bool _tblspace(int TDno, int *PagesFree)
{
	int v2;
	DBinfo *DBptr;
	TDinfo *TDptr;
	int MarkedPages;
	int *i;
	int v9;
	int UsedPages;
	int Grow;

	//printf("_tblspace(TDno = %d,PagesFree x%08X)\n",TDno, PagesFree);

	TDptr = _chktd(TDno);
	DBptr = TDptr->TDDBinfo;
	v2 = check_bup_sem(TDptr->TDDBinfo);
	if ( v2 )
		derror(v2, DBptr, 0);
	_usage(DBptr, &UsedPages,&Grow);	// Size of DB before "Grow" required 
	MarkedPages = 0;                                                
	
	for ( i = DBptr->SpareList ; *i; MarkedPages++ )
		i++;
	
	for ( i = DBptr->PageList1 ; *i; MarkedPages++ )
		i++;
	
	v9 = DBptr->DBmaxpages - UsedPages + MarkedPages - 17;
	*PagesFree = v9;	// no sanity check on ptr
	return v9 > 0;
}
*/
#endif
