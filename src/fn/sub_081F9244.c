#include "global.h"
struct S { u8 pad[0xAA]; u8 st; u8 b0; u8 b1; u8 b2; u8 b3; u8 f1; u8 f2; u8 pad3[0xC4 - 0xB1]; u32 cnt; };
static inline u8 TakeFlag(struct S *p)
{
    if (p->f2) { p->f2 = 0; p->f1 = 0; return TRUE; }
    return FALSE;
}
void sub_081F9244(struct S *p)
{
    TakeFlag(p);
    p->cnt++;
}
