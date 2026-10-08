#include "global.h"
struct P { u8 pad; u8 b1; u8 pad1[0x20a]; u32 w20c; };
s32 sub_08002C70(void);
void sub_0821AD08(u32, u32);
void sub_08002CD8(struct P *p)
{
    s32 d;
    s32 a;
    u32 v;
    u32 *q;
    u32 t;
    s32 f1 = sub_08002C70();
    t = p->b1;
    d = t - f1;
    if (d < 0) d = -d;
    if ((s32)t < sub_08002C70()) {
        a = d;
        if (a > 8) a = 8;
        v = p->b1 + a;
    } else {
        t = p->b1;
        if ((s32)t > sub_08002C70()) {
            a = d;
            if (a > 8) a = 8;
            v = p->b1 - a;
        } else {
            v = sub_08002C70();
        }
    }
    p->b1 = v;
    q = &p->w20c;
    if (*q != 0) {
        sub_0821AD08(*q, 0);
        *q = 0;
    }
}
