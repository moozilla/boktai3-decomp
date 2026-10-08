#include "global.h"

extern u8 *gUnk_020004B4;

s32 sub_08109464(void)
{
    if (gUnk_020004B4 != NULL) {
        return gUnk_020004B4[0x1C];
    }
    return 0;
}
