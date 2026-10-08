#include "global.h"
struct S { u8 p[0x2c]; u32 x; };
extern u16 gUnk_030054BC;
void sub_0824923C(struct S *, u32);
u32 sub_0822CA28(struct S *s) { if (gUnk_030054BC == 0) sub_0824923C(s, s->x); return 0; }
