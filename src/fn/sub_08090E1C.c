#include "global.h"

void sub_0808FE64(void *);

static inline void Clr(u16 *q, u32 m)
{
    *q = m & *q;
}

static inline u8 Tst(u16 *m, u32 b)
{
    if (*m & b)
        return TRUE;
    return FALSE;
}

u32 sub_08090E1C(u8 *p)
{
    u32 m = 0x100;
    if (Tst((u16 *)(p + 0x2D6), m))
    {
        sub_0808FE64(p);
        Clr((u16 *)(p + 0x2D6), ~0x100);
    }
    return 1;
}
