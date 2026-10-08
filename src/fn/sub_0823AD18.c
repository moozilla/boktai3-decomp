#include "global.h"
struct S { u8 p[0x41c]; u16 a; u8 q[6]; u16 c; u16 d; };
void sub_0823AD18(struct S *s) { u32 v = s->a * 10; s->d = v; s->c = v; }
