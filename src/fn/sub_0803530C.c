#include "global.h"

void rfu_clearAllSlot(void);
void WirelessController_ResetSession(void *);
u32 Wireless_ResetControllerAndSlots(void *p)
{
    rfu_clearAllSlot();
    WirelessController_ResetSession(p);
}
