#include "global.h"
#include "link/wireless/wireless_controller.h"
struct UniParentPacket {
    u8 sequence, count, bits, pad3;
    u16 flags;
    u16 peerFlags[4];
    u8 pad0e[2];
};
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
void WirelessController_RestoreLocalInput(struct Engine *);
void WirelessController_PollInputExchange(struct Engine *);
void Wireless_BuildParentUniPacket(struct Engine *);
void RfuManager_SendData(int);
static inline void publishInputs(struct Engine *p)
{
    int enabled = p->inputReady;
    if (enabled) {
        int i;
        gLinkMenuActive = 1;
        i = 0;
        while (1) {
            if (p->inputHistory[i] & 0x2000)
                gSceneSelectionDisplaySlots[i] = p->inputHistory[i] & 0xFFF;
            else
                gSceneSelectionDisplaySlots[i] |= 0xFFFF;
            if (++i > 4)
                goto done;
        }
    done:;
    } else
        gLinkMenuActive = enabled;
}
int Wireless_UpdateParentUniSession(struct Engine *p)
{
    p->inputReady = 0;
    while (gWirelessSessionState == 1)
        ;
    sub_08223BC0(0);
    if (p->retransmitDone && p->waitingForAck) {
        p->waitingForAck = 0;
        p->acknowledgedSlots = 1;
        p->receivedSlots = 1;
        p->retransmitDone = 0;
    }
    WirelessController_RestoreLocalInput(p);
    Wireless_BuildParentUniPacket(p);
    if (!gRfuLinkStatus->linkLossSlotFlag && p->sendReady) {
        gWirelessSessionState = 1;
        if (!p->waitingForAck) {
            rfu_UNI_changeAndReadySendData(p->sendSlot, &p->parentSend, 16);
            RfuManager_SendData(1);
        } else
            rfu_REQ_PARENT_resumeRetransmitAndChange();
    }
    WirelessController_PollInputExchange(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
    publishInputs(p);
}
