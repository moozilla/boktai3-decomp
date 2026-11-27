#include "global.h"
#include "link/wireless/rfu_manager.h"

void RfuManager_NotifyEvent(u32, u32);

void RfuManager_RequestMscCycleCompletion(void)
{
    if (gRfuLinkManager.childClockSlave_flag == 0) {
        RfuManager_NotifyEvent(0x45, 0);
    } else if (gRfuLinkManager.childClockSlave_flag == 1) {
        gRfuLinkManager.childClockSlave_flag = 2;
    }
}
