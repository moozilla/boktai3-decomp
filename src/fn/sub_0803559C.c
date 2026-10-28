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
// MATCHING_CONCESSION: register-pinning
void Wireless_EnqueuePacket(void *, u8 *);
void Wireless_BuildParentUniPacket(struct Engine *argument)
{
    struct {
        u32 fill;
        int sequence;
        u8 *waiting;
        u8 *sent;
    } stack;
    register struct Engine *p asm("r5") = argument;
    u8 *scratch = p->scratch;
    u8 *packet = (u8 *)p + 0x70;
    u8 *sent;
    int got;
    int expected;
    u8 *read;
    {
        int offset = 0xE7;
        register u8 *initial asm("r1");
        asm("" : "+r"(offset));
        initial = (u8 *)(offset + (u32)p);
        sent = initial;
        got = *initial;
        read = scratch;
        expected = read[2];
    }
    got &= expected;
    if (got == expected) {
        int zero = p->flag3e;
        if (zero) {
            if (read[3] > 1) {
                register u8 *waiting asm("r1") = &p->flag36;
                *waiting = 1;
            }
        } else {
            u8 *waiting;
            int i;
            u16 *backup;
            u16 *flags;
            u8 *seen;
            int bit;
            register u16 *input asm("r3");
            u16 *output;
            stack.sequence = *packet;
            Wireless_EnqueuePacket(p->message, packet);
            waiting = &p->flag36;
            *waiting = zero;
            stack.fill = zero;
            CpuSet(&stack.fill, packet, 0x05000004);
            i = 0;
            stack.sent = sent;
            stack.waiting = waiting;
            backup = (u16 *)((u8 *)p + 0xEA);
            {
                int offset = 0xEC;
                flags = (u16 *)(offset + (u32)p);
            }
            {
                int offset = 0xE6;
                seen = (u8 *)(offset + (u32)p);
            }
            {
                int initial = 2;
                bit = initial;
            }
            input = (u16 *)((u8 *)p + 0x82);
            output = (u16 *)(waiting + 0x40);
            do {
                int mask = bit;
                u8 *read;
                mask <<= i;
                read = scratch;
                if (read[2] & mask)
                    *output = *input;
                input += 2;
                output++;
            } while (++i <= 3);
            *(u16 *)(packet + 4) = *backup;
            {
                u16 *read = flags;
                int value;
                value = *read;
                *backup = value;
                value = 65535;
                *read = value;
            }
            {
                u8 *read = scratch;
                packet[2] = read[2];
            }
            packet[1] = 1;
            i = 0;
            {
                register int sequence asm("r3") = stack.sequence + 1;
                register int bit asm("r5") = 2;
                do {
                    u8 *read = scratch;
                    int active;
                    int mask;
                    active = read[2];
                    mask = bit << i;
                    asm("" : "+r"(mask));
                    if (active & mask)
                        packet[1]++;
                } while (++i <= 3);
                *packet = sequence;
            }
            {
                int one = 1;
                register u8 *write asm("r2") = stack.sent;
                *write = one;
                {
                    register u8 *write asm("r5") = seen;
                    asm("" : "+r"(write));
                    *write = one;
                }
                {
                    u8 *read = scratch;
                    if (read[3] > 1) {
                        register u8 *write asm("r5") = stack.waiting;
                        *write = one;
                    }
                }
            }
        }
    }
}
