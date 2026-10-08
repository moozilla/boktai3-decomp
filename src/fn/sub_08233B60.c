#include "global.h"
struct S { u8 p[0x1da]; u16 h; u32 a; };
void sub_08233B60(struct S *s, u32 v) { s->a = v; s->h = 0; }
