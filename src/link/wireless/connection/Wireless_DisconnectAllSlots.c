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
void Sound_PlaySong(int);
void InterruptsAndVideo_WaitFrames(int);
int Wireless_BeginParentNiSession(struct Engine *);
int sub_082238F0(int);
void WirelessController_ResetPeerExchange(struct Engine *);
int Wireless_DisconnectAllSlots(struct Engine *p)
{
    if (p->operationEntered)
        p->operationEntered = 0;
    if ((u8)rfu_getMasterSlave() == 1) {
        rfu_REQ_disconnect(gRfuLinkStatus->connSlotFlag | gRfuLinkStatus->linkLossSlotFlag);
        rfu_waitREQComplete();
    }
}
