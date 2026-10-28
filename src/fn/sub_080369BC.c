#include "global.h"
#include "ram.h"

void Wireless_UpdateControllerTask(void);
void Wireless_UpdateController(void)
{
    if (gWirelessController)
        Wireless_UpdateControllerTask();
}
