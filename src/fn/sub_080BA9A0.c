#include "global.h"
void sub_08076034(void *, u32);
void sub_080BA0EC(void);
static inline u8 tk(u8 *a) {
    if (*a) { *a = 0; return 1; }
    return 0;
}
struct S {
    u8 f1[0x104];
    u32 w104; u8 f2[0xc]; u32 w114; u8 f3[0x104]; void (*fn)(void);
    u8 f4[0x90]; u8 b2b0; u8 f5; u8 b2b2; u8 b2b3; u8 f6; u8 b2b5; u8 f7[0x12];
    u16 h2c8;
};
void sub_080BA9A0(struct S *p) {
    if (tk(&p->b2b2)) sub_08076034(p, 0x16);
    if (p->b2b3) {
        void (*f)(void) = sub_080BA0EC;
        u32 k = 0x3e;
        u32 z;
        u8 *a = &p->b2b2;
        u32 *w;
        s32 m;
        z = 0;
        *a = 1;
        p->b2b0 = z;
        p->b2b5 = k;
        p->fn = f;
        p->h2c8 = z;
        w = &p->w114;
        m = -2;
        *w = m & *w;
        p->w104 = z;
    }
}
