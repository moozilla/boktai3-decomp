#include "global.h"
struct S { u8 b0; u8 b1; u16 h2; u8 p[0xc]; u32 w; };
void sub_08231788(struct S *s) { s->b0 = 0; s->h2 = 0; s->b1 = 10; s->w |= 1; }
