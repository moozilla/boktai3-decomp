#include "global.h"
#include "rfu_manager.h"
void RfuManager_RequestStopMode(void)
{
    gRfuLinkManager.state = 0x15;
}
