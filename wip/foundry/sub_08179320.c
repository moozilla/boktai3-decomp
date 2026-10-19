#include "global.h"

s32 Div(s32, s32);

struct Inner {
    u8 pad[0x1c0];
    u16 h[0x20];
};

struct Obj {
    u8 pad0[0x4c];
    struct Inner *in;
    s32 cur;
    u8 pad1[4];
    s32 **p58;
};

void sub_08179320(struct Obj *p)
{
    u16 buf[16];
    s32 idx;
    s32 i;
    s32 cur;
    u16 *dst = (u16 *)0x03005180;
    struct Inner *in;

    if (p->p58 == 0)
        idx = 0;
    else
        idx = Div(**p->p58, 0x898);
    cur = p->cur;
    if (cur > idx)
        idx = cur - 1;
    else if (cur < idx)
        idx = cur + 1;

    in = p->in;
    buf[0] = in->h[0];
    buf[14] = in->h[0xe];
    buf[15] = in->h[0xf];
    for (i = 0; i <= 12; i++) {
        if (i < idx)
            buf[i + 1] = in->h[i + 1];
        else
            buf[i + 1] = in->h[0xe];
    }
    CpuSet(buf, dst, 0x10);
    p->cur = idx;
}
