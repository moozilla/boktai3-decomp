#include "global.h"
#include "link/wireless/rfu_manager.h"
void RfuManager_RequestStopMode(void)
{
    gRfuLinkManager.state = 0x15;
}
