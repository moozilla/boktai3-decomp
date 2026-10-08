#include "global.h"
struct P { u8 f[0x10]; u16 st; u8 g[0xe4]; u16 c; };
void sub_0822B2F8(s32);
void sub_0804F7D0(struct P *, u32, void (*)(void));
void sub_0804FB48(void);
void sub_0804F91C(struct P *p)
{
    u16 *c = &p->c;
    u32 v;
    if (*c == 0) sub_0822B2F8(0x1a0);
    v = *c + 1;
    *c = v;
    if ((u16)v <= 7) {
        p->st = 4;
    } else if ((u16)v <= 0xf) {
        p->st = 3;
    } else {
        p->st = 2;
        sub_0822B2F8(0x1a0);
        sub_0804F7D0(p, 0, sub_0804FB48);
    }
}
