#include "global.h"
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
    u8 pad4c[0x34];
    struct UniPeerPacket peers[4];
    u8 pad90[0x14];
    u32 number;
    u8 work[16];
    u8 padb8[0x18];
    u8 lastSequence[4], repeats[4], behind[4], ahead[4], pade0[4];
    u8 sendStalls, receiveStalls, received, sent, pade8[4];
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
void Wireless_ReceiveParentUniPackets(struct Engine *p)
{
    u8 *scratch = p->scratch;
    int i;
    struct UniPeerPacket *packet;
    p->flag3e = 0;
    i = 0;
    packet = p->peers;
    do {
        int offset = i << 2;
        int bit = 2 << i;
        if ((scratch[2] & bit) && (*(struct RfuSlotStatusUNI **)((u8 *)gRfuSlotStatusUNI + offset))->recv.newDataFlag) {
            p->received |= bit;
            rfu_UNI_clearRecvNewDataFlag(i);
            if ((u8)(p->lastSequence[i] + 1) != packet->sequence) {
                if (p->lastSequence[i] == packet->sequence) {
                    p->repeats[i]++;
                    packet->flags = 0;
                } else if (p->lastSequence[i] < packet->sequence) {
                    p->ahead[i]++;
                    packet->flags = 0;
                } else
                    p->behind[i]++;
            }
            p->lastSequence[i] = packet->sequence;
        }
        packet++;
    } while (++i <= 3);
    {
        int received = p->received;
        int expected = scratch[2];
        int matched = expected & received;
        if (matched == expected) {
            if ((expected & p->sent) == matched)
                goto check;
            p->sendStalls++;
        }
        if ((p->sent & scratch[2]) != scratch[2])
            return;
    check:
        if ((p->received & scratch[2]) != scratch[2])
            p->receiveStalls++;
    }
}
