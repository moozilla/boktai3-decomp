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
#define gRfuLinkStatus    gUnk_03006A80
#define gRfuSlotStatusUNI gUnk_03006A60
#define gRfuSlotStatusNI  gUnk_03006A70
#include "rfu_compat.h"
#include "ram.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
void Wireless_EnqueuePacket(void *, u8 *);
int Wireless_ReceiveChildUniPacket(struct Engine *p)
{
    u8 *scratch = p->scratch;
    int result = 0;
    int slot;
    {
        struct RfuSlotStatusUNI **slots = gUnk_03006A60;
        slot = p->index;
        if (slots[slot]->recv.newDataFlag) {
            u8 *packet = (u8 *)p + 0x90;
            u8 *previous;
            u8 *sequence;
            register int last asm("r6");
            int next;
            int received;
            int i;
            u16 *flags;
            u8 *changed;
            rfu_UNI_clearRecvNewDataFlag(slot);
            {
                register u8 *initial asm("r0") = (u8 *)p + 0xD0;
                u8 *incoming;
                last = *initial;
                next = last + 1;
                incoming = (u8 *)p + 0x94;
                next = (u8)next;
                received = *incoming;
                previous = initial;
                sequence = incoming;
            }
            if (next == received) {
                Wireless_EnqueuePacket(p->message, sequence);
            } else {
                register int old asm("r0") = (u8)last;
                register u8 *counter asm("r1");
                if (old == received)
                    counter = (u8 *)p + 0xD4;
                else
                    counter = (u8 *)p + 0xD8;
                (*counter)++;
            }
            {
                int value = *sequence;
                int zero = 0;
                *previous = value;
                (*packet)++;
                packet[1] = zero;
                i = zero;
                flags = &p->flags;
                changed = &p->changed;
            }
            do {
                register u8 *read asm("r6") = scratch;
                if ((read[2] >> i) & 1)
                    packet[1]++;
            } while (++i <= 4);
            {
                int value = *flags;
                register int zero asm("r1") = 0;
                *(u16 *)(packet + 2) = value;
                value = 65535;
                *flags = value;
                *changed = zero;
            }
            result = 1;
        }
    }
    return result;
}
