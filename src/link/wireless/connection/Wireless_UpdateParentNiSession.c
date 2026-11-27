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
void RfuManager_SendData(int);
void Wireless_TryStartParentNiSend(struct Engine *);
void Wireless_PollParentNiSend(struct Engine *);
int Callback_InvokeActor(void *, void *);
extern void *gUnk_086056EC[];
void Wireless_UpdateParentNiSession(struct Engine *argument)
{
    struct Engine *p = argument;
    u8 *lost;
    while (gWirelessSessionState == 1)
        ;
    sub_08223BC0(0);
    lost = &p->lostSlots;
    if (*lost) {
        rfu_REQ_disconnect(*lost);
        rfu_waitREQComplete();
        *lost = 0;
    }
    {
        int mode = gRfuLinkStatus->parentChild;
        if (mode == 1) {
            gWirelessSessionState = mode;
            RfuManager_SendData(1);
        }
    }
    Wireless_TryStartParentNiSend(p);
    Wireless_PollParentNiSend(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
}
