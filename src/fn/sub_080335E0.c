#include "global.h"
#include "ram.h"
/* TextWindowManager_IsCreated: Returns whether the text window manager singleton pointer is nonnull */
#define TextWindowManager_IsCreated sub_080335E0

u32 sub_080335E0(void)
{
    u32 r = (u32)gWindowManager;
    if (r)
        r = 1;
    return r;
}
