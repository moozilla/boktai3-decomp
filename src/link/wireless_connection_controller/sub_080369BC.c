#include "engine/shared/global.h"
#include "engine/shared/ram.h"

void Wireless_UpdateControllerTask(void *);
void Wireless_UpdateController(void)
{
    if (gWirelessController)
        Wireless_UpdateControllerTask(gWirelessController);
}
