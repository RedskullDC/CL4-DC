#ifndef LENSTR_C
#define LENSTR_C

#include <stdio.h>
#include <string.h>

inline int lenstr(char *a1)
{
  char *i;

  for ( i = a1; *i; ++i )
    ;
  //printf("a1 = %s, Length = %d , strlen(a1) = %d\n",a1, i-a1, strlen(a1));
  return i - a1;
}

#endif
