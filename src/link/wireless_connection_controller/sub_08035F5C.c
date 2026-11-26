#include "engine/shared/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"
#include "engine/shared/ram.h"
void Wireless_SetStatus(int);
int Wireless_TryStartParentNiSend(WirelessController *p)
{
    int index;
    if (p->niFlags & 2)
        goto failed;
    if (gRfuLinkStatus->linkLossSlotFlag)
        goto failed;
    if (!(p->niFlags & 1))
        goto ready;
    rfu_clearSlot(TYPE_NI_SEND, p->sendSlot);
    if (rfu_NI_setSendData((u8)(1 << p->sendSlot), 15, p->niSend, sizeof(p->niSend)))
        goto sendError;
    for (index = 0; index < RFU_CHILD_MAX; index++) {
        if (gRfuLinkStatus->connSlotFlag & (1 << index))
            break;
    }
    p->sendSlot = index;
    p->niFlags = (p->niFlags & 0xFFFE) | 2;
    goto ready;
sendError:
    Wireless_SetStatus(2);
failed:
    return 0;
ready:
    return 1;
}
