#include "global.h"
u32 sub_0824923C(void *, u32);
void sub_080F2BE4(void);
struct S2B94 {
    u8 f0[0xCDC];
    void (*fp)(void);
    u8 f1[0xCF0 - 0xCE0];
    u32 v;
    u8 f2[0xD53 - 0xCF4];
    s8 b;
};
void sub_080F2B94(struct S2B94 *s) {
    u32 v = s->v;
    if (v != 0) {
        if ((u8)sub_0824923C(s, v) != 0)
            s->b++;
    }
    if (s->b > 1) {
        s->b = 0;
        s->fp = sub_080F2BE4;
    }
}
