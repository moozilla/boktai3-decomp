#include "global.h"
extern u8 *gUnk_02000710;
extern u16 gUnk_03004BD8;
extern u32 gUnk_030053F0;
extern u32 gUnk_030053F4;
extern int gUnk_02000080, gUnk_02000084;
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
s32 Mod(s32, s32);
void sub_08003448(u32 v);
int sub_08017468(int, int);
void sub_080174B8(void *, int);
s32 sub_080174C4(s16 *p);

struct S {
    u8 pad[0x18];
    s32 f18;
    u8 f1c;
    u8 pad2[3];
    u8 f20;
    u8 f21;
};

static inline u32 tst(u32 *a, u32 *b, u32 m) { return (*a | *b) & m; }

u32 sub_08017A6C(struct S *p)
{
    s32 r;
    s32 v;
    u8 *b;
    if (p->f1c != 0) {
        p->f1c = 0;
        *(u32 *)(gUnk_02000710 + 0x868) &= ~0x1000;
        sub_08003448(0);
        gUnk_03004BD8 &= ~0x200;
    }
    if (sub_08017468(p->f21, 2) == 0) {
        r = sub_080174C4((s16 *)p);
        b = gUnk_02000710;
        v = *(s16 *)(b + 0x876);
        if (tst(&gUnk_030053F4, &gUnk_030053F0, 0x400) == 0 && p->f20 != 0) {
            if (v > 0) {
                *(s32 *)(b + 0x630) += v >> 2;
                if (*(s32 *)(gUnk_02000710 + 0x630) >= gUnk_02000080)
                    *(s32 *)(gUnk_02000710 + 0x630) = gUnk_02000080;
            } else if (*(s32 *)(b + 0x630) == 0) {
                gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
                if ((gUnk_0203B400[gUnk_03005308] & 0x3f) >= r) {
                    *(s32 *)(b + 0x630) = gUnk_02000080;
                    if (gUnk_02000084 > 0) {
                        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
                        *(s32 *)(gUnk_02000710 + 0x630) += Mod(gUnk_0203B400[gUnk_03005308], gUnk_02000084);
                    }
                } else {
                    sub_080174B8(p, 1);
                    return 1;
                }
            } else {
                *(s32 *)(b + 0x630) -= 1;
            }
        }
    }
    p->f18++;
    return 0;
}
