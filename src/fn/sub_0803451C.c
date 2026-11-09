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
void WirelessController_PollPeerRosterSend(struct Engine *p)
{
    int changed = 0;
    if (p->mask & 2) {
        struct RfuSlotStatusNI **slots = gRfuSlotStatusNI;
        int slot = p->mode;
        struct NIComm *s = &slots[slot]->send;
        if (s->state == 0x26) {
            changed = s->bmSlotOrg ^ s->bmSlot;
            rfu_clearSlot(4, p->mode);
            p->mask &= ~2;
            if (p->mask & 16)
                p->mask |= 128;
            if (p->mask & 8)
                p->mask = (p->mask & ~8) | 32;
        } else if (s->state == 0x27) {
            changed = s->bmSlotOrg ^ s->bmSlot;
            rfu_clearSlot(4, p->mode);
            p->mask = 0;
            if (p->operation == 9) {
                int next = 7;
                p->flag = 0;
                p->operation = next;
                p->ticks = 0;
                p->pending = 1;
            }
        }
        if (changed)
            p->flag35 |= changed;
    }
}
