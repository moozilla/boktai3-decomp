#include "global.h"
#include "engine/ram.h"

s32 WindCannonManager_IsCreated(void)
{
    if (gFanCannonManager == NULL) {
        return 0;
    }
    return 1;
}
