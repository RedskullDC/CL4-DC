#ifndef LOCKREC_C
#define LOCKREC_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"                 // for bit field values

// UpdateMode:
// #define	upd_delete		0	// delete record
// #define	upd_put			1	// put record
// #define	upd_lock		2	// lock record	( create a lock record in TD2 )

int _lockr(DBinfo *DBptr, short IndexOff, char *KeyDefs, unsigned int KeySize, short UpdateMode)
{
	short v6; // ax@3
	//pid_t PID; // eax@7
	TDinfo *TD2; // eax@7
	TD2REC Dest; // [sp+30h] [bp-A8h]@5

	//printf("_lockr(DBptr: x%08X,Idx: x%04X, KeySize = %d, UpdateMode = %d)\n" ,DBptr,IndexOff, KeySize,UpdateMode);

	if ( DBptr->DBvflag > 3 )                     // Only V4 DBases have a locks table
	{
		v6 = check_bup_sem(DBptr);                  // Check BitMap update??
		if ( v6 )
			derror(v6, DBptr, 0);
		
		Dest.TableIndex = IndexOff;                 // 1 byte table index offset
		
		fill(Dest.KeyBuff, sizeof(Dest.KeyBuff), 0);
		if ( KeySize > sizeof(Dest.KeyBuff) )
			KeySize = sizeof(Dest.KeyBuff);
		
		cpybuf(Dest.KeyBuff, KeyDefs, KeySize);
		
		//PID = getpid();
		//ltoms(&Dest.PID, PID);
		
		ltoms(&Dest.PID, getpid());

		//DumpBlock((char*)&Dest, sizeof(Dest));
		
		TD2 = _chktd(td_locks);		// TD2 = locks table (2)
		TD2->TDDBinfo = DBptr;		// insert current DB

		return _update(TD2,(char*)&Dest, 0, UpdateMode, 0);
	}
	else
		return 0;
}

#endif
