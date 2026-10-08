#include "global.h"
struct S {
    u8 f0[0xE2C]; u16 e2c; u8 f1[4]; u16 e32; u8 f2[0x124];
    union { struct { u16 pad; u8 a:4; } x; struct { u16 pad; u16 a:4; u16 b:12; } y; } u; u16 f5c; u8 f3[6]; u8 f64; u8 f4[0x21]; u8 f86; u8 f87;
};
extern struct S *gUnk_020003C8;
static inline u32 orr(u32 m, u8 *a) { return m | *a; }
struct S *sub_081EA730(u32, u32);
void sub_081E4560(void) {
    struct S *p = gUnk_020003C8;
    u32 v;
    if (p == 0) {
        p = sub_081EA730(0xBB34, 0);
        if (p == 0) return;
    }
    v = 3;
    p->f5c = p->u.y.a;
    p->u.x.a = v;
    p->u.y.b = 1;
    if (p->f5c == 3 && p->u.x.a == 3) {
        p->f86 = p->e2c;
        p->f87 = p->e32;
    }
    { u8 k1 = 1; p->f64 = k1 | p->f64; }
}
