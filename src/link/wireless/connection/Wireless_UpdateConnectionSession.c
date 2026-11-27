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
int Wireless_UpdateConnectionSession(struct Engine *argument)
{
    struct Engine *p = argument;
    if (p->managerReady) {
        int initial = gWirelessSessionState;
        u8 *lost = &p->lostSlots;
        if (initial == 1) {
            do {
            } while (gWirelessSessionState == 1);
        }
        sub_08223BC0(0);
        if (*lost) {
            rfu_REQ_disconnect(*lost);
            rfu_waitREQComplete();
            *lost = 0;
        }
        {
            int mode = gRfuLinkStatus->parentChild;
            if (mode == 1) {
                gWirelessSessionState = mode;
                rfu_REQ_sendData(1);
            }
        }
    }
    WirelessController_UpdatePeerRosterSend(p);
    WirelessController_PollPeerRosterSend(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
}
