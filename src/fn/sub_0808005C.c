#include "global.h"

void sub_08076034(void *, u32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag)
    {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_0808005C(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        sub_08076034(p, 0xB);
    if (*(u8 *)(p + 0x2B3))
        *(u8 *)(p + 0x2B0) = 1;
}
