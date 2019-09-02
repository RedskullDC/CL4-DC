#ifndef LEINCLUDE_C
#define LEINCLUDE_C
#include "DBdefs.h"
#include "cl4.h"

bool leincl(void)
{
	char	*IncFileName;
	INCLUDE *incl;
	FILE	*InclFP;
	
	symbol = getsym();			// filename
	if ( symbol == 2220 )		// String literal filename - quoted string
    {
        IncFileName = mstrcpy(chkpath(sym, incl_dir, "CLIPATH", 0, 0x100u), 0);	// 0x100 = read only access
        InclFP = fopen64(IncFileName, "r");
        if ( InclFP )
        {
			gotonl();
			incl = (INCLUDE *)mmalloc(sizeof(INCLUDE));
			incl->FileName = mstrcpy(IncFileName, 0);
			FileTree->childINC = incl;
			FileTree->LastLine = ll;
			ll = -1;
			--lla;
			incl->parentINC = FileTree;		// set linkage so we can "find our way back"
			FileTree = incl;
			MainFile = InclFP;				// MainFile is always the *currently active* file
			incl->filePTR = InclFP;
			return false;					// return 0 = success.
        }
		loaderr(72, sym);		// "can't open include file"
     }
     else
		loaderr(71, sym);		// "invalid 'include' filename"

	 return true;				// error exit
}

#endif
