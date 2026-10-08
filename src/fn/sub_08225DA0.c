#include "global.h"
struct In { u8 pad[8]; s16 f8; s16 fa; };
struct Q { u32 f0; struct In *f4; };
struct G { u8 pad[8]; s16 f8; s16 fa; };
extern struct G gUnk_030042A0;
s32 sub_0821ABA8(u32, u32);
struct Q *sub_0821A2C4(u32);
void sub_0821447C(u32, void (*)(void), void (*)(void), void (*)(void));
void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
void sub_08218254(void);
void sub_08214D20(void);
void sub_0821964C(void);
void sub_08225DA0(void)
{
    s32 r = sub_0821ABA8(0x76, 0);
    struct Q *p;
    if (r == 0) {
        p = sub_0821A2C4(0x56C2);
        sub_0821447C(0, sub_08217F4C, sub_08214B44, sub_08219624);
        if (p && p->f4) {
            gUnk_030042A0.f8 = p->f4->f8 >> 4;
            gUnk_030042A0.fa = p->f4->fa >> 4;
            return;
        }
    } else {
        sub_0821447C(r, sub_08218254, sub_08214D20, sub_0821964C);
    }
    gUnk_030042A0.f8 = 0;
    gUnk_030042A0.fa = 0;
}
