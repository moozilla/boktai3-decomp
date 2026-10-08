#include "global.h"
struct P { u8 f0[0x33C]; u16 a; s16 b; u8 x; u8 c; u8 e; u8 f; u8 g[2]; u16 d; };
extern u32 gUnk_030053F4;
extern u32 gUnk_030053D8;
void sub_081630E0(struct P *);
void sub_08224F70(s32);
void sub_0821A0C0(struct P *);
static inline void andm(u32 *a, u32 m) { *a &= m; }
void sub_08163654(struct P *p)
{
    sub_081630E0(p);
    if (p->c == 8) {
        andm(&gUnk_030053F4, ~0x400);
        gUnk_030053D8 = 0;
        sub_08224F70(p->f);
        sub_0821A0C0(p);
    }
}
