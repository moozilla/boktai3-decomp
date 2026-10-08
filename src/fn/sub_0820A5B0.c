#include "global.h"

extern u8 *gUnk_02000600;

s32 sub_0820A5B0(void)
{
    if (gUnk_02000600 == NULL) {
        return 0;
    }
    return 1;
}
