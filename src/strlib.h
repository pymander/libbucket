/* Various expanded string manipulation functions in C
 *
 * Copyright (c) 2001-2002, Erik Arneson
 *
 * You may distribute this package under the terms of the Artistic
 * license as outlined in the "COPYING" file.
 *
 * $Id$
 */ 

#ifndef _STRLIB_H
# define _STRLIB_H 1

#include <string.h>

char * strspace      (char *);
char * strstriplead  (char *);
char * strstripfinal (char *);
char * strunesc      (char *);
char * strupper      (char *);
char * strlower      (char *);
int    strwords      (char *);

/* These are some macros that might be useful */
#define strchopfirst(str)   (memmove(str, str+1, strlen(str)))
#define strchoplast(str)    (str[strlen(str)-1] = '\0')
#define strsearch(str, sub) (strstr(str, sub) - str)

#endif /* _STRLIB_H */
/* End of file */
