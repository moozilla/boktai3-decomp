#include "global.h"

u8 sub_080FF7AC(u8 *s, u32 m)
{
    if (*(u16 *)(s + 0xC8) & m)
        return TRUE;
    return FALSE;
}
