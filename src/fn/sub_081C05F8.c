#include "global.h"
struct S { u8 f[0x30]; u32 a; };
void sub_0824923C(void *, u32);
void sub_081C05F8(struct S *p) { sub_0824923C(p, p->a); }
