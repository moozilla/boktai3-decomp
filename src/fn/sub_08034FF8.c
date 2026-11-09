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
    u8 pad4c[0x58];
    u32 number;
    u8 work[16];
    u8 padb8[0x34];
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
#include "input.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
int Wireless_BeginChildTransfer(struct Engine *);
void Wireless_SetStatus(int);
void RfuManager_RequestMscCycleCompletion(void);
int Wireless_UpdateChildHandshake(struct Engine *p)
{
    u8 *slot;
    if (p->pending)
        p->pending = 0;
    {
        struct RfuSlotStatusNI **slots = gRfuSlotStatusNI;
        u8 *index = &p->index;
        int state = slots[*index]->recv.state;
        slot = index;
        if (state == 0x46) {
            if (p->scratch[1])
                Wireless_BeginChildTransfer(p);
            if (p->scratch[0]) {
                p->scratch[0] = 0;
                p->mask |= 64;
            }
            rfu_clearSlot(8, *slot);
        }
    }
    if (gPadState[1] & 2) {
        p->mask |= 0x100;
        p->mask |= 1;
    }
    if (p->mask & 64) {
        if (!(p->mask & 40)) {
            p->mask |= 0x100;
            p->mask |= 1;
        }
    }
    if (!(p->mask & 2)) {
        if (!gRfuLinkStatus->linkLossSlotFlag && (p->mask & 1)) {
            u8 *data;
            int cancel = p->mask & 256;
            data = (u8 *)&p->number;
            if (cancel) {
                *data = 1;
                p->mask = (p->mask & ~0x100) | 8;
            }
            rfu_clearSlot(4, *slot);
            if (rfu_NI_setSendData(1 << *slot, 6, data, 4)) {
                Wireless_SetStatus(2);
                return 0;
            }
            p->mask = (p->mask & ~1) | 2;
        }
        if (!(p->mask & 2))
            goto complete;
    }
    {
        int state = gRfuSlotStatusNI[*slot]->send.state;
        if (state == 0x26) {
            rfu_clearSlot(4, *slot);
            p->mask &= ~2;
            if (p->mask & 8) {
                p->mask = (p->mask & ~8) | 32;
                *(u8 *)&p->number = 0;
            }
        } else if (state == 0x27) {
            rfu_clearSlot(4, *slot);
            p->mask = 0;
        }
    }
complete:
    if ((p->mask & 96) == 96) {
        p->mask &= ~0x20;
        p->mask &= ~0x40;
        RfuManager_RequestMscCycleCompletion();
        setOperation(p, 12, 0);
        return 0;
    }
}
