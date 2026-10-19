#include "global.h"

struct E {
    u32 pad0;
    u8 state, entered, pad6, mode;
    u32 pad8, timer;
    u8 pad10[8];
    u16 px, py;
    u8 pad1c[0x2c - 0x1c];
    u32 flags;
    u8 pad30[0x44 - 0x30];
    u16 a, b;
};

void FloatingEffect_Project(struct E *);

void sub_08016AA0(void *m, struct E *p)
{
    if (p->entered) {
        p->entered = 0;
        p->flags &= ~1;
    }
    if (p->mode == 3) {
        if (p->timer <= 3)
            p->py += 0x40;
        else
            p->py -= 4;
    } else {
        if (p->timer <= 4)
            p->py += 0x40;
        else
            p->py -= 8;
    }
    FloatingEffect_Project(p);
    if (p->mode == 3) {
        if (p->timer > 3) {
            u32 *r = (u32 *)0x03005308;
            u16 *tbl = (u16 *)0x0203B400;
            u32 n, m2, x, y;
            n = (*r + 1) & 0x3FF;
            *r = n;
            x = tbl[n] & 3;
            p->a = p->a - 1 + x;
            m2 = (n + 1) & 0x3FF;
            *r = m2;
            y = tbl[m2] & 3;
            p->b = p->b - 1 + y;
        }
    }
    if (p->mode == 3) {
        if (p->timer <= 8)
            goto inc;
        goto done;
    }
    if (p->timer <= 7)
        goto inc;
done:
    {
        u32 four = 4;
        u32 zero = 0;
        p->state = four;
        p->entered = 1;
        p->timer = zero;
    }
    return;
inc:
    p->timer++;
}
