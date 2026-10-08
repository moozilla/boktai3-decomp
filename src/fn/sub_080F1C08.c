#include "global.h"

struct P { u8 f[0xcc5]; u8 a; u8 b; u8 g[0xcf0 - 0xcc7]; void (*cb)(void); u8 h[0xd32 - 0xcf4]; u16 v; };
void sub_080F1C48(void);
s32 sub_080F1C08(struct P *p)
{
    u16 *s = &p->v;
    p->a = (*s >> 5) & 0x1f;
    p->b = (*s >> 10) & 0x1f;
    p->cb = sub_080F1C48;
}