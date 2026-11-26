#include "global.h"
#include "engine/ram.h"
s32 WirelessController_IsCreated(void)
{
    if (gWirelessController)
        return 1;
    return 0;
}
