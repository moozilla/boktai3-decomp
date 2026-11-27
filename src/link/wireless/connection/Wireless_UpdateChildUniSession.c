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
int Wireless_UpdateChildUniSession(struct Engine *p)
{
    p->inputReady = 0;
    sub_08223BC0(0);
    rfu_getMasterSlave();
    WirelessController_RestoreLocalInput(p);
    WirelessController_PollInputExchange(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
    publishInputs(p);
}
