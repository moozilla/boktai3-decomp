#include "global.h"

struct S { u8 f[0x9e]; u8 a; u8 b; u8 g[4]; u32 c; };

void sub_08012D08(struct S *s, u8 v)
{
    s->a = v;
    s->c = 0;
    s->b = 1;
}
