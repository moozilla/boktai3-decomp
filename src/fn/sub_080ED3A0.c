#include "global.h"

void sub_080766F8(void *, u32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag)
    {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Clr(u16 *q, u32 m)
{
    *q = m & *q;
}

void sub_080ED3A0(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
    {
        u16 *q;
        *(u8 *)(*(u8 **)(p + 4) + 7) = 2;
        q = (u16 *)(p + 0x13A);
        *q = (*q & 0xFFFB) | 8;
        Clr((u16 *)(p + 0x2C0), 0xFFFFF7FF);
    }
    sub_080766F8(p, 0xE);
}
