#include "global.h"
struct UniPacket {
    u8 count, bits;
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
    u8 pad4c[0x44];
    struct UniPacket uni;
    u8 recv[16];
    u32 number;
    u8 work[16];
    u8 padb8[0x18];
    u8 lastSequence, padd1[3], repeats, padd5[3], drops, padd9[0x13];
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
int Wireless_ReceiveChildUniPacket(struct Engine *p)
{
    u8 *scratch = p->scratch;
    int result = 0;
    if (gRfuSlotStatusUNI[p->index]->recv.newDataFlag) {
        struct UniPacket *packet = &p->uni;
        int i;
        rfu_UNI_clearRecvNewDataFlag(p->index);
        if ((u8)(p->lastSequence + 1) == p->recv[0])
            Wireless_EnqueuePacket(p->message, p->recv);
        else if (p->lastSequence == p->recv[0])
            p->repeats++;
        else
            p->drops++;
        p->lastSequence = p->recv[0];
        packet->count++;
        packet->bits = 0;
        for (i = 0; i <= 4; i++)
            if ((scratch[2] >> i) & 1)
                packet->bits++;
        packet->flags = p->flags;
        p->flags = 0xFFFF;
        p->changed = 0;
        result = 1;
    }
    return result;
}
