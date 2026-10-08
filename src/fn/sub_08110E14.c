#include "global.h"
struct S { u8 f[0x18]; u32 *p; u8 g[0x20]; u32 a; u32 b; u32 c; u32 d; u32 e; };
void sub_08110C9C(void *);
s32 sub_08110E14(struct S *s)
{
    s->e = s->c;
    if (s->p != 0 && s->a != 1)
        s->c = *s->p;
    sub_08110C9C(s);
    return 0;
}
