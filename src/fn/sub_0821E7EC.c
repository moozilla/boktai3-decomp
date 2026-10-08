#include "global.h"
void sub_0821E748(u32); s32 sub_0821E76C(void); void sub_0821E780(u16 *, s32, u32);
s32 sub_0821E7EC(u16 *a, u32 b, u32 c) { s32 r; s32 x; sub_0821E748(b); x = sub_0821E76C(); if (x != 0) { sub_0821E780(a, x, c); r = 0; } else { a[0] = x; a[1] = x; a[2] = x; r = -1; } return r; }
