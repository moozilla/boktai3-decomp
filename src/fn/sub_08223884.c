#include "global.h"
#include "rfu_compat.h"
#include "rfu_manager.h"
#define G (&gRfuLinkManager)
struct RfuLinkManager;
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);

void sub_08223884(u8 bm)
{
    u8 i;
    if ((bm & G->linkRecoveryTimer.active) == 0)
        return;
    G->linkRecoveryTimer.active &= ~bm;
    for (i = 0; i < 4; i++) {
        if ((bm >> i) & 1)
            G->linkRecoveryTimer.count[i] = 0;
    }
    i = gRfuLinkStatus->linkLossSlotFlag & bm;
    if (i)
        RfuManager_DisconnectAndWait(i);
    G->event.words.code = i;
    RfuManager_NotifyEvent(0x33, i);
}
