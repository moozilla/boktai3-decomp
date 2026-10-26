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
void String_CopyPadded(const void *, void *, int);
int WirelessController_BuildPeerRosterPacket(struct Engine *p)
{
    u8 *packet = p->scratch;
    u16 fill;
    u16 *fillPtr = &fill;
    int zero = 0;
    int i;
    asm("" : "+r"(zero));
    *fillPtr = zero;
    CpuSet(fillPtr, packet, 0x01000020);
    if (p->mask & 0x100) {
        packet[0] = 1;
        {
            int bits = p->mask;
            bits |= 8;
            bits |= zero;
            {
                int mask = 0xFEFF;
                p->mask = bits & mask;
            }
        }
    } else
        packet[0] = zero;
    {
        u16 bits = p->mask & 0x200;
        if (bits) {
            packet[1] = 1;
            {
                int flags = p->mask | 16;
                int mask = 0xFDFF;
                p->mask = flags & mask;
            }
        } else
            packet[1] = bits;
    }
    packet[3] = 1;
    for (i = 0; i <= 3; i++)
        if ((p->bits >> i) & 1)
            packet[3]++;
    packet[2] = (p->bits << 1) | 1;
    String_CopyPadded(gUnk_03006A80->my.uname, packet + 4, 8);
    *(u16 *)(packet + 0x36) = gUnk_03006A80->my.id;
    for (i = 0; i <= 3; i++) {
        int selected = (p->bits >> i) & 1;
        int next = i + 1;
        if (selected) {
            String_CopyPadded(gUnk_03006A80->partner[i].uname, packet + 14 + i * 10, 8);
            {
                int offset = next << 1;
                u16 *ids;
                asm("" : : "r"(offset));
                ids = (u16 *)(packet + 0x36);
                ids += next;
                *ids = gUnk_03006A80->partner[i].id;
            }
        }
    }
}
