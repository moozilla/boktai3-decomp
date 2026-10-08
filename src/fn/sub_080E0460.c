#include "global.h"
void sub_08076034(void *, u32);
static inline u32 orr(u32 k, u16 *a) { return k | *a; }
struct S { u8 f0[0x2b0]; u8 b0; u8 f1; u8 b2; u8 b3; u8 f2[0x20]; u16 h4; u16 h6; };
static inline u8 tk(u8 *a) {
    if (*a) { *a = 0; return 1; }
    return 0;
}
void sub_080E0460(struct S *p, s32 n) {
    if (tk(&p->b2)) {
        s32 m;
        u16 *a;
        sub_08076034(p, 7);
        a = &p->h4;
        m = -2;
        *a = m & *a;
    }
    if (n == 0x28) {
        u16 k = 0x10;
        p->h6 = k | p->h6;
    }
    if (p->b3 && n > 0x78) p->b0 = 1;
}
