#include "global.h"
struct S { u8 p[0x202]; u16 h; u32 a; };
void sub_082343A0(struct S *s, u32 v) { s->a = v; s->h = 0; }
