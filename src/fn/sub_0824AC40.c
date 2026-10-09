// COMPILER: old_agbcc
// CFLAGS: -O2
/* More subroutines needed by GCC output code on some machines.  */
/* Compile this one with gcc.  */
/* Copyright (C) 1989, 92-97, 1998 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* As a special exception, if you link this library with other files,
   some of which are compiled with GCC, to produce an executable,
   this library does not by itself cause the resulting executable
   to be covered by the GNU General Public License.
   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.  */


/* Specialized from pinned pret/agbcc libgcc/libgcc2.c. */
typedef long int ptrdiff_t;
typedef unsigned long int size_t;
typedef int wchar_t;
typedef unsigned int UQItype __attribute__ ((mode (QI)));
typedef int SItype __attribute__ ((mode (SI)));
typedef unsigned int USItype __attribute__ ((mode (SI)));
typedef int DItype __attribute__ ((mode (DI)));
typedef unsigned int UDItype __attribute__ ((mode (DI)));
typedef float SFtype __attribute__ ((mode (SF)));
typedef float DFtype __attribute__ ((mode (DF)));
typedef int word_type __attribute__ ((mode (__word__)));
struct DIstruct {SItype low, high;};
typedef union
{
  struct DIstruct s;
  DItype ll;
} DIunion;
extern DItype __fixunssfdi (SFtype a);
extern DItype __fixunsdfdi (DFtype a);
DItype
sub_0824AC40 (DItype u, word_type b)
{
  DIunion w;
  word_type bm;
  DIunion uu;
  if (b == 0)
    return u;
  uu.ll = u;
  bm = (sizeof (SItype) * 8) - b;
  if (bm <= 0)
    {
      w.s.high = 0;
      w.s.low = (USItype)uu.s.high >> -bm;
    }
  else
    {
      USItype carries = (USItype)uu.s.high << bm;
      w.s.high = (USItype)uu.s.high >> b;
      w.s.low = ((USItype)uu.s.low >> b) | carries;
    }
  return w.ll;
}
