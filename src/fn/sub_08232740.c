#include "global.h"
struct S { u8 p[1]; u8 b1; u8 b2; u8 q[3]; u16 h; u16 g; };
void sub_08232740(struct S *s) { if (s->h++ > s->g) { u32 o = 1; u32 z = 0; s->b2 = o; s->h = z; s->b1 = o; } }
