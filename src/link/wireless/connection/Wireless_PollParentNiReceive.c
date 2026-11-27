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
int Wireless_PollParentNiReceive(struct Engine *p)
{
    u8 *slot;
    int state;
    if (p->operationEntered)
        p->operationEntered = 0;
    {
        struct RfuSlotStatusNI **slots = gRfuSlotStatusNI;
        slot = &p->sendSlot;
        state = slots[*slot]->recv.state;
    }
    if (state == 0x46) {
        void *callback = p->onReceive;
        if (callback)
            Callback_InvokeActor(p->niReceive, callback);
        rfu_clearSlot(8, *slot);
    } else if ((u16)(state - 0x47) <= 1) {
        void *callback = p->onReceiveFailure;
        if (callback)
            sub_08249238(callback);
    }
    return 0;
}
