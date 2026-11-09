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
void WirelessController_BuildPeerRosterPacket(struct Engine *);
void Wireless_SetStatus(int);
int WirelessController_UpdatePeerRosterSend(struct Engine *p)
{
    int result = 0;
    if ((p->mask & 2) || gRfuLinkStatus->linkLossSlotFlag || (p->mask & 0xA0))
        return 0;
    if (p->operation == 9 && !(p->mask & 0x210))
        p->mask |= 0x205;
    if (p->pad1a != p->bits) {
        p->mask |= 4;
        if (gRfuLinkStatus->connSlotFlag)
            p->mask |= 1;
    }
    p->pad1a = p->bits;
    if (p->mask & 4) {
        WirelessController_BuildPeerRosterPacket(p);
        p->mask &= ~4;
        result = 1;
    }
    if (p->mask & 1) {
        if (!rfu_NI_setSendData(p->bits, 67, p->scratch, 64)) {
            int slot = 0;
            if (!((gRfuLinkStatus->connSlotFlag >> slot) & 1)) {
                do {
                    slot++;
                    if (slot > 3)
                        break;
                } while (!((gRfuLinkStatus->connSlotFlag >> slot) & 1));
            }
            p->mode = slot;
            p->mask = (p->mask & ~1) | 2;
        } else
            Wireless_SetStatus(2);
    }
    return result;
}
