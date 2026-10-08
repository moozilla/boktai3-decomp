#include "global.h"
struct S { u8 a[0x78]; u32 w78; };
void sub_0824923C(struct S *, u32);
u32 sub_082374D4(struct S *p) { sub_0824923C(p, p->w78); return 0; }
