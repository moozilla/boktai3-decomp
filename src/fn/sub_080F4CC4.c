#include "global.h"

struct P { u8 f[0x1c]; u16 a, b, c; u8 g[0x13c - 0x22]; u16 d, e, h; u8 i[0x174 - 0x142]; void (*cb)(void); u8 j[0x18d - 0x178]; u8 z; };
void sub_080F4CFC(void);

void sub_080F4CC4(struct P *p)
{
    p->z = 0;
    p->cb = sub_080F4CFC;
    p->d = p->a;
    p->e = p->b;
    p->h = p->c;
}
