/* Rewrite file test program
   Specify location of rewrite file
   and your hostname below and compile

   Code taken directly from NOS
   Compiles with JN110x5 support code
   bcc rw_test.c
*/

#include <ctype.h>
#include <stdio.h>
#include "global.h"

#define Hostname "wa3dsp.ampr.org"
#define Rewrite "rewrite"

char *rewrite_address(char *);


void
main()
{
 char addr[100];
 printf("\nRewrite File Test Program\n\n");
 while (1==1) {
     printf("Enter address - (^C to Exit) -  ");
     scanf("%s",addr);
     printf("\nResult - %s\n\n",rewrite_address(addr));
  }
}


static int Star __ARGS((char *s,char *p,char **argv,int single));
static int haveDot __ARGS((register char *c,register int len));


/* Read the rewrite file for lines where the first word is a regular
 * expression and the second word are rewriting rules. The special
 * character '$' followed by a digit denotes the string that matched
 * a '*' character. The '*' characters are numbered from 1 to 9.
 * Example: the line "*@*.* $2@$1.ampr.org" would rewrite the address
 * "foo@bar.xxx" to "bar@foo.ampr.org".
 * $H is replaced by our hostname, and $$ is an escaped $ character.
 * If the third word on the line has an 'r' character in it, the function
 * will recurse with the new address.
 */
char *
rewrite_address(addr)
char *addr;
{
        char *argv[10], buf[200], *cp, *cp2, *retstr;
        int cnt;
        FILE *fp;

        if ((fp = fopen(Rewrite,READ_TEXT)) == NULLFILE){
                printf("\nFile '%s' not found\n",Rewrite);
                exit(1);
                }

        memset((char *)argv,0,10*sizeof(char *));
        while(fgets(buf,sizeof(buf),fp) != NULLCHAR) {
                if(*buf == '#')         /* skip commented lines */
                        continue;

        if((cp = strchr(buf,' ')) == NULLCHAR) /* get the first word */
                        if((cp = strchr(buf,'\t')) == NULLCHAR)
                                continue;
                *cp = '\0';
                if((cp2 = strchr(buf,'\t')) != NULLCHAR){
                        *cp = ' ';
                        cp = cp2;
                        *cp = '\0';
                }
                if(!wildmat(addr,buf,argv))
                        continue;               /* no match */
                rip(++cp);
        /* scan past additional whitespaces */
        while (*cp == ' ' || *cp == '\t') ++cp;
        cp2 = retstr = (char *) calloc(1,200);
                while(*cp != '\0' && *cp != ' ' && *cp != '\t')
                        if(*cp == '$') {
                                if(isdigit(*(++cp)))
                                        if(argv[*cp - '0'-1] != '\0')
                                                strcat(cp2,argv[*cp - '0'-1]);
                                if(*cp == 'h' || *cp == 'H') /* Our hostname */
                                        strcat(cp2,Hostname);
                                if(*cp == '$')  /* Escaped $ character */
                                        strcat(cp2,"$");
                                cp2 = retstr + strlen(retstr);
                                cp++;
                        }
                        else
                                *cp2++ = *cp++;
                for(cnt=0; argv[cnt] != NULLCHAR; ++cnt)
                        free(argv[cnt]);
                fclose(fp);
                /* If there remains an 'r' character on the line, repeat
                 * everything by recursing.
                 */
                if(strchr(cp,'r') != NULLCHAR || strchr(cp,'R') != NULLCHAR) {
                        if((cp2 = rewrite_address(retstr)) != NULLCHAR) {
                                free(retstr);
                                return cp2;
                        }
                }
                return retstr;
        }
        fclose(fp);
        return "NO Match";
}
/*
 * @(#)wildmat.c 1.3 87/11/06   Public Domain.
 *
From: rs@mirror.TMC.COM (Rich Salz)
Newsgroups: net.sources
Subject: Small shell-style pattern matcher
Message-ID: <596@mirror.TMC.COM>
Date: 27 Nov 86 00:06:40 GMT

There have been several regular-expression subroutines and one or two
filename-globbing routines in mod.sources.  They handle lots of
complicated patterns.  This small piece of code handles the *?[]\
wildcard characters the way the standard Unix(tm) shells do, with the
addition that "[^.....]" is an inverse character class -- it matches
any character not in the range ".....".  Read the comments for more
info.

For my application, I had first ripped off a copy of the "glob" routine
from within the find(1) source, but that code is bad news:  it recurses
on every character in the pattern.  I'm putting this replacement in the
public domain.  It's small, tight, and iterative.  Compile with -DTEST
to get a test driver.  After you're convinced it works, install in
whatever way is appropriate for you.

I would like to hear of bugs, but am not interested in additions; if I
were, I'd use the code I mentioned above.
*/
/*
**  Do shell-style pattern matching for ?, \, [], and * characters.
**  Might not be robust in face of malformed patterns; e.g., "foo[a-"
**  could cause a segmentation violation.
**
**  Written by Rich $alz, mirror!rs, Wed Nov 26 19:03:17 EST 1986.
*/

/*
 * Modified 6Nov87 by John Gilmore (hoptoad!gnu) to return a "match"
 * if the pattern is immediately followed by a "/", as well as \0.
 * This matches what "tar" does for matching whole subdirectories.
 *
 * The "*" code could be sped up by only recursing one level instead
 * of two for each trial pattern, perhaps, and not recursing at all
 * if a literal match of the next 2 chars would fail.
 */

/* Modified by Anders Klemets to take an array of pointers as an optional
   argument. Each part of the string that matches '*' is returned as a
   null-terminated, malloced string in this array.
 */

static int
haveDot(s,len)
register char *s;
register int len;
{
        while (len-- > 0)
                if (*s++ == '.')
                        return TRUE;
        return FALSE;
}

static int
Star(s,p,argv,single)
register char *s;
register char *p;
register char **argv;
int single;
{
        char *cp = s;
        while (wildmat(cp, p, argv) == FALSE)
                if(*++cp == '\0')
                        return -1;
        if ((single == TRUE) && haveDot(s, cp - s))
                return -1;
        return cp - s;
}

int
wildmat(s,p,argv)
register char *s;
register char *p;
register char **argv;
{
        register int last;
        register int matched;
        register int reverse;
        register int cnt;
        int single;

        for(; *p; s++,p++){
                switch(*p){
                case '\\':
                        /* Literal match with following character; fall through. */
                        p++;
                default:
                     /*   if(*s != *p)   */
                     if (tolower(*s) != tolower(*p))
                                return FALSE;
                        continue;
                case '?':
                        /* Match anything. */
                        if(*s == '\0')
                                return FALSE;
                        continue;
                case '*':
                case '+':
                        single = (*p == '+');

                        /* Trailing star matches everything. */
                        if(argv == NULLCHARP)
                                return *++p ? 1 + Star(s, p, NULLCHARP, single) : TRUE;
                        if(*++p == '\0'){
                                cnt = strlen(s);
                                if ((single == TRUE) && haveDot(s, cnt))
                                        return FALSE;
                        } else {
                                if((cnt = Star(s, p, argv+1, single)) == -1)
                                        return FALSE;
                        }
                        *argv = malloc(cnt+1);
                        strncpy(*argv,s,cnt);
                        *(*argv + cnt) = '\0';
                        return TRUE;
                case '[':
                        /* [^....] means inverse character class. */
            reverse = (p[1] == '^' || p[1] == '!') ? TRUE : FALSE;
                        if(reverse)
                                p++;
                        for(last = 0400, matched = FALSE; *++p && *p != ']'; last = *p){
                                /* This next line requires a good C compiler. */
                                if(*p == '-' ? *s <= *++p && *s >= last : *s == *p)
                                        matched = TRUE;
                        }
                        if(matched == reverse)
                                return FALSE;
                        continue;
                }
        }
        /* For "tar" use, matches that end at a slash also work. --hoptoad!gnu */
        return *s == '\0' || *s == '/';
}


#ifdef  TEST
#include <stdio.h>

extern char *gets();

main()
{
        char pattern[80];
        char text[80];
        char *argv[80], *cp;
        int cnt;
    
        while (TRUE){
                printf("Enter pattern:  ");
                if(gets(pattern) == NULL)
                        break;
                while (TRUE){
                        bzero(argv,80*sizeof(char *));
                        printf("Enter text:  ");
                        if(gets(text) == NULL)
                                exit(0);
                        if(text[0] == '\0')
                                /* Blank line; go back and get a new pattern. */
                                break;
                        printf("      %d\n", wildmat(text, pattern, argv));
                        for(cnt = 0; argv[cnt] != NULLCHAR; ++cnt){
                                printf("String %d is: '%s'\n",cnt,argv[cnt]);
                                free(argv[cnt]);
                        }
                }
        }
        exit(0);
}
#endif  /* TEST */

/* replace terminating end of line marker(s) with null */
void
rip(s)
register char *s;
{
        register char *cp;

        if((cp = strchr(s,'\n')) != NULLCHAR)
                *cp = '\0';
}

