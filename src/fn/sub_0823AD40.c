#include "global.h"
struct S { u8 p[0x41e]; u16 a; u8 q[8]; u16 c; u16 d; };
void sub_0823AD40(struct S *s) { u32 v = s->a * 5 + 100; s->d = v; s->c = v; }
