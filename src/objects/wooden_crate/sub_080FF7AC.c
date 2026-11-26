#include "global.h"

u8 WoodenCrate_HasFlags(u8 *s, u32 m)
{
    if (*(u16 *)(s + 0xC8) & m)
        return TRUE;
    return FALSE;
}
