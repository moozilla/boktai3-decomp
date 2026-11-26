#include "engine/shared/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"
#include "engine/shared/ram.h"
void WirelessController_PollPeerRosterSend(WirelessController *p)
{
    int changed = 0;
    if (p->niFlags & 2) {
        struct RfuSlotStatusNI **slots = gRfuSlotStatusNI;
        int slot = p->sendSlot;
        struct NIComm *s = &slots[slot]->send;
        if (s->state == SLOT_STATE_SEND_SUCCESS) {
            changed = s->bmSlotOrg ^ s->bmSlot;
            rfu_clearSlot(TYPE_NI_SEND, p->sendSlot);
            p->niFlags &= ~2;
            if (p->niFlags & 16)
                p->niFlags |= 128;
            if (p->niFlags & 8)
                p->niFlags = (p->niFlags & ~8) | 32;
        } else if (s->state == SLOT_STATE_SEND_FAILED) {
            changed = s->bmSlotOrg ^ s->bmSlot;
            rfu_clearSlot(TYPE_NI_SEND, p->sendSlot);
            p->niFlags = 0;
            if (p->operation == WIRELESS_OP_WAIT_PARENT_TRANSFER)
                WirelessController_SetOperation(p, WIRELESS_OP_PEER_HANDSHAKE);
        }
        if (changed)
            p->lostSlots |= changed;
    }
}
