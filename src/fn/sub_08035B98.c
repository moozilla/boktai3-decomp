#include "global.h"
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
    u8 pad4c[0x44];
    u8 send[4];
    u8 recv[16];
    u32 number;
    u8 work[16];
    u8 padb8[0x18];
    u8 slotA[4], slotB[4], slotC[4], slotD[4];
    u8 pade0[0xC];
    u16 flags, backup, history[5];
    u8 changed, padfb[5];
    u16 diff[5];
    u8 pad10a[2];
    u32 key;
    u16 input[5];
    u8 pad11a[0x1A];
    u8 scratch[0x36];
    u16 peerIds[4];
    u8 pad172[2];
    u8 message[0x204];
    void *head, *tail;
};
#include "rfu_compat.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
void Bg_ClearTilemap(int);
void Wireless_InitPacketQueue(void *);
void Memory_Clear(void *, int);
void Wireless_ResetRemoteInputHistory(struct Engine *);
void Wireless_OnChildUniSessionEvent(u8);
void Wireless_OnChildUniClockEvent(u16);
void RfuManager_SetEventCallback(void (*)(u8));
void RfuManager_SetMscCallback(void (*)(u16));

/* Starts the child side of a UNI session: finds this unit's peer slot,
   clears the exchange buffers and registers the receive/send buffers. */
void sub_08035B98(struct Engine *p)
{
    u8 *scratch = p->scratch;
    int i;
    Bg_ClearTilemap(0);
    for (i = 0; i <= 3; i++) {
        if (p->peerIds[i] == gRfuLinkStatus->my.id)
            break;
    }
    p->flag1f = i;
    Wireless_InitPacketQueue(p->message);
    Memory_Clear(p->send, 4);
    Memory_Clear(p->recv, 16);
    Wireless_ResetRemoteInputHistory(p);
    for (i = 0; i <= 3; i++) {
        p->slotA[i] = 0;
        p->slotB[i] = 0;
        p->slotC[i] = 0;
        p->slotD[i] = 0;
    }
    p->pad37 = scratch[2];
    p->send[0] = 0;
    RfuManager_SetEventCallback(Wireless_OnChildUniSessionEvent);
    RfuManager_SetMscCallback(Wireless_OnChildUniClockEvent);
    rfu_setRecvBuffer(16, p->index, p->recv, 16);
    {
        u16 result = rfu_UNI_setSendData(1 << p->index, p->send, 4);
        if (result) {
            setOperation(p, 0x1B, 0);
        } else {
            p->phase = 1;
            setOperation(p, 0x14, result);
        }
    }
}
