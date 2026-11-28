#include "global.h"
#include "engine/ram.h"
/* TextWindowManager_IsCreated: Returns whether the text window manager singleton pointer is nonnull */

u32 TextWindowManager_IsCreated(void)
{
    return gWindowManager != 0;
}
