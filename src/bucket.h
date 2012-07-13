/* Various expanded string manipulation functions in C
 *
 * Copyright (c) 2001-2002, Erik Arneson
 *
 * You may distribute this package under the terms of the Artistic
 * license as outlined in the "COPYING" file.
 *
 * $Id$
 */

#ifndef _BUCKET_H
# define _BUCKET_H 1

#include <string.h>
#include <stdarg.h>

/* Default block size for string bucket memory blocks. */
#define STRB_SIZE 4096

/* Maximum number of digits allowed in conversions (such as 
 * strb_vappendf()). If this is larger than required, no harm
 * is done. */
#define STRB_MAX_VAL_DIGITS 16

/* Structures used to redefine some of our standard flex
 * functionality. */
typedef struct _str_bucket {
    char * bucket;

    /* "loc" is a cursor used by internal functions in various ways. After
     * some functions (fe. strb_puts()) it points to the NULL at the end of
     * a string; after others (fe. strb_set()) it does not. */

    char * loc;

    /* The first of these variables is used as the strlen of the stored
     * string, the second as the amount of string memory allocated by
     * the bucket.  This is used when the str_bucket is more of an
     * "incoming" buffer than an "outgoing" buffer. */

    int    length;
    int    memlen;

    /* The blocksize member is a constant which is added to realloc() calls
     as a time optimization. */
    
    int    blocksize;
} str_bucket;

/* These are our string bucket functions. */
str_bucket *  new_str_bucket     (void);
void          destroy_str_bucket (str_bucket *);
int           strb_ready         (str_bucket *, int);
int           strb_set           (str_bucket *, char *);
int           strb_puts          (str_bucket *, char *, size_t);
int           strb_putc          (str_bucket *, char);
int           strb_concat        (str_bucket *, str_bucket *);
int           strb_vappendf      (str_bucket *, char *, va_list);
int           strb_appendf       (str_bucket *, char *, ...);
void          strb_reset         (str_bucket *);
char *        strb_strdup        (str_bucket *);
int           strb_getc          (str_bucket *, char *, int);
void          strb_ungetc        (str_bucket *, int);
int           strb_count         (str_bucket *, int);

/* We need some other things to make sense. */
#define strb_buffer(x, y, z)   strb_puts(x, y, z)

#endif /* _BUCKET_H */
/* End of file */
