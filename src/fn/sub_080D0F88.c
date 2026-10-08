#include "global.h"

void sub_0807C72C(void *, u32, u32, u32);
u32 sub_08076034(void *, u32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag)
    {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_080D0F88(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        sub_0807C72C(p, 0, 0x7F, 0);
    sub_08076034(p, 1);
}
