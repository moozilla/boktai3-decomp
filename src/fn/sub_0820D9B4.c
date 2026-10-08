#include "global.h"
struct S { u8 a[7]; u8 b7; u8 c[0x30-8]; u32 w30; u32 w34; };
void sub_0820D9B4(struct S *p, u32 a, u32 b, u8 c) { p->w30 = a; p->w34 = b; p->b7 = c; }
