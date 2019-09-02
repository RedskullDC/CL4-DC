#ifndef KFIELDS_C
#define KFIELDS_C

int getkflds(KXTAB *kxtab)		// get key fields for a create table statement
{
	FLDdesc *fld;
	int		*TTlist;
	short	*ElemIDptr;
	short	*DataIDptr;
	short	*SortORDptr;
	short	*KeyIDptr;
	
    int		TTno;
	int		v8;
	short	FieldNo;
	short	count;
	unsigned short v15;
	 
    bool	first_keys;
	char	FieldType;

    fflush(stdout); // why is this here ????
    
    KeyIDptr	= &kxtab->KeyElemIDs[kxtab->NumKeyFields];		// kxtab->NumKeyFields normally ZERO. Suggests we can call routine again to add extra keys :o) 
    SortORDptr	= &kxtab->TkeyTy[kxtab->NumKeyFields];			// specifies any fields which have a descending sort
    DataIDptr	= &kxtab->DataElemIDs[kxtab->NumDataFields];	// normally ZERO. Same as above
     
	while ( symbol == 2230 )            // 2230 = possible variable/table name. pre-tested for sanity. (starts with alpha, then alphanum)
    {
		// sym is a TableName at this point . getident updates symbol for next loop
		FieldNo = getident(sym, &TTno, &FieldType);  // result >= zero == a FieldNo. < zero == a TableNo. -1 == ERROR!
		
		if ( FieldNo == -1 )			// -1 means sym wasn't located. ERROR! Will be in the <= -3 if a user table.
			return -1;					// error_exit.

		else if ( FieldNo < 0 )			// negative if we are including all *key* fields from TTno
		{
			TTno = -FieldNo;			// FieldNo held a TTno
			
			TTlist = kxtab->TTlist;
			while (*TTlist && (*TTlist != TTno))	// Look to see if this TT is already in the list.
				TTlist++;
				
			if (!*TTlist)				// end of list reached, not found. add it to the list
			{							// *** would be nice if we could have *normal* variables in the list.....??
				*TTlist = TTno;
				++kxtab->NumTTs;		// Number of TT's referenced
			}

			first_keys = kxtab->NumKeyFields == 0;		// Can only be true if this whole table is first specifier following create command
			
			fld = ttab[TTno].TTfields;
			while ( fld->FLDelemID )
			{
				if ( first_keys && fld->TDFtype & tdf_KEY )	// 0x0001 Add *all* key fields from whole table if no keys existed already
				{
					*KeyIDptr++ = fld->FLDelemID;
					++kxtab->NumKeyFields;
					kxtab->KeySize += fld->FLDlen;
					kxtab->RecSize += fld->FLDlen;
				}
				else if ( (v8 = fld->TDFtype & 0x3000, v8 != 0x2000) && v8 != 0x3000 )	// ignore sub/key recs
				{
					ElemIDptr = kxtab->KeyElemIDs;			// see if this element has already been indexed
					count = 0;
					while ( count < kxtab->NumKeyFields && *ElemIDptr != fld->FLDelemID )	// if *ElemIDptr == fld->FLDelemID , already in our keylist, just continue
					{
						count++;
						ElemIDptr++;
					}
					if ( *ElemIDptr != fld->FLDelemID )		// Doesn't exist in our Keylist, check for it in the Datalist
					{
						ElemIDptr = kxtab->DataElemIDs;
						count = 0;
						while ( count < kxtab->NumDataFields && *ElemIDptr != fld->FLDelemID )
						{
							count++;
							ElemIDptr++;
						}
						if ( *ElemIDptr != fld->FLDelemID )	// Not already in our datalist.
						{									// ** should always be true. list should be empty at this point **
							*DataIDptr++ = fld->FLDelemID;	// so add it.
							++kxtab->NumDataFields;
							kxtab->RecSize += fld->FLDlen;
						}
					}
				}
				fld++;
			}
		}
		else		// FieldNo >= 0 . Look for a single Field in a Table
		{
			TTlist = kxtab->TTlist;
			while (*TTlist && (*TTlist != TTno))	// Look to see if this TT is already in the list. TTno can't be zero
				TTlist++;
				
			if (!*TTlist)					// end of list reached, not found. add it to the list
			{								// *** would be nice if we could have *normal* variables in the list.....??
				*TTlist = TTno;
				++kxtab->NumTTs;			// Number of TT's referenced
			}

			fld = &ttab[TTno].TTfields[FieldNo];
			*KeyIDptr++ = fld->FLDelemID;	// Add the elementID to our KeyIDList. **doesn't check for duplicates!!**
			++kxtab->NumKeyFields;
			kxtab->KeySize += fld->FLDlen;	// adjust Key/Rec size to include this new element
			kxtab->RecSize += fld->FLDlen;
			
			v15 = 0;						// look for a descending sort specifier
			if ( symbol == 180 )			// ","
			{
				symbol = getsym();
				if ( sym[0] != 'd' )		// specifies a descending sort on this field
				{
					loaderr(84, sym);		// "invalid sort specification" ??
					return -1;
				}
				v15 = 0x4000u;				// descending sort flag
				symbol = getsym();			// update symbol for next loop
			}
			*SortORDptr++ = v15;
		}
	}
	return 0;	// exit_success
}


#endif
