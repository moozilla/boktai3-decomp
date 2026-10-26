#include "global.h"
#include "ram.h"

u32 ShadowRegistry_CreateTask(void);

u32 ShadowRegistry_GetTask(void)
{
    u32 r = gShadowRegistry;
    if (r == 0) {
        return ShadowRegistry_CreateTask();
    }
    return r;
}
