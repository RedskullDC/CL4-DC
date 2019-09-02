#ifndef GETIDENT_C
#define GETIDENT_C

// only called by lecreate() routines

short getident(char *FieldName, int *TTno, char *FieldType)
{
	int		TDno;
    short	fno;
    char	FLDtype;
    bool	WholeTable = false;
    char	TableName[128];		// 128 is probably overkill
    char	FieldName2[128];

	if ( symbol )
	{
		cdbcpystr(TableName, FieldName, 0);
		FieldName2[0] = 0;
		
		symbol = getsym();
		if ( symbol == 1080 )                   // "." (decimal point)
		{
			//WholeTable = false;
			symbol = getsym();
			cdbcpystr(FieldName2, sym, 0);
		}
		else
			WholeTable = true;                  // if no ".", we want to include *all* fields from this table

		TDno = findtd(TableName, -1);			// Look for tablename in *all* open *user* tables.
		*TTno = TDno;							// Update caller mem directly
		
		if ( TDno >= 0 )						
		{
			if ( WholeTable )
				return -TDno;						// Return the *negative* TDno if whole Record
			
			fno = findfno(*TTno, FieldName2);
			if ( fno >= 0 )
			{
				FLDtype = ttab[*TTno].TTfields[fno].FLDtype;
				*FieldType = FLDtype;
				if ( FLDtype != 'S' && FLDtype != 'K' )		// set nd key table types
				{
					symbol = getsym();				// updates symbol for caller
					return fno;						// Return the FieldNo in this case - *** exit_success ***
				}
				loaderr(24, FieldName2);			// "invalid operation on 'set' or 'key' field"
			}
			else
				loaderr(8, FieldName2);				// "invalid field name or number"
		}
		else										// couldn't find this table amongst the open tables
			loaderr(14, TableName);					// "table not open"
	}
	return -1;			// -1 == ERROR
	
	// Will be in the <= -3 if a user table.
	// Safe to use -1 as an error indicator
}
#endif
