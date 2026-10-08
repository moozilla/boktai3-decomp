#include "global.h"

extern u8 *gUnk_020004B8;

s32 sub_08110B7C(void)
{
    if (gUnk_020004B8 == NULL) {
        return 0;
    }
    return 1;
}
