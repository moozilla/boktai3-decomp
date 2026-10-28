#include "global.h"
#include "rfu_manager.h"
u32 rfu_REQBN_softReset_and_checkID(void);
void RfuManager_CompletePendingMscCycle(void);
u32 RfuManager_ResetAndCheckAdapter(void)
{
    u32 r = rfu_REQBN_softReset_and_checkID();
    struct RfuLinkManager *s;
    u32 c;
    if (r == 0x8001)
        gRfuLinkManager.RFU_powerOn_flag = 1;
    c = gRfuLinkManager.state;
    s = &gRfuLinkManager;
    if (c != 0x17 && c != 1) {
        s->next_state = 0;
        s->state = 0;
    }
    s->pcswitch_flag = 0;
    s->reserveDisconnectSlot_flag = 0;
    s->acceptCount = 0;
    s->acceptSlot_flag = 0;
    s->parent_child = 0xff;
    RfuManager_CompletePendingMscCycle();
    return r;
}
