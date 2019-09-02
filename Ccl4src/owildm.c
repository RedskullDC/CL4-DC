#ifndef OWILDM_C
#define OWILDM_C

#include "DBdefs.h"
#include "cl4.h"
//#define _LARGEFILE64_SOURCE

int Star_0(char *v3, char *a2, short noncase)
{

	while ( owildmat(v3, a2, noncase) )	// recursive, calls back into owildmat()
	{
		++v3;
		if ( !*v3 )
			return -1;
	}
	return 0;
}

// appears derived from GNU tar wildmat.c 

int owildmat(char *v3, char *v4, short noncase)
{

	char	*v9;
	int		v7;
	int		result;
	int		v12;
	int		v6;
	int		v13;
	
	bool	reverse;
	bool	matched;
	register int last;

	//printf("owildmat(v3 = %s, v4 = %s, noncase = %d)\n",v3,v4,noncase);
  	while ( *v4 )
	{
		switch ((unsigned char)*v4)
		{
		case 0xBFu:			// -65 '?' 0x3F with high bit set 0x3F -> 0xBF
			if ( !*v3 )
				return -*v4;
			break;

		case 0xAAu:			// -86 '*' with high bit set. 0x2A -> 0xAA
			v9 = v4 + 1;
			if ( *v9 )		// anything after the '*'?
				result = Star_0(v3, v9, noncase);	// can be recursive
			else
				result = 0;
			return result;
			break;
		
		case 0xDBu:			// -37 '[' with high bit set 0x5B -> 0xDB
			// [^....] means inverse character class.
			reverse		= v4[1] == '^';		// '^' 0x5E 94;
			last		= 256;
			matched		= false;
			for ( v4 += reverse + 1; *v4 && *v4 != ']'; last = *v4++ )	// 93 = ']'
			{
				if (*v4 == '-')		// 45 
				{
					++v4;
					if ( *v3 > *v4 || *v3 < last )
						continue;
				}
				else
				{
					if ( *v3 != *v4 )
						continue;
				}
				matched = true;
			}
			if ( matched == reverse )
				return *v3 - *(v4 - 1);

			break;
			
		case '\\':						// '\\' 0x5C 92
			v4++;		// fall through to default case
		
		default:
			if ( noncase & 8 )			// try a case-insensitive match
			{
				v6 = *v3;
				if ( (*v3 - 0x41) <= 0x19u )		
					v6 += 32;
				v7 = v6 - *v4;
				if ( (*v4 - 0x41) <= 0x19u )
					v7 -= 32;
			}	
			else
				v7 = *v3 - *v4;

			if ( v7 )
				return v7;				// return when different
			break;
		}

		++v3;
		++v4;
	}	// end while (*v4)

	return *v3 - *v4;
}

#endif
