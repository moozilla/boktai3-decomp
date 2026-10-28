#include "global.h"
#include "ram.h"
s32 WirelessController_IsCreated(void)
{
    if (gWirelessController)
        return 1;
    return 0;
}
