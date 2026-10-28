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
    u8 pad4c[0x24];
    u8 send[16];
    u8 recv[4][4];
    u8 pad90[0x40];
    u8 slotA[4], slotB[4], slotC[4], slotD[4];
    u32 fE0;
    u8 fE4, fE5, fE6, fE7, fE8, fE9, padEA[2];
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
void Wireless_OnParentUniSessionEvent(u8);
void Wireless_OnParentUniSendComplete(u16);
void RfuManager_SetEventCallback(void (*)(u8));
void RfuManager_SetMscCallback(void (*)(u16));

/* Starts the parent side of a UNI session: builds the slot mask from the
   connected children, clears the exchange buffers and registers one receive
   buffer per child. */
void sub_080359E4(struct Engine *p)
{
    u8 *scratch = p->scratch;
    int i;
    Bg_ClearTilemap(0);
    p->flag1f = 0;
    Wireless_InitPacketQueue(p->message);
    Memory_Clear(p->send, 16);
    Memory_Clear(p->recv, 16);
    Wireless_ResetRemoteInputHistory(p);
    p->fE8 = 0;
    p->fE9 = 0;
    p->fE7 = 1;
    p->fE6 = 1;
    p->pad37 = scratch[2];
    scratch[2] = (gRfuLinkStatus->connSlotFlag << 1) | 1;
    p->send[2] = scratch[2];
    p->send[0] = 1;
    p->send[1] = 1;
    for (i = 0; i <= 3; i++) {
        if (scratch[2] & (2 << i))
            p->send[1]++;
    }
    p->flag36 = 1;
    for (i = 0; i <= 3; i++) {
        p->slotA[i] = 0;
        p->slotB[i] = 0;
        p->slotC[i] = 0;
        p->slotD[i] = 0;
    }
    p->fE0 = 0;
    p->fE4 = 0;
    p->fE5 = 0;
    RfuManager_SetEventCallback(Wireless_OnParentUniSessionEvent);
    RfuManager_SetMscCallback(Wireless_OnParentUniSendComplete);
    for (i = 0; i <= 3; i++) {
        if (scratch[2] & (2 << i))
            rfu_setRecvBuffer(16, i, p->recv[i], 4);
    }
    if ((u16)rfu_UNI_setSendData(scratch[2] >> 1, p->send, 16) == 0) {
        for (i = 0; i <= 3; i++) {
            if (scratch[2] & (2 << i))
                break;
        }
        p->mode = i;
        rfu_UNI_readySendData(p->mode);
        p->phase = 1;
        setOperation(p, 0x13, 0);
    } else {
        setOperation(p, 0x1B, 0);
    }
}
