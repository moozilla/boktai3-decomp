#include "wireless_controller.h"
#include "rfu_compat.h"
#include "ram.h"
void Wireless_SetStatus(int);
int Wireless_TryStartParentNiSend(WirelessController *p)
{
    int flags = p->niFlags;
    struct RfuLinkStatus **links;
    int one;
    u8 *slot;
    int index;
    if (flags & 2)
        goto failed;
    links = &gRfuLinkStatus;
    if ((*links)->linkLossSlotFlag)
        goto failed;
    one = 1;
    {
        int check = 1;
        do {
            check &= flags;
        } while (0);
        if (!check)
            goto ready;
    }
    slot = &p->sendSlot;
    rfu_clearSlot(TYPE_NI_SEND, *slot);
    if (rfu_NI_setSendData((u8)(1 << *slot), 15, p->niSend, sizeof(p->niSend)))
        goto sendError;
    index = 0;
    if (!((*links)->connSlotFlag & one)) {
        struct RfuLinkStatus **cursor = links;
        do {
            index++;
            if (index > 3)
                break;
        } while (!(((*cursor)->connSlotFlag >> index) & 1));
    }
    *slot = index;
    p->niFlags = (p->niFlags & 0xFFFE) | 2;
    goto ready;
sendError:
    Wireless_SetStatus(2);
failed:
    return 0;
ready:
    return 1;
}
