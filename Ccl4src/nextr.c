#ifndef NEXTREC_C
#define NEXTREC_C

#include "DBdefs.h"
#include "cl4.h"
NODE_1* _stepfwd(TDinfo *TDptr);
char* DumpShortBits(short Input, char Output[20]);

int _nxtr(int TDNo, char *WorkArea, int SkipRecs, POS *Pos, EXPR *SelectExpr)
{
	TDinfo *TDptr;
	int RecsFound;
	NODE_1 *N1p;
	//char Buff[20];

	//printf("_nxtr( TDno: x%04X (%3d),WorkArea = %s,SkipRecs = %d, POS = x%08X, EXPR: x%08X)\n", TDNo,TDNo,WorkArea,SkipRecs,Pos,SelectExpr);

	TDptr = _chktd(TDNo);
	RecsFound = check_bup_sem(TDptr->TDDBinfo);	// dual use of RecsFound variable 
	if ( RecsFound )
		derror(RecsFound, 0, TDptr);
	
	RecsFound = 0;	// not really necessary, since above check means RecsFound = ZERO anyway

	//printf("_nxtr: TDFlags: x%04X ( %s )\n",TDptr->TDFlags,DumpShortBits(TDptr->TDFlags,Buff));
	
	if ( (TDptr->TDFlags & 0x0210) != 0x0010 )
	{
		if ( TDptr->TDFlags & 0x0200 )
		{
			relnode(TDptr->TDNodePtr);
			TDptr->TDNodePtr = 0;
			TDptr->TDFlags &= 0xFDFFu;			// xxxx xx0x xxxx xxxx   clear 0200
		}
		
		if ( TDptr->TDFlags & 0x0002 )							// This is only true for a temp table?
			return _picknxt(TDptr, WorkArea, SkipRecs, Pos);
    
		if ( !TDptr->KeyBuf1 )
			newseq(TDptr);		// allocate mem for TDptr->KeyBuf1 and TDptr->KeyBuf2
		
		if ( Pos )
			cdbrindex(Pos, 0);	// pointless code?

		TDptr->Key2Size = 0;
		while ( SkipRecs > RecsFound )
		{
			N1p = _stepfwd(TDptr);
			if ( N1p )
			{
				if ( _match(N1p->Data, SelectExpr, TDptr->TableDefs) )
				{
					if ( !TDptr->Key2Size )
					{
						TDptr->Key2Size = cpybuf(TDptr->KeyBuf2, N1p->Data, TDptr->TDKeySize);
						TDptr->field_42 = 1;
					}
					if ( SkipRecs == 1 ) // if not skipping records, save to workarea
						tuptor(WorkArea, Pos, N1p->Data, TDptr->TableDefs);
					
					RecsFound++;
				}
				else if ( _beyond(SelectExpr, 0x18u, N1p->Data, TDptr->TableDefs)) // 0x18 == "Less than or equal"
				{
					TDptr->TDFlags |= 0x0010u;		// there are more records left, but they don't satisfy our expression!
					break;
				}
			}
			else
			{
				TDptr->TDFlags |= 0x0010u;			// No more/no valid records beyond current position
				break;		//exit, no more valid records
			}
			// didn't find a match, but keep looking
		}
		if ( !RecsFound )
			relseq(TDptr);
	}
	return RecsFound;	// Recs Skipped or read
}

// HACK! These should call through xenter()

int nxtr(int TDNo, char *WorkArea, POS *Pos, EXPR *Expr)
{
  return _nxtr(TDNo, WorkArea, 1, Pos, Expr);
}

// same as nxtr()
int nextr(int TDNo, char *WorkArea, POS *Pos, EXPR *Expr)
{
    return _nxtr(TDNo, WorkArea, 1, Pos, Expr);
}

// Pos = 0.  Assume no data re-positioning in output buffer
int getnxt(int TDno, char *WorkArea, EXPR *Expression)
{
  return _nxtr(TDno, WorkArea, 1, 0, Expression);
}

// nextset allows SkipRecs value to be specified
int nextset(int TDno, char *WorkArea, int SkipRecs, POS *Pos, EXPR *SelectExpr)
{
  return _nxtr(TDno, WorkArea, SkipRecs, Pos, SelectExpr);
}

#endif
