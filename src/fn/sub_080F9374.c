#include "global.h"

void sub_08076818(void *, u32);

// A bool8 (u8) result from an inline helper keeps the materialised 0/1 and
// the second test; an int result gets jump-threaded away.
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag)
    {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_080F9374(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        sub_08076818(p, 1);
}
