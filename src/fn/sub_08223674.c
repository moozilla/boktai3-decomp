#include "global.h"
#include "rfu_manager.h"
void RfuManager_ResetConnectionState(void)
{
    u8 i;
    gRfuLinkManager.next_state = 0;
    gRfuLinkManager.state = 0;
    gRfuLinkManager.parent_child = 0xff;
    gRfuLinkManager.pcswitch_flag = 0;
    gRfuLinkManager.child_slot = 0;
    gRfuLinkManager.connectSlot_flag_old = 0;
    gRfuLinkManager.nameAcceptTimer.active = 0;
    gRfuLinkManager.linkRecoveryTimer.active = 0;
    for (i = 0; i < 4; i++) {
        gRfuLinkManager.nameAcceptTimer.count[i] = 0;
        gRfuLinkManager.linkRecoveryTimer.count[i] = 0;
    }
}
