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
void WirelessController_PollPeerRosterSend(struct Engine *p)
{
    int changed = 0;
    if (p->mask & 2) {
        struct RfuSlotStatusNI **slots = gRfuSlotStatusNI;
        int slot = p->mode;
        struct NIComm *s = &slots[slot]->send;
        {
            int state;
            state = s->state;
            if (state == 0x26) {
                int original = s->bmSlotOrg;
                int current = s->bmSlot;
                DEBUG_NOP();
                changed = current ^ original;
                rfu_clearSlot(4, slot);
                p->mask &= ~2;
                if (p->mask & 16) {
                    p->mask |= 128;
                }
                if (p->mask & 8) {
                    p->mask = (p->mask & ~8) | 32;
                }
            } else if (state == 0x27) {
                int original = s->bmSlotOrg;
                int current = s->bmSlot;
                DEBUG_NOP();
                changed = current ^ original;
                rfu_clearSlot(4, slot);
                {
                    int zero = 0;
                    p->mask = zero;
                    if (p->operation == 9) {
                        int op = 7;
                        int byteZero = 0;
                        p->flag = byteZero;
                        p->operation = op;
                        p->ticks = zero;
                        p->pending = 1;
                    }
                }
            }
        }
        if (changed) {
            p->flag35 |= changed;
        }
    }
}
