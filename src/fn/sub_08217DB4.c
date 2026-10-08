#include "global.h"

void sub_08217DB4(u16 *dst, u16 *src)
{
    s32 i;
    for (i = 15; i >= 0; i--) {
        *dst = *src;
        dst++;
        src++;
    }
}
