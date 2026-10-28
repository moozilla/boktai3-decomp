#include "global.h"
#include "rfu_compat.h"
#include "rfu_manager.h"
#define G (&gRfuLinkManager)
struct RfuLinkManager;
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);

u8 sub_08224C08(u16 limit)
{
    if (gRfuLinkStatus->sendSlotNIFlag | gRfuLinkStatus->recvSlotNIFlag) {
        G->event.words.code = 6;
        RfuManager_NotifyEvent(0xf3, 1);
        return 6;
    }
    G->NI_failCounter_limit = limit;
    return 0;
}
