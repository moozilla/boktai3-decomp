#include "global.h"
struct P { u32 w0; u32 w4; u8 f[0x154 - 8]; u8 a; u8 pad; u8 b; };
void sub_08220D78(void *, void *, s32, s32, s32);
void sub_08013728(void *);
void sub_0824923C(void *, u32);
void sub_081980FC(struct P *p)
{
    u8 *q = &p->a;
    u32 v = *q;
    if (v == 0) {
        sub_08220D78((u8 *)p + 0x28, (u8 *)p + 8, 6, 2, v);
        sub_08013728((u8 *)p + 0x88);
        (*q)++;
    } else if (p->b != 0) {
        u32 z = 0;
        p->w0 = z;
        if (p->w4 != 0) {
            sub_0824923C(p, p->w4);
            p->w4 = z;
        }
    }
}
