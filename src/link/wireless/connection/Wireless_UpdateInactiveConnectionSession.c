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
void sub_08223BC0(int);
void WirelessController_UpdatePeerRosterSend(struct Engine *);
void WirelessController_PollPeerRosterSend(struct Engine *);
int Callback_InvokeActor(void *, void *);
extern void *gUnk_086056EC[];
int Wireless_UpdateInactiveConnectionSession(struct Engine *p)
{
    if (p->managerReady) {
        p->availableSlots = 0;
        sub_08223BC0(0);
    }
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
}
