#include "engine/shared/global.h"
#include "link/rfu_manager/rfu_manager.h"
void RfuManager_RequestStopMode(void)
{
    gRfuLinkManager.state = 0x15;
}
