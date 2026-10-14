#include "global.h"

u8 *sub_08037DC0(void);

u32 sub_08039800(void)
{
    u8 *p = sub_08037DC0();
    if (p == NULL || p[0x28] != 7)
        return 0;
    return 1;
}
