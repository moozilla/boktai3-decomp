#include "global.h"
void sub_080542A4(u8 *);
void sub_08054294(u8 *, void (*)(void));
void sub_08054394(void);
void sub_08054354(u8 *p)
{
    if (*(u16 *)(p + 0xF8) >= *(u16 *)(p + 0xFA)) {
        u16 *q;
        u32 v;
        u32 m;
        p[0xFE] = 1;
        sub_080542A4(p);
        q = (u16 *)(p + 0x4E);
        v = *q;
        m = 4;
        m |= v;
        *q = m;
        sub_08054294(p, sub_08054394);
    }
}
