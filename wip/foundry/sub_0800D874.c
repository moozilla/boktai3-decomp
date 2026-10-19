#include "global.h"

struct Wav {
    u8 pad0[0x54];
    u16 buf[1];
    u8 pad1[0x2D4 - 0x56];
    u16 row;
    s16 amp;
    u16 delta;
    u16 start;
    u16 end;
};

#define BUF(p, i) (*(u16 *)((u8 *)(p)->buf + (p)->row * 320 + (i) * 2))

void sub_0800D874(struct Wav *p)
{
    s32 i;
    s32 z;

    i = 0;
    z = i;
    while (i < p->start) {
        BUF(p, i) = z;
        i++;
    }
    i = p->start;
    {
        s16 *amp = &p->amp;
        u16 *delta = &p->delta;
        u16 *tbl = (u16 *)0x0203B400;
        u32 *r = (u32 *)0x03005308;
        while (i < p->end) {
            u32 n;
            u32 v;
            n = (*r + 1) & 0x3FF;
            *r = n;
            v = tbl[n] * *amp >> 18;
            BUF(p, i) = v | v << 8;
            i++;
        }
        i = p->end;
        while (i <= 0x9f) {
            BUF(p, i) = 0;
            i++;
        }
        *amp += *delta;
        if (*amp < 0)
            *amp = 0;
        else if (*amp > 0x80)
            *amp = 0x80;
    }
}
