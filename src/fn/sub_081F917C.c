#include "global.h"
struct S { u8 pad[0xAA]; u8 st; u8 b0; u8 b1; u8 b2; u8 b3; u8 f1; u8 f2; u8 pad3[0xC4 - 0xB1]; u32 cnt; };
static inline u8 TakeFlag(struct S *p)
{
    if (p->f2) { p->f2 = 0; p->f1 = 0; return TRUE; }
    return FALSE;
}
void sub_081F917C(struct S *p)
{
    u32 *c;
    u8 t;
    if (TakeFlag(p)) { u32 v = 0x11; p->st = v; }
    t = p->f1;
    c = &p->cnt;
    if (t) { u32 z = 0, o = 1; p->b1 = z; p->b2 = z; p->b3 = o; *c = z; p->f2 = o; }
    (*c)++;
}
