#include "global.h"
#include "ram.h"
#include "rfu_manager.h"
#include "rfu_compat.h"
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
struct Record {
    u8 pad0, mode, pad2[6], count, pad9[13], status, pad17[9];
};
void WirelessController_RebuildAvailablePeerList(struct Engine *p)
{
    int i;
    int left, right, limit;
    p->count = 0;
    i = 0;
    for (; i < ((struct Record *)gRfuLinkStatus)->count; i++) {
        if ((p->allowed >> i) & 1) {
            {
                struct Record *base = ((struct Record *)gRfuLinkStatus);
                int offset;
                offset = i << 5;
                base = (struct Record *)((u32)base + offset);
                if (base->status <= 3) {
                    p->ids[p->count] = i;
                    p->count++;
                }
            }
        }
    }
    left = p->scroll - 2;
    right = p->scroll + 3;
    if (left < 0) {
        i = left;
        do {
            i++;
        } while (i < 0);
    }
    i = left;
    if (i < 0)
        i = 0;
    goto test;
loop:
    i++;
test:
    limit = right;
    if (right > 8)
        limit = 8;
    if (i < limit)
        goto loop;
    if (right > 7 && i < right) {
        i = right - i;
        do {
            i--;
        } while (i);
    }
}
