#include "global.h"
struct P { void (*fn)(void); u32 w4; u32 w[3]; u8 b14; u8 b15; u8 pad[2]; u8 obj[1]; };
s32 sub_08228DB8(void);
s32 sub_08197DD0(s32);
void sub_08217DB4(void *, u32);
void sub_08197E0C(void);
void sub_08197E74(struct P *o, u32 a, u32 b, u32 c)
{
    u32 z;
    s32 r;
    r = sub_08197DD0(sub_08228DB8());
    z = 0;
    o->b14 = r;
    o->b15 = r;
    o->w[0] = a;
    o->w[1] = b;
    o->w[2] = c;
    sub_08217DB4(o->obj, o->w[r]);
    o->fn = sub_08197E0C;
    o->w4 = z;
}
