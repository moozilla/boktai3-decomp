#include "engine/shared/global.h"
#include "engine/shared/ram.h"

s32 FanCannonManager_IsCreated(void)
{
    if (gFanCannonManager == NULL) {
        return 0;
    }
    return 1;
}
