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
extern struct Engine *gUnk_02000484;
void Memory_Clear(void *, int);
#define gRfuLinkStatus   gUnk_03006A80
#define gRfuSlotStatusNI gUnk_03006A70
#include "rfu_compat.h"
#include "ram.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
void Callback_InvokeActor(void *, void *);
void sub_08249238(void *);
void Wireless_PollParentNiSend(struct Engine *p)
{
    register int lost asm("r6") = 0;
    if (p->mask & 2) {
        struct RfuSlotStatusNI **slots = gUnk_03006A70;
        u8 *slot = &p->mode;
        int state = slots[*slot]->send.state;
        if (state == 0x26) {
            void *callback = p->head;
            if (callback)
                Callback_InvokeActor((u8 *)p + 0xB8, callback);
            {
                struct NIComm *send = &slots[*slot]->send;
                u8 *original = &send->bmSlotOrg;
                int old = *original;
                {
                    register int current asm("r0") = send->bmSlot;
                    asm("" : "+r"(current));
                    lost = current;
                    lost ^= old;
                }
            }
            rfu_clearSlot(4, *slot);
            p->mask &= 0xFFFD;
        } else if (state == 0x27) {
            void *callback = *(void **)((u8 *)p + 0x380);
            if (callback)
                sub_08249238(callback);
            {
                struct NIComm *send = &slots[*slot]->send;
                u8 *original = &send->bmSlotOrg;
                int old = *original;
                {
                    register int current asm("r0") = send->bmSlot;
                    asm("" : "+r"(current));
                    lost = current;
                    lost ^= old;
                }
            }
            rfu_clearSlot(4, *slot);
            p->mask = 0;
        }
        if (lost) {
            lost |= p->flag35;
            p->flag35 = lost;
        }
    }
}
