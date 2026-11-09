#include "global.h"
struct UniParentPacket {
    u8 sequence, count, bits, pad3;
    u16 flags;
    u16 peerFlags[4];
    u8 pad0e[2];
};
struct UniPeerPacket {
    u8 sequence, pad1;
    u16 flags;
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
    u8 pad4c[0x24];
    struct UniParentPacket uniOut;
    struct UniPeerPacket peers[4];
    u8 pad90[0x14];
    u32 number;
    u8 work[16];
    u8 padb8[0x18];
    u8 lastSequence[4], repeats[4], behind[4], ahead[4], pade0[4];
    u8 sendStalls, receiveStalls, received, sent, pade8[2];
    u16 lastFlags;
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
void Wireless_EnqueuePacket(void *, u8 *);
void Wireless_BuildParentUniPacket(struct Engine *p)
{
    u8 *scratch = p->scratch;
    struct UniParentPacket *packet = &p->uniOut;
    if ((p->sent & scratch[2]) == scratch[2]) {
        if (p->flag3e) {
            if (scratch[3] > 1)
                p->flag36 = 1;
        } else {
            /* "+ 0": one more gcse insn outside i's live range, for PRE's stack-slot order (scope preorder) */
            int sequence = (u8)(packet->sequence + 0);
            int i;
            Wireless_EnqueuePacket(p->message, packet);
            p->flag36 = 0;
            CpuFill32(0, packet, sizeof(*packet));
            for (i = 0; i <= 3; i++) {
                int bit = 2 << i;
                if (scratch[2] & bit)
                    packet->peerFlags[i] = p->peers[i].flags;
            }
            packet->flags = p->lastFlags;
            p->lastFlags = p->flags;
            p->flags = 0xFFFF;
            packet->bits = scratch[2];
            packet->count = 1;
            i = 0;
            {
                int next = sequence + 1;
                int bit = 2;
                do {
                    if (scratch[2] & (bit << i))
                        packet->count++;
                } while (++i <= 3);
                packet->sequence = next;
            }
            p->sent = 1;
            p->received = 1;
            if (scratch[3] > 1)
                p->flag36 = 1;
        }
    }
}
