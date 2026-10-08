#include "global.h"

extern u8 *gUnk_02000124;

s32 sub_08066C8C(u8 *p)
{
    if (gUnk_02000124 != 0 && p != 0 && p[0x31] != 0)
        return 0;
    return 1;
}
