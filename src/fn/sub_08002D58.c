#include "global.h"

void sub_08002AE0(void *, u32, u32, u32, u32, u32);
struct S { u8 f[0x1c]; u8 v; u8 g[0xA8C-0x1d]; u8 buf[1]; u8 h[0xE90-0xA8D]; void *ptr; };
struct A { u8 a; u8 pad[3]; u32 b; u32 c; };

void sub_08002D58(struct S *p, struct A *a)
{
    void *buf = p->buf;
    u32 b = a->b;
    u32 c = a->c;
    sub_08002AE0(buf, b, c, a->a, 0, 0xd0);
    p->ptr = buf;
    p->v = 1;
}
