#include "global.h"
#include "rfu_manager.h"

void RfuManager_NotifyEvent(u32, u32);

void RfuManager_CompletePendingMscCycle(void)
{
    if (gRfuLinkManager.childClockSlave_flag != 0) {
        gRfuLinkManager.childClockSlave_flag = 0;
        RfuManager_NotifyEvent(0x45, 0);
    }
}
