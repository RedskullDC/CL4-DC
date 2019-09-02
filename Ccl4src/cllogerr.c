#ifndef CLLOGERROR_C
#define CLLOGERROR_C

void cllogerr(int ErrorCode, char *ProgName, char *DBASEname, char *TABLEname)
{
	//char *v4; 
	//char *v5; 
	char *TimeStamp;	
	__pid_t PID;	
	__uid_t UID;	
	FILE *stream;	
	time_t timer;	
	char v14[32];	
	char v15[32];	
	char FileName[80];

	if ( ErrorCode != 15 || !isCGI )
	{
		fflush(stderr);
		//v4 = ttyname(2);
		//cdbcpystr(v15, v4, 0);
		//v5 = getevar("CLELOG");
		//cdbcpystr(FileName, v5, 0);
		cdbcpystr(v15, ttyname(2), 0);
		cdbcpystr(FileName, getevar("CLELOG"), 0);

		if ( !FileName[0] )
			cdbcpystr(FileName, "CLELOG", 0);
		stream = fopen64(FileName, "a+");
		if ( stream )
		{
			cdbcpystr(v14, v15, 0);
			if ( !v14[0] )
				cdbcpystr(v14, "bkgnd", 0);
			time(&timer);
			TimeStamp = ctime(&timer);
			PID = getpid();
			UID = getuid();
			fprintf(stream, "%s(%s): tty=%s, uid=%d, pid=%d, %s", _pname, ProgName, v14, UID, PID, TimeStamp);
			if ( ErrorCode >= 0 )
			{
				if ( ErrorCode <= 0 )
				{
					if ( DBASEname && *DBASEname )
						fprintf(stream, "\t%s\n", DBASEname);
					if ( !TABLEname || !*TABLEname )
					{	
						fflush(stream);
						fclose(stream);
						return;
					}
					fprintf(stream, "\t%s\n", TABLEname);
				}
				else
				{
					fprintf(stream, "\tSignal %d\n", -ErrorCode);
				}
			}
			else
			{
				fprintf(stream, "\t%s err %d ", (DBASEname ? "DBASE" : "INTERNAL"), -ErrorCode);
				if ( DBASEname && *DBASEname )
				{
					fprintf(stream, "Database: [%s] ", DBASEname);
					if ( TABLEname )
						fprintf(stream, "Table: [%s]", TABLEname);
				}
				fputc('\n', stream);
			}
			fflush(stream);
			fclose(stream);
			return;
		}
	}
}

#endif
