#include "global.h"

void Palette_CopyBank(u16 *dst, u16 *src)
{
    s32 i;
    for (i = 15; i >= 0; i--) {
        *dst = *src;
        dst++;
        src++;
    }
}
