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

// The original caller saw an int-width prototype for rfu_clearSlot (u8, u8 in
// the SDK header): its argument order only matches with int parameters.
#define rfu_clearSlot rfu_clearSlot_sdk
#include "rfu_compat.h"
#include "ram.h"
#undef rfu_clearSlot
u16 rfu_clearSlot(int connTypeFlag, int slotStatusIndex);
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
extern u16 gUnk_03005260[];
static inline void requestUpdate(struct Engine *p)
{
    int flags = p->mask;
    register int mask asm("r3") = 256;
    register int value asm("r2");
    asm("" : "+r"(flags), "+r"(mask));
    value = mask;
    value |= flags;
    value |= 1;
    asm("" : "+r"(value));
    p->mask = value;
}
static inline void requestUpdateB(struct Engine *p, int flags)
{
    register int mask asm("r3") = 256;
    register int value asm("r1");
    asm("" : "+r"(mask));
    value = mask;
    value |= flags;
    value |= 1;
    asm("" : "+r"(value));
    p->mask = value;
}
static inline void finish(struct Engine *p, int operation, u8 byteZero, u32 wordZero)
{
    p->flag = byteZero;
    p->operation = operation;
    p->ticks = wordZero;
    p->pending = 1;
}
int Wireless_UpdateChildHandshake(struct Engine *p)
{
    register u8 *slot asm("r5");
    int flags;
    if (p->pending)
        p->pending = 0;
    {
        struct RfuSlotStatusNI **slots = gRfuSlotStatusNI;
        register u8 *initial asm("r3") = &p->index;
        int state;
        state = slots[*initial]->recv.state;
        slot = initial;
        if (state == 0x46) {
            if (p->scratch[1])
                Wireless_BeginChildTransfer(p);
            {
                register u32 offset asm("r3") = 0x134;
                u8 *marker;
                asm("" : "+r"(offset));
                marker = (u8 *)p + offset;
                if (*marker) {
                    *marker = 0;
                    p->mask |= 64;
                }
            }
            rfu_clearSlot(8, *slot);
        }
    }
    if (gUnk_03005260[1] & 2)
        requestUpdate(p);
    flags = p->mask;
    if (!(flags & 64))
        goto send;
    if (flags & 40)
        goto send;
    requestUpdateB(p, flags);
send:;
    flags = p->mask;
    if (!(flags & 2)) {
        if (!gRfuLinkStatus->linkLossSlotFlag) {
            u8 *data;
            int one = 1;
            asm("" : "+r"(one));
            {
                int test = one & flags;
                if (!test)
                    goto maybeComplete;
                {
                    int cancel = 256 & flags;
                    asm("" : "+r"(cancel));
                    data = (u8 *)&p->number;
                    if (cancel) {
                        int bits;
                        int mask;
                        *data = 1;
                        bits = p->mask;
                        mask = 0xFEFF;
                        mask &= bits;
                        mask |= 8;
                        p->mask = mask;
                    }
                    rfu_clearSlot(4, *slot);
                    {
                        register int sendMask asm("r0") = one;
                        asm("" : "+r"(sendMask));
                        if (rfu_NI_setSendData((u8)(sendMask << *slot), 6, data, 4)) {
                            Wireless_SetStatus(2);
                            return 0;
                        }
                        p->mask = (p->mask & ~1) | 2;
                    }
                }
            }
        }
    maybeComplete:
        if (!(p->mask & 2))
            goto complete;
    }
    {
        struct RfuSlotStatusNI **slots = gRfuSlotStatusNI;
        int index;
        int state;
        index = *slot;
        state = slots[index]->send.state;
        if (state == 0x26) {
            rfu_clearSlot(4, index);
            p->mask &= ~2;
            if (p->mask & 8) {
                p->mask = (p->mask & ~8) | 32;
                *(u8 *)&p->number = 0;
            }
        } else if (state == 0x27) {
            rfu_clearSlot(4, index);
            p->mask = 0;
        }
    }
complete:
    flags = p->mask;
    if ((flags & 96) == 96) {
        int value = 0xFFDF;
        register u8 zeroByte asm("r4");
        u32 zeroWord;
        register int second asm("r1");
        value &= flags;
        zeroByte = 0;
        zeroWord = 0;
        asm("" : "+r"(zeroByte), "+r"(zeroWord));
        second = 0xFFBF;
        value &= second;
        p->mask = value;
        RfuManager_RequestMscCycleCompletion();
        finish(p, 12, zeroByte, zeroWord);
        return 0;
    }
}
