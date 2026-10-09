/*
Copyright (c) 1994, 1997 Cygnus Solutions.
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
at Cygnus Solutions.  Cygnus Solutions may not be used to
endorse or promote products derived from this software without
specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/
/* Adapted from pinned pret/agbcc libc/string/memcpy.c.
 * This software was developed at Cygnus Solutions. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define memcpy sub_0824DA48
typedef unsigned long size_t;
void *
memcpy(void * dst0 , const void * src0 , size_t len0)



{

  char *dst = dst0;
  const char *src = src0;
  long *aligned_dst;
  const long *aligned_src;
  int len = len0;



  if (!((len) < (sizeof (long) << 2)) && !(((long)src & (sizeof (long) - 1)) | ((long)dst & (sizeof (long) - 1))))
    {
      aligned_dst = (long*)dst;
      aligned_src = (long*)src;


      while (len >= (sizeof (long) << 2))
        {
          *aligned_dst++ = *aligned_src++;
          *aligned_dst++ = *aligned_src++;
          *aligned_dst++ = *aligned_src++;
          *aligned_dst++ = *aligned_src++;
          len -= (sizeof (long) << 2);
        }


      while (len >= (sizeof (long)))
        {
          *aligned_dst++ = *aligned_src++;
          len -= (sizeof (long));
        }


      dst = (char*)aligned_dst;
      src = (char*)aligned_src;
    }

  while (len--)
    *dst++ = *src++;

  return dst0;

}
