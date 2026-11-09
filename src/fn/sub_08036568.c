#include "global.h"
struct UniParentPacket {
    u8 sequence, count, bits, pad3;
    u16 flags;
    u16 peerFlags[4];
    u8 pad0e[2];
};
struct Engine {
    u8 pad0[0x18];
    u8 pad18, bits, pad1a, entered, flag, pad1d, state, flag1f, mode, index;
    u16 mask;
    s8 scroll;
    u8 selected, allowed, count, ids[8], active, flag31, pending, flag33, phase, flag35, flag36,
        pad37;
    u16 operation, value;
    u8 flag3c, flag3d, flag3e, flag3f;
    u16 timer;
    u8 flag42, flag43;
    u32 ticks;
    void *pointer;
    u8 pad4c[0x24];
    struct UniParentPacket uniOut;
    u8 pad80[0x24];
    u32 number;
    u8 work[16];
    u8 padb8[0x2E];
    u8 received, sent, retransmit, retransmitDone, padea[2];
    u16 flags, backup, history[5];
    u8 changed, padfb[5];
    u16 diff[5];
    u8 pad10a[2];
    u32 key;
    u16 input[5];
    u8 pad11a[0x1A];
    u8 scratch[64];
    u8 message[0x204];
    void *head, *tail;
};
void Memory_Clear(void *, int);
#include "rfu_compat.h"
#include "ram.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
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
    int enabled = p->flag31;
    if (enabled) {
        int i;
        gLinkMenuActive = 1;
        i = 0;
        while (1) {
            if (p->history[i] & 0x2000)
                gSceneSelectionDisplaySlots[i] = p->history[i] & 0xFFF;
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
    p->flag31 = 0;
    while (gWirelessSessionState == 1)
        ;
    sub_08223BC0(0);
    if (p->retransmitDone && p->retransmit) {
        p->retransmit = 0;
        p->sent = 1;
        p->received = 1;
        p->retransmitDone = 0;
    }
    WirelessController_RestoreLocalInput(p);
    Wireless_BuildParentUniPacket(p);
    if (!gRfuLinkStatus->linkLossSlotFlag && p->flag36) {
        gWirelessSessionState = 1;
        if (!p->retransmit) {
            rfu_UNI_changeAndReadySendData(p->mode, &p->uniOut, 16);
            RfuManager_SendData(1);
        } else
            rfu_REQ_PARENT_resumeRetransmitAndChange();
    }
    WirelessController_PollInputExchange(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
    publishInputs(p);
}
