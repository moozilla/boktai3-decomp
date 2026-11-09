#include "global.h"
struct PeerRosterPacket {
    u8 pending, sent, bits, count;
    u8 names[5][10];
    u16 ids[5];
};
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
    struct PeerRosterPacket scratch;
    u8 message[0x204];
    void *head, *tail;
};
void Memory_Clear(void *, int);
#include "rfu_compat.h"
#include "ram.h"
void String_CopyPadded(const void *, void *, int);
int WirelessController_BuildPeerRosterPacket(struct Engine *p)
{
    struct PeerRosterPacket *packet = &p->scratch;
    int i;
    CpuFill16(0, packet, sizeof(*packet));
    if (p->mask & 0x100) {
        packet->pending = 1;
        p->mask |= 8;
        p->mask &= ~0x100;
    } else
        packet->pending = 0;
    if (p->mask & 0x200) {
        packet->sent = 1;
        p->mask |= 16;
        p->mask &= ~0x200;
    } else
        packet->sent = 0;
    packet->count = 1;
    for (i = 0; i <= 3; i++)
        if ((p->bits >> i) & 1)
            packet->count++;
    packet->bits = (p->bits << 1) | 1;
    String_CopyPadded(gRfuLinkStatus->my.uname, packet->names[0], 8);
    packet->ids[0] = gRfuLinkStatus->my.id;
    for (i = 0; i <= 3; i++) {
        if ((p->bits >> i) & 1) {
            String_CopyPadded(gRfuLinkStatus->partner[i].uname, packet->names[i + 1], 8);
            packet->ids[i + 1] = gRfuLinkStatus->partner[i].id;
        }
    }
}
