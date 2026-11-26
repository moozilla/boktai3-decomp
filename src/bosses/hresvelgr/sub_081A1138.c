#include "global.h"

struct S {
    u8 pad0[0x9c];
    u32 f9c;
    u8 pad1[0xb1 - 0xa0];
    u8 fb1;
    u8 pad2[0x31c - 0xb2];
    u16 h[4];
};

static inline void clr(struct S *p)
{
    if (p->fb1 != 0)
        p->fb1 = 0;
}

static inline void setf(struct S *p, u32 m)
{
    p->f9c |= m;
}

void Hresvelgr_OnActionTwentyEight(struct S *p)
{
    clr(p);
    p->h[0] = 0;
    p->h[1] = 0;
    p->h[2] = 0;
    p->h[3] = 0;
    setf(p, 0x1c);
}
