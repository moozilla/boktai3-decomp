#include "global.h"
struct S { u32 w0; u32 w4; u8 a[0x22]; u16 h2a; };
u32 sub_0820D23C(struct S *p) { u32 m = 2; p->w4 |= m; if (p->w0 > p->h2a) return 1; return 0; }
