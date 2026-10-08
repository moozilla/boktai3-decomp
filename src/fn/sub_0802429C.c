#include "global.h"
extern u32 gUnk_030053F4;
struct S { u8 f0[0x9a]; u8 a9a; u8 f9[0xf]; u8 aa; u8 f1; u8 ac; u8 ad; u8 ae; u8 af; u8 b0; u8 f2[0x13]; u32 c4; u8 f3[0x28e]; u16 h356; };

static inline u8 chk(struct S *p, u32 z) {
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

void sub_0802429C(struct S *p) {
    struct S *p2 = p;
    u32 z;
    u8 *a = &p->a9a;
    z = 0;
    *a = 1;
    if (chk(p, z)) { u32 k = 7; p->aa = k; }
    if (p->af) {
        u32 z2 = 0;
        p2->ac = z2;
        p2->ad = z2;
        p2->ae = z2;
        p2->c4 = z2;
        p2->b0 = 1;
        if (gUnk_030053F4 & 0x1000) p2->h356 = z2;
        else p2->h356 = 0x5a;
    } else p->c4++;
}
