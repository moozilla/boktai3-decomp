#include "global.h"
struct S { u8 pad0[0x9c]; u32 w9c; u8 pad[0xAA - 0xa0]; u8 st; u8 b0; u8 b1; u8 b2; u8 b3; u8 f1; u8 f2; u8 pad3[0xC4 - 0xB1]; u32 cnt; u8 pad4[0xF04 - 0xC8]; u32 f04; };
static inline u8 TakeFlag(struct S *p)
{
    if (p->f2) { p->f2 = 0; p->f1 = 0; return TRUE; }
    return FALSE;
}
void sub_081F91D8(struct S *p)
{
    u32 m;
    u32 t;
    u8 *q = (u8 *)p;
    if (TakeFlag(p)) { u32 v = 0x12; p->st = v; }
    m = 1;
    t = p->w9c & m;
    if (t == 0) {
        *(u32 *)(q + 0xF04) = m;
        p->b1 = t; p->b2 = t; p->b3 = m; p->cnt = t; p->f2 = m;
    } else
        p->cnt++;
}
