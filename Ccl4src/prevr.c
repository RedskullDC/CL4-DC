#ifndef PREVREC_C
#define PREVREC_C

#include "DBdefs.h"
#include "cl4.h"

//*not* public in libcl4

int _prevr(int TDNo, char *WorkArea,short SkipRecs, POS *Pos, EXPR *SelectExpr)
{
	TDinfo *TDptr;
	int RecsFound;
	NODE_1 *N1p;

	//printf("_prevr( TDno: x%04X (%3d),WorkArea = %s,SkipRecs = %d, POS = x%08X, EXPR: x%08X)\n", TDNo,TDNo,WorkArea,SkipRecs,Pos,SelectExpr);

	TDptr = _chktd(TDNo);
	if ( RecsFound = check_bup_sem(TDptr->TDDBinfo))	// dual use of RecsFound variable 
		derror(RecsFound, 0, TDptr);

	RecsFound = 0;	// not really necessary, since above check means RecsFound = ZERO anyway
	
	if ( (TDptr->TDFlags & 0x0210) != 0x0210 )
	{
		if ( !(TDptr->TDFlags & 0x0200) )
		{
			relnode(TDptr->TDNodePtr);
			TDptr->TDNodePtr = 0;
			TDptr->TDFlags |= 0x0200u;
		}
		if ( TDptr->TDFlags & 0x0002 )					// Only true for temp tables?
			return _pickprev(TDptr, WorkArea, SkipRecs, Pos);
		
		if ( !TDptr->KeyBuf1 )
			newseq(TDptr);			// allocate mem for TDptr->KeyBuf1 and TDptr->KeyBuf2
		
		if ( Pos )
			cdbrindex(Pos, 0);		// Pointless code?
		
		TDptr->Key1Size = 0;
		while ( SkipRecs > RecsFound )
		{
			N1p = _stepbak(TDptr);
			if ( N1p )
			{
				if ( _match(N1p->Data, SelectExpr, TDptr->TableDefs) )
				{
					if ( !TDptr->Key1Size )
					{
						TDptr->Key1Size = cpybuf(TDptr->KeyBuf1, N1p->Data, TDptr->TDKeySize);
						TDptr->field_3A = 2;
					}
					//if ( SkipRecs == 1 ) // _nxtr() only copies data if *not* skipping records 
					tuptor(WorkArea, Pos, N1p->Data, TDptr->TableDefs);	// copy data to record area
					RecsFound++;
				}
				else if ( _beyond(SelectExpr, 0x30u, N1p->Data, TDptr->TableDefs))	// 0x30 == greater than or equal	'>='
				{
					TDptr->TDFlags |= 0x0010u;		// there are more records left, but they don't satisfy our expression!
					break;
				}
			}
			else
			{
				TDptr->TDFlags |= 0x0010u;			// No more/no valid records beyond current position
				break;
			}
			// didn't find a match, but keep looking
		}
		if ( !RecsFound )
			relseq(TDptr);
	}
	return RecsFound;
}

// HACK! These should call through xenter()

int prevr(int TDno, char *WorkArea, POS *Pos, EXPR *Expr)
{
  return _prevr(TDno, WorkArea, 1, Pos, Expr);
}

int getprev(int TDno, char *WorkArea, EXPR *Expr)
{
  return _prevr(TDno, WorkArea, 1, 0, Expr);
}

int prevset(int TDno, char *WorkArea, int SkipRecs, POS *Pos, EXPR *Expr)
{
  return _prevr(TDno, WorkArea, SkipRecs, Pos, Expr);
}


#endif
