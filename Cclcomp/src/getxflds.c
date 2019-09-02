#ifndef XFIELDS_C
#define XFIELDS_C

int getxflds(KXTAB *kxtab)		// get data fields for a create table statement
{
    FLDdesc *fld;
    int		*TTlist;
    short	*ElemIDptr;
    short	*DataIDptr;

    int		v7;
    int		TTno;
    
	short	FieldNo;
    short	count;

	char	FieldType;			// placeholder for getident(), not used.

    DataIDptr = &kxtab->DataElemIDs[kxtab->NumDataFields]; // Some Key elems may have been added to the Datalist already!

	while ( symbol == 2230 )                      // string literal
    {
		// sym is a TableName at this point . getident updates symbol for next loop
		FieldNo = getident(sym, &TTno, &FieldType);	// 
		if ( FieldNo < 0 )			// result >= zero == a FieldNo, < zero == a TableNo, -1 == ERROR!
		{
			if ( FieldNo == -1 )		// error not found!
				return -1;				// error_exit

			TTno = -FieldNo;			// FieldNo held a TTno

			TTlist = kxtab->TTlist;
			while (*TTlist && (*TTlist != TTno))	// Look to see if this TT is already in the list.
				TTlist++;
					
			if (!*TTlist)				// end of list reached, not found. add it to the list
			{							// *** would be nice if we could have *normal* variables in the list.....??
				*TTlist = TTno;
				++kxtab->NumTTs;       // Number of TT's referenced
			}

			fld = ttab[TTno].TTfields;
			while ( fld->FLDelemID )
			{
				if ( (v7 = fld->TDFtype & 0x3000, v7 != 0x2000) && v7 != 0x3000 )	// ignore S and K types
				{
					ElemIDptr = kxtab->KeyElemIDs;	// see if this element has already been indexed
					count = 0;
					while ( count < kxtab->NumKeyFields && *ElemIDptr != fld->FLDelemID) // if *ElemIDptr == fld->FLDelemID , already in our keylist, just continue
					{
						count++;
						ElemIDptr++;
					}
					if ( *ElemIDptr != fld->FLDelemID )	// Doesn't exist in our Keylist, check for it in the Datalist
					{
						ElemIDptr = kxtab->DataElemIDs;
						count = 0;
						while ( count < kxtab->NumDataFields && *ElemIDptr != fld->FLDelemID )
						{
							count++;
							ElemIDptr++;
						}
						if ( *ElemIDptr != fld->FLDelemID )	// Not already in our datalist
						{
							*DataIDptr++ = fld->FLDelemID;	// so add it to our DataFields list.
							++kxtab->NumDataFields;
							kxtab->RecSize += fld->FLDlen;
						}
					}
				}
				fld++;
			}
		}
		else 	// FieldNo >= 0 . Look for a single Field in a Table
		{
			TTlist = kxtab->TTlist;
			while (*TTlist && (*TTlist != TTno))	// Look to see if this TT is already in the list.
				TTlist++;
					
			if (!*TTlist)				// end of list reached, not found. add it to the list
			{							// *** would be nice if we could have *normal* variables in the list.....??
				*TTlist = TTno;
				++kxtab->NumTTs;		// Number of TT's referenced
			}

			fld = &ttab[TTno].TTfields[FieldNo];
			*DataIDptr++ = fld->FLDelemID;			// Add the elementID to our Datalist. **doesn't check for duplicates!!**
			++kxtab->NumDataFields;
			kxtab->RecSize += fld->FLDlen;
		}
	}
    *DataIDptr = 0;	// terminate the list
    return 0;		// exit_success!
}


#endif
