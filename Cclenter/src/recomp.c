#ifndef RECOMP_C
#define RECOMP_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values


#define SLOP	5


char	**__sp_;
char	**__stmax;
int	 __i_size;

int __size(char *strg)
{
	int v1 = 1;

	while(*strg++) v1++;
	return v1;
}

char *_rpop()
{
	return (*__sp_ == (char*)-1) ? 0 : *__sp_--;
}

bool _rpush(char *ptr)
{
	if (++__sp_ > __stmax) return false;
	*__sp_ = ptr;
	return true;
}

char *recomp(char *cs1, char *cs2, char *cs3)
{
  int v3; // esi@1
  char *v4; // ST00_4@2
  char *v5; // edi@3
  char *v6; // eax@3
  char *result; // eax@3
  int v8; // edx@4
  char *v9; // edi@4
  char *i; // edi@9
  char v11; // edx@10
  char *v12; // eax@12
  char *v13; // eax@18
  char *v14; // ecx@23
  signed int v15; // edx@25
  char v16; // al@26
  int v19; // esi@37
  signed int cclcnt; // ecx@37
  char *v21; // edx@40
  int v22; // edx@58
  int v23; // esi@59
  char *v24; // eax@64
  int v25; // edx@66
  int v26; // edx@85
  int cflg; // [sp+1Ch] [bp-ACh]@57
  int ngrp; // [sp+24h] [bp-A4h]@1
  int nbra; // [sp+28h] [bp-A0h]@1
  char *eptr; // [sp+2Ch] [bp-9Ch]@24
  char *sep; // [sp+30h] [bp-98h]@3
  char *lastep; // [sp+34h] [bp-94h]@1
  char **v33; // [sp+38h] [bp-90h]@1
  int *v34; // [sp+38h] [bp-90h]@3
  char *ep; // [sp+3Ch] [bp-8Ch]@3
  char *v36; // [sp+40h] [bp-88h]@1
  char *v37; // [sp+44h] [bp-84h]@1
  char *v38; // [sp+48h] [bp-80h]@1
  char stack[51]; // [sp+60h] [bp-68h]@1

	lastep = 0;
	v36 = cs1;
	v37 = cs2;
	v38 = cs3;
	__sp_ = (char**)stack;
	*__sp_ = (char*)-1;
	__stmax = (char**)&stack[50];
	v33 = &v36;
	ngrp = 0;
	nbra = 0;
	v3 = 0;
	if ( cs1 )
	{
		do
		{
			v4 = *v33;
			++v33;
			v3 += __size(v4);
		}
		while ( *v33 );
	}
	v34 = (int *)&v37;
	v5 = v36;

	v6 = mmalloc(2 * v3 + SLOP);
	ep = v6;
	sep = v6;
	result = 0;
	if ( sep )
	{

    v8 = *v5;
    v9 = v5 + 1;
    if ( !v8 )
      goto cerror;
    if ( v8 == '^' )
    {
      v8 = *v9++;
      *ep++ = CIRCFL;
    }
	if ((v8 == '*') || (v8 == '+') || (v8 == '{'))
		goto cerror;

    for ( i = v9 - 1; ; i = v12 )
    {
LABEL_10:
      while ( 1 )
      {
        v11 = *i++;
        if ( !v11 )
          break;
        if ( v11 != '*' && v11 != '{' )
        {
          v13 = ep;
          if ( v11 == '+' )
            v13 = lastep;
          lastep = v13;
        }
        
		switch ( v11 )
        {
          case '(':
            if ( !_rpush(ep) )
              goto cerror;
            *ep = '<';
            v14 = ep + 1;
            ep[1] = -1;
            ep = v14 + 1;
			break;
          
		  case ')':
            eptr = _rpop();
            if ( !eptr )
              goto cerror;
            v15 = *i++;
            if ( v15 == '>' )	// different to regcmp.c
            {
              *ep = CKET;
              v16 = nbra++;
              eptr[1] = v16;
              v14 = ep + 1;
              ep[1] = v16;
              ep = v14 + 1;
            }
            else
            {
              *ep++ = EGRP;
              *ep++ = ngrp++;
              --i;
			  switch (v15)
			  {
			  case '+':
				  *eptr = PGRP;
				  break;
			  case '*':
                  *eptr = SGRP;
				  break;
			  case '{':
                  *eptr = TGRP;
				  break;
			  default :
                  *eptr = GRP;
				  continue;			// *****			
			  }

			  v19 = ep - eptr - 2;
              for ( cclcnt = 0; v19 > 255; cclcnt++ )
                v19 -= 256;
              if ( cclcnt > 3 ) goto cerror;
              v21 = eptr;
              *v21 |= cclcnt;
              //LOBYTE(cclcnt) = v19;
			  cclcnt = v19;
              v21[1] = cclcnt;
            }
            break;
          
		  case '\\':				// diff to regcmp!!
            *ep++ = CCHR;
            v11 = *i++;
            if ( !v11 )
              goto cerror;
            if ( v11 == 'n' )
				v11 = '\n';
            else if ( v11 == 'r' )
				v11 = '\r';
            else if ( v11 == 't' )
                v11 = '\t';
            else if ( v11 == 'f' )
				v11 = '\f';
            else if ( (unsigned int)(v11 - 48) <= 7 )	// octal number
            {
				for ( v11 -= 48; (unsigned char)(*i - 48) <= 7u; i++ )
					v11 = *i + 8 * v11 - 48;
            }
            *ep++ = v11;
			break;
          
		  case '{':
            *lastep |= RNGE;
            cflg = 0;
            
			while ( 2 )
            {
              if ((v22 = *i++) == '}') goto cerror;
              v23 = 0;
              do
              {
                if ( (unsigned int)(v22 - 48) > 9 )
                  goto cerror;
                v23 = v22 + 10 * v23 - 48;
                v22 = *i++;
              }
              while ( v22 != '}' && v22 != ',' );
              if ( v23 > 255 ) goto cerror;
              
			  *ep++ = v23;
              if ( v22 == ',' )
              {
                if ( ++cflg != 1 )
                  goto cerror;
                v25 = *i++;
                if ( v25 != '}' )
                {
                  --i;
                  continue;	// continue local loop
                }
                *ep++ = -1;
              }
              else
              {
                if ( !cflg )
                {
				  *ep++ = v23;
				  break;
                }
                if ( (unsigned char)*(ep - 1) < (signed int)(unsigned char)*(ep - 2) )
                  goto cerror;
              }
              goto LABEL_10;
            }
          
		  case '.':
            *ep++ = CDOT;
            continue;
          
		  case '+':
            if ( *lastep == CBRA || *lastep == CKET )
              goto cerror;
            *lastep |= PLUS;
            continue;
          
		  case '*':
            if ( *lastep == CBRA || *lastep == CKET )
              goto cerror;
            *lastep |= STAR;
            continue;
          
		  case '$':
            if ( *i || *v34 )
              goto defchar;
            *ep++ = CDOL;
            goto LABEL_10;
          
		  case '[':
            *ep++ = CCL;
            *ep++ = 0;
            cclcnt = 1;
            
            if ( (v26 = *i++) == '^' )
            {
              v26 = *i++;
              ep[-2] = NCCL;
            }
            do
            {
              if ( !v26 )
                goto cerror;
              if ( v26 != '-' || cclcnt <= 1 || *i == ']' )
              {
                *ep++ = v26;
              }
              else
              {
                *ep = *(ep - 1);
                *(++ep - 2) = MINUS;
              }
              cclcnt++;
            }
            while ((v26 = *i++) != ']');
			lastep[1] = cclcnt;
			break;
          
		  default:
defchar:
            *ep++ = CCHR;
            *ep++ = v11;
			break;
        }
      }
      if ( !*v34 )
        break;
      v12 = (char *)*v34;
      ++v34;
    }
    *ep = 52;
    if ( nbra - 1 <= 10 && *__sp_ == (char*)-1 )
    {
      __i_size = ep + 1 - sep;
      result = sep;
    }
    else
    {
cerror:
      mfree_0(sep);
      result = 0;
    }
  }
  return result;
}


#endif
