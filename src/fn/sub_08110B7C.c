#include "global.h"

extern u8 *gUnk_020004B8;

s32 SceneTilemap_IsActive(void)
{
    if (gUnk_020004B8 == NULL) {
        return 0;
    }
    return 1;
}
