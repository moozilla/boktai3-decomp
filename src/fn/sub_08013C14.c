#include "global.h"

extern u32 gUnk_02000470;
u32 ShadowRegistry_CreateTask(void);

u32 ShadowRegistry_GetTask(void)
{
    u32 r = gUnk_02000470;
    if (r == 0) {
        return ShadowRegistry_CreateTask();
    }
    return r;
}
