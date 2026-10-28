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
#define gRfuLinkStatus   gUnk_03006A80
#define gRfuSlotStatusNI gUnk_03006A70
#include "rfu_compat.h"
#include "ram.h"
void WirelessController_BuildPeerRosterPacket(struct Engine *);
void Wireless_SetStatus(int);
int WirelessController_UpdatePeerRosterSend(struct Engine *p)
{
    int result = 0;
    register int bits asm("r2") = p->mask;
    if (bits & 2)
        goto fail;
    if (gUnk_03006A80->linkLossSlotFlag)
        goto fail;
    if (bits & 0xA0)
        goto fail;
    goto run;
fail:
    return 0;
run:
    if (p->operation == 9 && !(bits & 0x210))
        p->mask = 0x205 | bits;
    {
        register int value asm("r0") = p->bits;
        int previous = p->pad1a;
        int current;
        asm("" : "+r"(value), "+r"(previous));
        current = value;
        if (previous != current) {
            {
                int flags = p->mask;
                int mask = 4;
                int unusedZero = 0;
                p->mask = flags | mask;
            }
            if (gUnk_03006A80->connSlotFlag)
                p->mask |= 1;
        }
        p->pad1a = current;
    }
    if (p->mask & 4) {
        WirelessController_BuildPeerRosterPacket(p);
        p->mask &= ~4;
        result = 1;
    }
    {
        register int flags asm("r1") = p->mask;
        int one = 1;
        if (one & flags) {
            if (!rfu_NI_setSendData(p->bits, 67, p->scratch, 64)) {
                int slot = 0;
                if (!(gUnk_03006A80->connSlotFlag & one)) {
                    do {
                        slot++;
                        if (slot > 3)
                            break;
                    } while (!((gUnk_03006A80->connSlotFlag >> slot) & 1));
                }
                p->mode = slot;
                p->mask = (p->mask & ~1) | 2;
            } else
                Wireless_SetStatus(2);
        }
    }
    return result;
}
