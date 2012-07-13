/* Various expanded string manipulation functions in C.  All of these
 * functions are DESTRUCTIVE unless otherwise noted.
 *
 * Copyright (c) 2001-2002, Erik Arneson
 *
 * You may distribute this package under the terms of the Artistic
 * license as outlined in the "COPYING" file.
 *
 * $Id: strlib.c,v 1.1 2001/08/28 18:19:01 earneson Exp $
 */

#ifdef HAVE_CONFIG_H
# include "../config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "strlib.h"

/* Remove duplicate spaces from a string. */
char *
strspace(str)
    char *str;
{
    register int i, j;
  
    for (i=0, j=0 ; str[i] != (char)NULL; i++) {
        if (!(isspace(str[i]) && isspace(str[i+1]))) {
            if (isspace(str[i]))
                str[j++] = ' ';
            else
                str[j++] = str[i];
        }
    }
    str[j] = 0;

    return str;
}

/* Strip leading spaces from a string. */
char *
strstriplead(str)
    char *str;
{
    register int i = 0, j;

    j = strlen(str) + 1;
    while (isspace(str[i++]))
        j--;

    memmove(str, &str[i-1], j);

    return str;
}

char *
strstripfinal(str)
    char *str;
{
    register int i;

    i = strlen(str) - 1;
    while (isspace(str[i]))
        i--;
    str[i+1] = '\0';

    return str;
}

/* Translate escape characters in a string */
char *
strunesc(str)
    char * str;
{
    /* p is our standard pointer string, f is the 'following'
     * pointer. */
    register char *p, *f;

    f = p = str;
    while (*p != '\0') {
        if (*p == '\\') {
            switch (*++p) {
            case 'n':
                *f++ = '\n';
                break;
            case 't':
                *f++ = '\t';
                break;
            case '\\':
                *f++ = '\\';
                break;
            default:
                *f++ = *p;
            }
        } else
            *f++ = *p;
        p++;
    }

    *f = '\0';

    return str;
}

char *
strupper(str)
    char *str;
{
    register char *p;

    p = str;
    while (*p != '\0') {
        *p = toupper(*p);
        p++;
    }

    return str;
}

char *
strlower(str)
    char *str;
{
    register char *p;

    p = str;
    while (*p != '\0') {
        *p = tolower(*p);
	p++;
    }

    return str;
}

/* Count the words in a string */
int
strwords(p)
    char *p;
{
    register int count = 0;

    while (*p != '\0') {
        if (!isspace(*p)) {
            count++;
            while (*p != '\0' && !isspace(*p))
                p++;
        }
        while (*p != '\0' && isspace(*p))
            p++;
    }

    return count;
}

/* End of file */
