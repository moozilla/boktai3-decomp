#include "global.h"

extern u8 *gUnk_020004B4;

s32 Registry1745_GetSessionState(void)
{
    if (gUnk_020004B4 != NULL) {
        return gUnk_020004B4[0x1C];
    }
    return 0;
}
