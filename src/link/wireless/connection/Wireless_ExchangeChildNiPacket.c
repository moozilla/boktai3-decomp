#include "link/wireless/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"
#include "engine/ram.h"
void Wireless_SetStatus(int);
void Callback_InvokeActor(void *, void *);
void sub_08249238(void *);
int Wireless_ExchangeChildNiPacket(WirelessController *p)
{
    u8 *slot;
    int index;
    if (p->niFlags & 2)
        goto failed;
    if (gRfuLinkStatus->linkLossSlotFlag) {
        goto failed;
    }
    {
        struct RfuSlotStatusNI **slots = gRfuSlotStatusNI;
        u8 *initial;
        int state;
        int index;
        initial = &p->parentSlot;
        index = *initial;
        state = slots[index]->recv.state;
        slot = initial;
        if (state == SLOT_STATE_RECV_SUCCESS) {
            void *callback = p->onReceive;
            if (callback)
                Callback_InvokeActor(p->niReceive, callback);
            rfu_clearSlot(TYPE_NI_RECV, *slot);
        } else if ((u16)(state - SLOT_STATE_RECV_FAILED) <= 1) {
            void *callback = p->onReceiveFailure;
            if (callback) {
                sub_08249238(callback);
            }
        }
    }
    if (!(p->niFlags & 1))
        goto ready;
    rfu_clearSlot(TYPE_NI_SEND, *slot);
    if (rfu_NI_setSendData((u8)(1 << (*slot)), 14, p->niSend, sizeof(p->niSend))) {
        goto sendError;
    }
    for (index = 0; index < RFU_CHILD_MAX; index++) {
        if (gRfuLinkStatus->connSlotFlag & (1 << index))
            break;
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
