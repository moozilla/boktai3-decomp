#include "global.h"
struct S { u8 f[0x24]; u32 a; };
void sub_081C0470(void *, void (*)(void *));
void sub_081C05A8(void *);
void sub_081C0564(struct S *p) { p->a = 0; sub_081C0470(p, sub_081C05A8); }
