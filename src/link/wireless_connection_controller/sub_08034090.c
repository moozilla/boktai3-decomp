#include "engine/shared/global.h"
#include "engine/shared/ram.h"
s32 WirelessController_IsCreated(void)
{
    if (gWirelessController)
        return 1;
    return 0;
}
