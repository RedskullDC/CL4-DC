#ifndef RTOKEY_C
#define RTOKEY_C

#include "DBdefs.h"
#include "cl4.h"

int rtokey(char *KeyBuf, char *Src, POS *Pos, TDef *TDefP)
{
	TDef	*tdef;
	char	*Dest;
	size_t	v6;
	short	TDFtype;
	short	v7;
	//short	v10;
	short	v11;

	int		localsize;
	char	local[128];	// for testing

	//printf("rtokey(KeyBuf: x%08X,Src: x%08X,POS: x%08X, TDef: x%08X)\n" ,KeyBuf,Src,Pos,TDefP);

	tdef = TDefP;
	Dest = KeyBuf;
	if ( TDefP->TDFentry )
	{
		for ( TDFtype = TDefP->TDFtype; TDFtype & tdf_KEY; TDFtype = tdef->TDFtype )	// & 1 == Keyfield.
		{
			if ( Pos )	// Has a re-position record been supplied?
			{
				v7 = cdbrindex(Pos, tdef->TDFentry);
				if ( v7 >= 0 )
				{	
					if ( tdef->TDFtype & tdf_NOCASE )  // Allow case-insensitive matches
					{
						if ( tdef->TDFtype & tdf_STRING )
							v11 = _fwidth(&Src[v7], tdef->TDFsize, tdef->TDFtype);
						else
							v11 = tdef->TDFsize;
						Dest += cpytuc(Dest, &Src[v7], v11);	// force to upper case
					}
					else
						Dest += xtodom(Dest, tdef->TDFsize, tdef->TDFtype, &Src[v7]);
				}
				else
					Dest += cdbfclear(Dest, tdef->TDFsize, tdef->TDFtype);	// No Pos record for this field. Just blank mem area

			}
			else	// No Pos specified, just place in buffer at normal position.
			{
				if ( TDFtype & tdf_STRING )				// String field variable
					v6 = (size_t)_fwidth(Src, tdef->TDFsize, TDFtype);
				else
					v6 = tdef->TDFsize;

				Dest += cpybuf(Dest, Src, v6);
				Src += tdef->TDFsize;
			}
			tdef++;
			if ( !tdef->TDFentry )
				break;
		}
	}
	return Dest - KeyBuf;
}

#endif
