#include "global.h"
struct P { u8 f0[0xC72]; s8 sel; };
void sub_081662FC(u8 *, u8 *, s32);
s32 sub_081686E8(u8 *, s32, s32, s32, s32);
void sub_08168784(u8 *p)
{
    s32 i = 0;
    u8 *q = p + 0xC88;
    s8 *r = (s8 *)(p + 0xC78);
    for (; i <= 2; i++) {
        sub_081662FC(p, q, r[1]);
        *(u16 *)(q + 0x22) = sub_081686E8(p, i, ((struct P *)p)->sel, 3, 0);
        q += 0x60;
        r += 4;
    }
}
