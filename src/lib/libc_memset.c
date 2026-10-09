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
/* Adapted from pinned pret/agbcc libc/string/memset.c.
 * This software was developed at Cygnus Solutions. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define memset sub_0824DAA8
typedef unsigned long size_t;
void *
memset(void * m , int c , size_t n)



{

  char *s = (char *) m;
  int count, i;
  unsigned long buffer;
  unsigned long *aligned_addr;
  unsigned char *unaligned_addr;

  if (!((n) < (sizeof(long))) && !((long)m & ((sizeof(long)) - 1)))
    {


      aligned_addr = (unsigned long*)m;



      c &= 0xff;
      if ((sizeof(long)) == 4)
        {
          buffer = (c << 8) | c;
          buffer |= (buffer << 16);
        }
      else
        {
          buffer = 0;
          for (i = 0; i < (sizeof(long)); i++)
     buffer = (buffer << 8) | c;
        }

      while (n >= (sizeof(long))*4)
        {
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          n -= 4*(sizeof(long));
        }

      while (n >= (sizeof(long)))
        {
          *aligned_addr++ = buffer;
          n -= (sizeof(long));
        }

      s = (char*)aligned_addr;
    }

  while (n--)
    {
      *s++ = (char)c;
    }

  return m;

}
