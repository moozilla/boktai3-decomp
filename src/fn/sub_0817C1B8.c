#include "global.h"
struct S {
    u8 pad0[0x9c];
    u32 w9c;
    u8 pad[0xAA - 0xa0];
    u8 st;
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
    u8 f1;
    u8 f2;
    u8 pad3[0xC4 - 0xB1];
    u32 cnt;
};
static inline u8 TakeFlag(struct S *p)
{
    if (p->f2) {
        p->f2 = 0;
        p->f1 = 0;
        return TRUE;
    }
    return FALSE;
}
void sub_0817C1B8(struct S *p)
{
    u32 m;
    u32 t;
    if (TakeFlag(p)) {
        u32 v = 0xc;
        p->st = v;
    }
    m = 1;
    t = p->w9c & m;
    if (t == 0) {
        u32 e = 0xe;
        p->b1 = t;
        p->b2 = t;
        p->b3 = e;
        p->cnt = t;
        p->f2 = m;
        p->st = m;
    } else
        p->cnt++;
}
