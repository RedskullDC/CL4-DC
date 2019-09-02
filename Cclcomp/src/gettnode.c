#ifndef GETTNODE_C
#define GETTNODE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <assert.h>
#include "DBdefs.h"
#include "lvars.h"

bool alllower(char *a1)			// checks variable names for validity
{
	short	num_lc = 0;			// number of lower case chars found in the string
	int		cchar;

	cchar = *a1++;

	if (isalpha(cchar))				// Variable names must start with an ALPHA char	// isalpha() == 0x0400
	{
		if (islower(cchar))			// islower() == 0x0200
			num_lc = 1;
		
		while ( *a1 )
		{
			cchar = *a1++;
			if ( !isdigit(cchar) && isupper(cchar))		// isdigit() == 0x0800, isupper() == 0x0100
				return false;
			if ( islower(cchar))	// islower()  ==	0x0200
				num_lc++;
			
		}
		return num_lc > 0;
	}
	return false;
}

void alltoupper(char *a1)
{
	int v2; // edi@3

	while ( *a1 )
	{
		v2 = *a1;
		if (islower(v2))		//if ((*v3)[v2] & 0x0200 )	// islower() == 0x0200
			*a1 = toupper(v2);
		a1++;
	}
}

VARTREE *mktree(VARTREE *vartree, const char *VarName, int namesize, int VarNo)
{
	VARTREE *v4; // esi@1
	short v5; // dx@3

	//eprint("vartree - sizeof(VARTREE) = %d\n", sizeof(VARTREE));
	v4 = vartree;
	if ( vartree )
	{
		v5 = strncmp(VarName, vartree->VarName, namesize + 1);		// compare all string, including the \0
		if ( !v5 )
			__assert_fail("0", "gettnode.c", 0x6Au, "mktree");

		if ( v5 > 0 )
			vartree->VarNext = mktree(vartree->VarNext, VarName, namesize, VarNo);
		else // v5 < 0
			vartree->VarPrev = mktree(vartree->VarPrev, VarName, namesize, VarNo);
	}
	else	// vartree node doesn't exist. Create it.
	{
		v4 = (VARTREE *)getmem(sizeof(VARTREE));		// 16
		v4->VarName = getmem(namesize + 1);
		cpybuf(v4->VarName, (char *)VarName, namesize);
		v4->VarNo = VarNo;
	}
	return v4;
}

VARTREE *dogettnode(VARTREE *vartree, const char *varname, int namesize, int a4)
{
	VARTREE		*v4;
	short		v6;
	short		v7;
	
	v4 = vartree;
	if ( vartree )
	{
		v7 = strncmp(varname, vartree->VarName, namesize + 1);
		if ( v7 )
		{
			if ( v7 > 0 )
				vartree->VarNext = dogettnode(vartree->VarNext, varname, namesize, a4);
			else
				vartree->VarPrev = dogettnode(vartree->VarPrev, varname, namesize, a4);
		}
		else	// match
		{
			last_fnd = vartree->VarNo;
			++tnodewasfound;
		}
	}
	else
	{
		if ( !a4 )
			return 0;
		v4 = (VARTREE *)getmem(sizeof(VARTREE));		// 16
		v4->VarName = getmem(namesize + 1);
		cpybuf(v4->VarName, (char *)varname, namesize);
		v6 = last_var++;								// global
		last_fnd = v6;
		v4->VarNo = v6;
		if ( last_fnd < res_v_max )
		{
			if ( (unsigned char)(*varname - 97) <= 0x19u )
				__assert_fail("!('a' <= (*w) && (*w) <= 'z')", "gettnode.c", 0x8Eu, "dogettnode");
			//assert(!('a' <= (*varname) && (*varname) <= 'z'));
			rtroot = mktree(rtroot, varname, namesize, last_fnd);
		}
	}
	return v4;
}

VARTREE *gettnode(VARTREE *vartree, char *varname, int namesize, int a4)
{
	char s1[128];

	tnodewasfound = 0;
	if ( alllower(varname) && (strcpy(s1, varname), alltoupper(s1), dogettnode(rtroot, s1, namesize, 0), tnodewasfound) )
	{
		if ( !noWarnings )
			eprint("\"%s\", line %4d - warning: [%s] - replace with [%s]\n", FileTree->FileName, (unsigned short)ll, varname, s1);
		return vartree;
	}
	else
		return dogettnode(vartree, varname, namesize, a4);
}

#endif
