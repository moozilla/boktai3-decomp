#include "global.h"
extern u32 gUnk_030053F4;
struct S {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk(struct S *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void sub_080247AC(struct S *p)
{
    struct S *p2 = p;
    if (chk(p)) {
        u32 k = 0x10;
        p->aa = k;
    }
    if (p->af) {
        u32 z2 = 0;
        p2->ac = z2;
        p2->ad = z2;
        p2->ae = z2;
        p2->c4 = z2;
        p2->b0 = 1;
        if (gUnk_030053F4 & 0x1000)
            p2->h356 = z2;
        else
            p2->h356 = 0x5a;
    } else
        p->c4++;
}
