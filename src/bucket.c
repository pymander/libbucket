/* String Buffer Library
 *
 * The string bucket is a method of storing a string and moving around
 * in it like you would a stream.
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
#include <stdarg.h>
#include <assert.h>

/* Local includes */
#include "bucket.h"
#include "strlib.h"

str_bucket *
new_str_bucket(void)
{
    str_bucket * tmp;
    
    if ((tmp = (str_bucket *)malloc(sizeof(struct _str_bucket))) == NULL)
        fprintf(stderr, "Memory error!  Cannot malloc\n");
    tmp->bucket = NULL;
    tmp->memlen = 0;
    tmp->length = 0;
    tmp->blocksize = STRB_SIZE;

    tmp->loc = tmp->bucket;

    return tmp;
}

void
destroy_str_bucket(b)
    str_bucket * b;
{
    if (0 != b->bucket)
        free(b->bucket);

    free(b);
}

int
strb_ready(b, space)
    str_bucket * b;
    int space;
{
    if (space == 0)
        space = STRB_SIZE;

    if (b->memlen > 0)
        free(b->bucket);
    if ((b->bucket = (char *)malloc(space * sizeof(char))) == NULL) {
        fprintf(stderr, "Memory error!  Unable to malloc\n");
        return -1;
    }
    b->loc = b->bucket;
    b->memlen = space*sizeof(char);
    b->length = 0;

    *(b->loc) = 0;

    return 0;
}

int
strb_set(b, instr)
    str_bucket * b;
    char *       instr;
{
    int len;
    
    if (b->bucket)
        free(b->bucket);

    len = strlen(instr);
    if ((b->bucket = (char *) malloc ((len + 1) * sizeof(char))) == NULL) {
        fprintf(stderr, "Memory error!  Sheesh, not able to malloc\n");
        return -1;
    }

    memcpy(b->bucket, instr, len*sizeof(char));
    
    b->length = len;
    b->memlen = len + 1;

    /* After this function, b->loc should /not/ point to the end of the
     string, but rather to the beginning: */
    b->loc    = b->bucket;
    
    b->bucket[b->length] = 0;

    if (b->length > 0 && b->bucket != NULL)
        return 0;
    else
        return -1;
}

/* Add a string to the bucket */
int
strb_puts(b, buf, len)
    str_bucket * b;
    char *       buf;
    size_t       len;
{
    size_t new_length, current_length, addition_length,
           new_size;

    if (0 == b || 0 == buf)
        return 1;

    current_length = 0;

    if (0 != b->bucket)
        current_length = b->length;

    /* According to the documentation, if zero is specified as length, we calculate it;
      otherwise, we assume the caller knows what they're doing. */

    if (0 == len)
        addition_length = strlen(buf);
    else
        addition_length = len;

    /* Onward, ho: */
    
    new_length = current_length + addition_length;

    /* Short-circuting optimization: If we already have enough room for the new
      input, then we do not allocate more. */

    if (new_length >= b->memlen) {

        /* We allocate the additional required space plus a blocksize-length pad: */

	new_size = (new_length + current_length + 1 + b->blocksize)*sizeof(char);
	    
        b->bucket = (char *)realloc(b->bucket, new_size);

        if (0 == b->bucket)
            return 1;

	/* Since we just changed the memory size, we'll naturally want to adjust this.
               IMPORTANT: Note that memlen is /not/ in memory units. */
	
	b->memlen = new_size;
     }

    /* Note that although strncat() may not always add a terminating NULL,
      we have already set the memory region to zero above: */

    memcpy(b->bucket + current_length, buf, addition_length*sizeof(char));

    /* Update the remaining counters: */

    b->length = new_length;                /* The string length. */
    b->loc    = b->bucket + new_length;    /* Points at EOS in this case. */

    *(b->loc) = 0;

    return 0;
}

/* Add a single character to the bucket */
int
strb_putc(b, ch)
    str_bucket * b;
    char         ch;
{
    char s[2];

    if(0 == b)
     return -1;

    s[0] = ch;
    s[1] = 0;

    if (0 != strb_puts(b, s, 1))
        return 0;
    
    return 1;
}

int
strb_concat(to, from)
    str_bucket * to;
    str_bucket * from;
{
    return (strb_puts(to, from->bucket, from->length));
}

void
strb_reset(b)
    str_bucket * b;
{
    if (b->memlen > 0)
        memset(b->bucket, 0, b->memlen);
    b->loc = b->bucket;
    b->length = 0;
}

char *
strb_strdup(b)
    str_bucket * b;
{
    char * buf;

    if(0 == b || 0 == b->bucket)
     return 0;

    buf = (char *)malloc(b->memlen*sizeof(char));

    if(0 == buf)
     return 0;
  
    memcpy(buf, b->bucket, b->length*sizeof(char));

    buf[b->length] = 0;
    
    return buf;
}

int
strb_getc(b, buf, max_len)
    str_bucket * b;
    char * buf;
    int    max_len;
{
    int i = 0;
    /* flex bug workaround?  It seems to puke on me if I return more
     * than 127 characters at a time. */
    int my_max = 127;

    if (my_max > max_len)
        my_max = max_len;

    /* printf("Called with max_len = %d, set my_max to %d\n", max_len, my_max); */

    while (i < my_max && *b->loc != '\0')
        buf[i++] = *(b->loc++);

    if (*b->loc == '\0' && i == 0) {
        buf[0] = EOF;
    }
    
    /* printf("And now i = %d\n", i);*/

    return i;
}

void
strb_ungetc(b, count)
    str_bucket * b;
    int          count;
{
    if (b->bucket > (b->loc - count))
        b->loc = b->bucket;
    else
        b->loc -= count;
}

int
strb_count(b, c)
    str_bucket * b;
    char         c;
{
    register char *p;
    register int  count = 0;

    p = b->bucket;
    while (*p != '\0')
        if (*(p++) == c)
            count++;

    return count;
}

/* Writing to a string bucket */
/* A sort of swanky sprintf for buckets. */
int
strb_vappendf(b, fmt, args)
    str_bucket * b;
    char *       fmt;
    va_list      args;
{
    int longvar,
        specifier_parse_stop;    /* Flag to stop parsing format specifiers. */
    
    char  * pos,
            tmp[STRB_MAX_VAL_DIGITS];

    if (0 == b || 0 == fmt)
        assert(0);

    for (pos = fmt; 0 != *pos; pos++) {

        switch (*pos) {
	   default:
   	       /* Not a special format character: */
		   
	       strb_putc(b, *pos);	
	       
	       break;
	      
	case '%':
	      /* A special formatting character, a'la printf(): 
	       *  Now we've got to get our format string. This loop terminates
	       * either when we reach the EOS, or a non-alphanumeric character. *
	      */

	    longvar = 0;
	 
	    specifier_parse_stop = 0;

	    while(!specifier_parse_stop)
	     {
   	        pos++;

		if (0 == *pos)
		 return -1;

                switch(*pos) {

 	            case '%':
			    
                       strb_putc(b, '%');

		       /* This is ugly, but we can't allow "%%fmt"-- we must require
			 that to be written "%%%fmt", or else we have ambiguity problems. */

		       specifier_parse_stop = 1;

		       break;

	            case 'b':
		        /* Append a bucket's contents: */
		        strb_concat(b, va_arg(args, str_bucket *)); 

			specifier_parse_stop = 1;

	  	        break;

	            case 'c':
			/* Append a single character: */
			
		        strb_putc(b, va_arg(args, int));

			specifier_parse_stop = 1;

		        break;

	            case 's':
			/* Append a string: */
		   	
		        strb_puts(b, va_arg(args, char *), 0);

		
			specifier_parse_stop = 1;

		        break;

	            case 'l':
			/* long-integer specifier: */
			
		        longvar = 1;
	  	        break;

	            case 'd':
	            case 'X':
			/* Decimal or hexidecimal integer: */
			
			tmp[0] = 0;

			switch(*pos)
			 {
			  case 'd':
				  	if (longvar)
				  	 snprintf(tmp, STRB_MAX_VAL_DIGITS, "%ld", va_arg(args, long));
					else
					 snprintf(tmp, STRB_MAX_VAL_DIGITS, "%d", va_arg(args, int));
					
					break;
					
			  case 'X':
					if(longvar)
					 snprintf(tmp, STRB_MAX_VAL_DIGITS, "%lX", va_arg(args, long));
					else
			 		 snprintf(tmp, STRB_MAX_VAL_DIGITS, "%X", va_arg(args, int));

					break;

			  default:
					/* Invalid format specifier: */

					return -1;

					break;
			 }

			strb_puts(b, tmp, strlen(tmp));

			/*  This is a terminal for the format specifier; we therefore
			   exit the loop: */


			specifier_parse_stop = 1;

		       break;

		    default:

		       assert(0);
		     
		       break;	
		} 
            } /* The end of our %-field loop. */
	} /* The end of our outer switch. */
    }

    /* The world is ok: */

    return 0;
}


int
strb_appendf(str_bucket * b, char * fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = strb_vappendf(b, fmt, args);
    va_end(args);

    return ret;
}

/* End of file */
