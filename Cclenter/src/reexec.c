#ifndef REEXAMINE_C
#define REEXAMINE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

#define SSIZE		50
#define TGRP		48
#define A256		01
#define A512		02
#define A768		03
#define	NBRA		10
#define CIRCFL		32

#define	CBRA		60
#define GRP			40
#define SGRP		56
#define PGRP		68
#define EGRP		44
#define RNGE		03
#define	CCHR		20
#define	CDOT		64
#define	CCL			24
#define	NCCL		8
#define	CDOL		28
#define	CAEOF		52	// dodge re-def error in /usr/include/sys/ttydefaults.h
#define	CKET		12

#define	STAR		01
#define PLUS		02
#define MINUS		16

char		*__braslist[NBRA];
char		*__braelist[NBRA];
char		*__loc1;
intptr_t	__bravar[NBRA];
intptr_t	*__st[SSIZE + 1];
intptr_t	*__eptr_,*__lptr_;
intptr_t	__cflg;

char *__execute(char *addrc, char *addrl)
{
    register char *p1, *p2;
	char *v3;
	char v4;
	char v5;
	char v7;
	
    p1 = addrl;
	p2 = addrc;
    __eptr_ = (intptr_t *)&__st[SSIZE];
    __lptr_ = (intptr_t *)&__st[0];
    
	if ( *addrc == CIRCFL )
    {
        __loc1 = p1;
        return (v3 = __advance(p1, ++p2))? v3 : addrl;
    }
    else
    {
        if ( *p2 == CCHR )
        {	// fast check for first character
            v7 = p2[1];
            while ( 1 )
            {
                if ( *p1 == v7 )
                {
                    __eptr_ = (intptr_t *)&__st[SSIZE];
                    __lptr_ = (intptr_t *)&__st[0];
                    v3 = __advance(p1, p2);
                    if ( v3 )
                        break;
                }
                v4 = *p1++;
                v3 = addrl;
                if ( !v4 )
                    return v3;
            }
        }
        else
        {	// regular algorithm
            while ( 1 )
            {
                __eptr_ = (intptr_t *)&__st[SSIZE];
                __lptr_ = (intptr_t *)&__st[0];
                v3 = __advance(p1, p2);
                if ( v3 )
                    break;
                v5 = *p1++;
                if ( !v5 )
                    return addrl;
            }
        }
        __loc1 = p1;
    }
    return v3;
}

char *__advance(char *lp, char *ep)
{
	char *v7;
	char *v10;
	char *v11;
	char *v12;
	char *v28;
	char *v29;
	char *sep;
	char *curlp;
	
    int v9; // edx@13
    int v19; // eax@37
    int v27; // edx@61
    int v34; // [sp+8h] [bp-30h]@45
	signed int gflg; // [sp+18h] [bp-20h]@1
    int v38; // [sp+24h] [bp-14h]@13
    int dcnt; // [sp+28h] [bp-10h]@13

    char v24; // al@54
    char v32; // ST04_1@67

    gflg = 0;
    while ( 2 )
    {
        switch ( *ep++ )
        {
			case CCHR:					// 20
                if ( *ep++ == *lp++ )
                    continue;
                return 0;
            
			case EGRP|RNGE:				// 44 | 03
                return lp;
             
			case EGRP:					// 44
			case GRP:					// 40
                ep++;
                continue;
           
			case EGRP|STAR:				// 44 | 01
                __xpop(0);				// falls thru...
			case EGRP|PLUS:				// 44 | 02
                __xpush(0, ep + 1);
                return lp;
            
			case CDOT:					// 64
                if ( *lp++ )
                    continue;
                return 0;
            
			case CDOL:					// 28
                if ( !*lp )
                    continue;
                return 0;				// **********************************
			
			case CAEOF:					// 52
                __cflg = 1;		// comma flag
                return lp;
            
			case TGRP:					// 48
			case TGRP|A768:				// 48 | 03
			case TGRP|A512:				// 48 | 02
			case TGRP|A256:				// 48 | 01
                v9 = (unsigned char)*ep + ((*(ep - 1) & 3) << 8);
                v10 = ep + 1;
                v11 = &v10[v9];
                __xpush(0, &v10[v9 + 2]);
                gflg = 1;
                __getrnge(&v38, &dcnt, v11);
                --v38;
                if ( v38 == -1 )
                    goto LABEL_16;
                while ( 1 )
                {
                    lp = __advance(lp, v10);
                    v7 = 0;
                    if ( !lp )
                        return v7;
                    --v38;
                    if ( v38 == -1 )
                    {
LABEL_16:
                        curlp = lp;
                        __xpush(1, lp);
                        --dcnt;
                        for ( ; dcnt != -1; --dcnt )
                        {
                            v12 = __advance(lp, v10);
                            if ( !v12 )
                                break;
                            lp = v12;
                            __xpush(1, v12);
                        }
                        ep = (char *)__xpop(0);
                        goto star;
                    }
                }
			case CCHR|RNGE:				// 20 | 03
                sep = ep++;
                __getrnge(&v38, &dcnt, ep);
                --v38;
                if ( v38 == -1 )
                    goto LABEL_23;
                do
                {
                    if ( *lp++ != *sep )
                        return 0;
                    --v38;
                }
                while ( v38 != -1 );
LABEL_23:
                curlp = lp;
                --dcnt;
                for ( ; dcnt != -1; --dcnt )
                {
                    v24 = *lp++;
                    if ( v24 != *sep )
                        break;
                }
				if ( dcnt < 0 ) lp++;
				ep += 2;
				goto star;
			
			case CDOT|RNGE:				// 64 | 03
                __getrnge(&v38, &dcnt, ep);
                --v38;
                if ( v38 == -1 )
                    goto LABEL_30;
                while ( 1 )
                {
                    v7 = 0;
                    if ( !*lp++)
                        return v7;
                    --v38;
                    if ( v38 == -1 )
                    {
LABEL_30:
                        curlp = lp;
                        --dcnt;
                        for ( ; dcnt != -1; --dcnt )
                        {
                            if ( !*lp++ )
                                break;
                        }
                        if ( dcnt < 0 )
                            ++lp;
                        ep += 2;
                        goto star;
                    }
                }
			
			case CCL|RNGE:				// 24 | 03
			case NCCL|RNGE:				// 8 | 03
                __getrnge(&v38, &dcnt, &ep[(unsigned char)*ep]);
                --v38;
                if ( v38 == -1 )
                    goto LABEL_39;
                break;
            
			case CCL:					// 24
                if ( (__cclass(ep, *lp++, 1)))
				{
					ep += *ep;
					continue;
				}
				return 0;

			case NCCL:					// 8
                v34 = 0;
                if ( (__cclass(ep, *lp++, 0)))
				{
					ep += *ep;
					continue;
				}
				return 0;
			
			case CBRA:					// 60
                __braslist[*ep++] = lp;
                continue;

			case CKET:					// 12
                __braelist[*ep] = lp;
                __bravar[*ep] = 0;		// ***********************
                ++ep;
                continue;

			case CDOT|PLUS:				// 64 | 02
				if (*lp++ == '\0') return(0);
										// fall through
			case CDOT|STAR:				// 64 | 01
                curlp = lp;
                while ( *lp++ );		// while not zero
                goto star;
            
			case CCHR|PLUS:				// 20 | 02
				if (*lp++ != *ep) return(0);
            
			case CCHR|STAR:				// 20 | 01
                curlp = lp;
                while ( *lp++ == *ep );
                ep++;
                goto star;

			case PGRP:					// 68
			case PGRP|A256:				// 68 | 01
			case PGRP|A512:				// 68 | 02
			case PGRP|A768:				// 68 | 03
				if (!(lp=__advance(lp,ep+1))) return(0);
			case SGRP|A768:				// 56 | 03
			case SGRP|A512:				// 56 | 02
			case SGRP|A256:				// 56 | 01
			case SGRP:					// 56
                v27 = (unsigned char)*ep + ((*(ep - 1) & 3) << 8);
                v28 = ep + 1;
                __xpush(0, &v28[v27]);
                curlp = lp;
                while ( 1 )
                {
                    __xpush(1, lp);
                    v29 = __advance(lp, v28);
                    if ( !v29 )
                        break;
                    lp = v29;
                }
                ep = (char *)__xpop(0);
                gflg = 1;
                goto star;
            
			case NCCL|PLUS:
            case CCL|PLUS:
                if ( !__cclass(ep, *lp++, *(ep - 1) == (CCL|PLUS))) return 0;
            
			case CCL|STAR:
            case NCCL|STAR:
                curlp = lp;
                while ( __cclass(ep, *lp++, ((ep[-1]==(CCL|STAR)) || (ep[-1]==(CCL|PLUS)))));
                ep += *ep;
                goto star;
            
			default:
                return 0;
        }
        break;
    }

//---------------------------
    while ( 1 )
    {
        v19 = __cclass(ep, *lp++, *(ep - 1) == 27);
        v7 = 0;
        if ( !v19 )
            break;
        --v38;
        if ( v38 == -1 )
        {
LABEL_39:
            curlp = lp;
            --dcnt;
            for ( ; dcnt != -1; --dcnt )
            {
                if ( !__cclass(ep, *lp++, *(ep - 1) == 27) )
                    break;
            }
            if ( dcnt < 0 )
                ++lp;
            ep += *ep + 2;
            
			do
            {
star:
                if ( gflg )
                {
                    lp = (char *)__xpop(1);
                    if ( !lp )
                        return 0;
                }
                else
                {
                    --lp;
                }
                v7 = __advance(lp, ep);
                if ( v7 )
                    return v7;
            }
            while ( lp > curlp );
            return 0;
        }
    }
    return v7;
}

int __cclass(char *a1, char ac, int af)
{
    char	*set;
	char	*v6;
	char	*v7;
	int		v3;
	int		v5;
	
    v3 = 0;
    if ( ac )
    {
        set = a1 + 1;
        v5 = *a1 - 1;
        if ( *a1 == 1 )
            return af == 0;
        
		while ( 1 )
        {
            if ( *set == MINUS )
            {
                if ( set[2] - set[1] < 0 )
                    return 0;
                
				v6 = set + 1;
                if ( *v6 > ac )
                {
                    v7 = v6 + 1;
                }
                else
                {
                    v7 = v6 + 1;
                    if ( ac <= *v7 )
                        return af;
                }
                set = v7 + 1;
                v5 -= 2;
            }
            else
            {
                v3 = af;
                if ( *set++ == ac )
                    return v3;
            }
            --v5;
            if ( !v5 )
                return af == 0;
        }
    }
    return v3;
}

int __xpush(int a1, char* p)
{
    if ( __lptr_ >= __eptr_ )
    {
        write(2, "stack overflow\n", 15);
        exit(1);
    }
    if ( a1 ) *__lptr_++ = (intptr_t)p;
    else      *__eptr_-- = (intptr_t)p;
    return 1;
}

int __xpop(int a1)
{
    int *v1; // edx@3

    if ( a1 )
    {
        if ( (unsigned int)__lptr_ >= (unsigned int)__st )
        {
            v1 = __lptr_;
            --__lptr_;
            return v1[-1];
        }
    }
    else
    {
        if ( (unsigned int)__eptr_ <= (unsigned int)&__st[200] )
        {
            v1 = __eptr_;
            ++__eptr_;
            return v1[1];
        }
    }
    return 0;
}


void __getrnge(int *low, int *size, char *str)
{
    *low = (unsigned char)*str++;
    if ( *str == -1 )
        *size = 20000;
    else
        *size = (unsigned char)*str - *low;
}

//getrnge(char *str)
//{
//	low = *str++ & 0377;
//	size = *str == 255 ? 20000 : (*str &0377) - low;
//}

char *reexec(char *addrc, char *addrl, char **a3)
{
    char	*p1;
	char	*p2;
	char	*result;
	char	*cur;
	
	int		in;
	char	v8;
	
    *a3 = 0;
	for (in = 0; in < NBRA ; in++)
	{
        __braslist[in] = 0;
        __bravar[in] = -1;
	}
    
	__cflg = 0;
    cur = __execute(addrc, addrl);                     // sets _cflg!
    
	for (in = 0; in < NBRA; in++)
    {
        if ( (p1 = __braslist[in]) && __bravar[in] >= 0 )
        {
            p2 = (char *)mmalloc(__braelist[in] - p1 + 1);	// *****************
            *a3 = p2;										// a3 never advanced?
			while( p1 < __braelist[in] )
                *p2++ = *p1++;
            *p2 = 0;
        }
    }
    
	if ( __cflg )
    {
        result = cur;
    }
    else
    {
        result = 0;
        if ( addrl != cur )
            result = cur;
    }
    return result;
}

#endif
