#include "global.h"
/* Actor_SetActionParameters: Tentatively stores an action selector and copies its parameter words into the actor */
#define Actor_SetActionParameters sub_0802CCE0

void sub_0802CCE0(u8 *p, u32 value, s32 count, u32 *src)
{
    *(u32 *)(p + 0x16c) = value;
    if (count > 0) {
        u32 *dst = (u32 *)(p + 0x170);
        do {
            *dst++ = *src++;
        } while (--count != 0);
    }
}
