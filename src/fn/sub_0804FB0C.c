#include "global.h"
struct P { u8 f[0x10]; u16 f10; u8 f12[0xe4]; u16 ff6; };
void sub_0804F7D0(struct P *, u32, void (*)(void));
void sub_0804FB48(void);
void sub_0804FB0C(struct P *p)
{
    u16 *c = &p->ff6;
    u32 v = *c + 1;
    *c = v;
    if ((u16)v <= 7) {
        p->f10 = 7;
    } else if ((u16)v <= 0xf) {
        p->f10 = 6;
    } else {
        p->f10 = 2;
        sub_0804F7D0(p, 0, sub_0804FB48);
    }
}
