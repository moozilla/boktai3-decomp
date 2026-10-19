#include "global.h"

typedef struct { u16 x; u16 px; u16 y; u16 py; } Pt;

void sub_081263E0(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

#define M 0x3FF
#define IDX (*(u32 *)0x03005308)
#define RND() (tab[IDX = (IDX + 1) & M] & 0x3F)
#define DRAW(n) sub_081263E0(&pt, 8, 8, 4, n, zero, zero, 0x20, 0x40, 1, 1, 0x200)
#define TWO(sx, sy, n) \
    pt = *src; \
    { int t = pt.x sx 0x5c; pt.x = t - RND(); } \
    { int t = pt.y sy 0x5c; pt.y = t - RND(); } \
    DRAW(n);
#define ONE(sx, n) \
    pt = *src; \
    { int t = pt.x sx 0x5c; pt.x = t - RND(); } \
    DRAW(n);

void sub_08107398(u8 *p)
{
    Pt pt;
    Pt *src = (Pt *)(p + 0x84);
    u16 *tab = (u16 *)0x0203B400;
    u32 zero = 0;
    if (*(u32 *)0x030053F4 & 0x800) return;
    {
        int v = *(u16 *)(p + 0xb8);
        if (v > 1 || v < 0) return;
    }
    TWO(+, +, 4)
    TWO(-, -, 4)
    TWO(-, +, 4)
    TWO(+, -, 2)
    ONE(-, 2)
    ONE(+, 4)
}
