#include "palette_controller.h"

s32 sub_08002C70(void);
void sub_0821AD08(u32, u32);
void sub_08002CD8(struct PaletteTransition *t)
{
    s32 d;
    s32 a;
    u32 v;
    u32 *q;
    u32 u;
    s32 f1 = sub_08002C70();
    u = t->brightness;
    d = u - f1;
    if (d < 0)
        d = -d;
    if ((s32)u < sub_08002C70()) {
        a = d;
        if (a > 8)
            a = 8;
        v = t->brightness + a;
    } else {
        u = t->brightness;
        if ((s32)u > sub_08002C70()) {
            a = d;
            if (a > 8)
                a = 8;
            v = t->brightness - a;
        } else {
            v = sub_08002C70();
        }
    }
    t->brightness = v;
    q = &t->callback;
    if (*q != 0) {
        sub_0821AD08(*q, 0);
        *q = 0;
    }
}
