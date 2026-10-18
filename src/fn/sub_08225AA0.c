#include "global.h"
extern u32 *gUnk_030025F8;
u32 *SceneBody_GetRegistry(void)
{
    if (gUnk_030025F8 == 0)
        return 0;
    return gUnk_030025F8;
}
