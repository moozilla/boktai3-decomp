#include "global.h"
struct S { u8 f[0x1828]; u32 a; u32 pad; u32 b; u32 c; u32 d; u32 e; u32 f2; };
void sub_0811153C(struct S *s, u32 a, u32 b, u32 c, u32 d)
{
    s->c = a;
    s->d = b;
    s->e = c;
    s->b = d;
    s->a = 0;
    s->f2 = 0;
}
