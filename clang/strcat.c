/*
 * Synopsis:
 *      Fast, safe and simple string concatenator
 *  Usage:
 *	cc -I/usr/local
 *
 *	#include "jmscott/lib/clang/strcat.c"
 *
 *      buf[0] = 0
 *      jmscott_strcat(buf, sizeof buf, "hello, world");
 *      jmscott_strcat(buf, sizeof buf, ": ");
 *      jmscott_strcat(buf, sizeof buf, "good bye, cruel world");
 *  Note:
 *	Need to rename string.c or utf8.c, since UTF-8 code will appear soon.
 */

#ifndef JMSCOTT_CLANG_STRCAT
#define JMSCOTT_CLANG_STRCAT

void
jmscott_strcat(char *restrict tgt, int tgtsize, const char *restrict src)
{
        //  find null terminated end of target buffer
        while (*tgt++)
                --tgtsize;
        --tgt;

        //  copy non-null src bytes, leaving room for trailing null
        while (--tgtsize > 0 && *src)
                *tgt++ = *src++;

        // target always null terminated
        *tgt = 0;
}

#endif	//  define JMSCOTT_CLANG_STRCAT
