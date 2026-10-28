// MATCHING_CONCESSION: register-pinning
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
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
int WirelessController_ConsumePeerRefreshRequest(struct Engine *);
int WirelessController_ConsumeAdvanceRequest(struct Engine *);
int Wireless_UpdatePeerHandshake(struct Engine *p)
{
    int bits;
    if (p->pending) {
        p->pending = 0;
        p->entered = 0;
        p->flag = 0;
    }
    if (WirelessController_ConsumePeerRefreshRequest(p)) {
        if (!p->scratch[1] && !(p->mask & 0x238))
            p->mask = 0x105 | p->mask;
        p->entered = 0;
    }
    bits = p->mask;
    if (!(bits & 64))
        goto receive;
    if (bits & 0x338)
        goto receive;
    {
        register int literal asm("r3") = 0x105;
        register int value asm("r1");
        asm("" : "+r"(literal));
        value = literal;
        value |= bits;
        p->mask = value;
    }
receive:;
    if (gRfuSlotStatusNI[p->mode]->recv.state == 0x46) {
        u8 *base = (u8 *)p + p->mode * 4;
        u8 *message;
        message = base + 0xA8;
        if (*message) {
            *message = 0;
            p->mask |= 64;
        }
        rfu_clearSlot(8, p->mode);
    }
    bits = p->mask;
    if ((bits & 96) == 96) {
        int mask = 0xFFDF;
        int second;
        mask &= bits;
        second = 0xFFBF;
        mask &= second;
        p->mask = mask;
        p->flag35 |= gRfuLinkStatus->connSlotFlag | gRfuLinkStatus->linkLossSlotFlag;
        return 0;
    } else {
        int connected = p->bits;
        if (connected) {
            int blocked = bits & 0x168;
            if (!blocked && connected == gRfuLinkStatus->connSlotFlag) {
                if (WirelessController_ConsumeAdvanceRequest(p)) {
                    setOperation(p, 9, blocked);
                    return 0;
                }
            }
        }
    }
}
