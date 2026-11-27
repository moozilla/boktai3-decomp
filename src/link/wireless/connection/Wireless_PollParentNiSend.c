#include "global.h"
#include "link/wireless/wireless_controller.h"
void Memory_Clear(void *, int);
#include "lib/rfu/rfu_compat.h"
#include "engine/ram.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->operationStage = zero;
    p->operation = op;
    p->operationFrame = zero;
    p->operationEntered = 1;
}
void Callback_InvokeActor(void *, void *);
void sub_08249238(void *);
void Wireless_PollParentNiSend(struct Engine *p)
{
    int lost = 0;
    if (p->niFlags & 2) {
        struct RfuSlotStatusNI **slots = gRfuSlotStatusNI;
        u8 *slot = &p->sendSlot;
        int state = slots[*slot]->send.state;
        if (state == 0x26) {
            if (p->onSend)
                Callback_InvokeActor((u8 *)p + 0xB8, p->onSend);
            lost = slots[*slot]->send.bmSlotOrg ^ slots[*slot]->send.bmSlot;
            rfu_clearSlot(4, *slot);
            p->niFlags &= ~2;
        } else if (state == 0x27) {
            if (p->onSendFailure)
                sub_08249238(p->onSendFailure);
            lost = slots[*slot]->send.bmSlotOrg ^ slots[*slot]->send.bmSlot;
            rfu_clearSlot(4, *slot);
            p->niFlags = 0;
        }
        if (lost)
            p->lostSlots |= lost;
    }
}
