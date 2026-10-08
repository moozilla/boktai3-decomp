#include "global.h"
struct E { u8 f[0x20]; u16 h; u8 p[0xe]; };
struct S { u8 f[0x1844]; u16 a; u16 pad; u16 b; u16 pad2; u8 c; };
extern struct E gUnk_03004C30[];
void sub_08111398(struct S *s)
{
    s->b += s->a;
    gUnk_03004C30[s->c].h = s->b >> 7;
}
