#include "global.h"
/* TextState_SetChoiceProcedures: Copies choice procedure identifiers into the table at offset 0xC8 and sets the choice count */
#define TextState_SetChoiceProcedures sub_08032CEC

s32 sub_08032CEC(u8 *p, s32 count, u32 *src)
{
    if (count > 0) {
        u32 *s = src;
        u32 *dst = (u32 *)(p + 0xc8);
        s32 n = count;
        do {
            *dst++ = *s++;
        } while (--n != 0);
    }
    p[0xd] = count;
}
