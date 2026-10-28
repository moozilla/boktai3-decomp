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
void Wireless_OnParentUniSendComplete(void)
{
    struct Engine *p = gWirelessController;
    u8 slots;
    if (p) {
        u8 *scratch = p->scratch;
        int status = rfu_UNI_PARENT_getDRAC_ACK(&slots);
        /* do-while(0) only keeps agbcc's block layout */
        do {
            if (!status) {
                u8 *seen = ((u8 *)p) + 0xE7;
                int bits = (slots << 1) | (*seen);
                int expected;
                *seen = bits;
                expected = scratch[2];
                if ((bits & expected) == expected) {
                    u8 *waiting = ((u8 *)p) + 0xE8;
                    *waiting = status;
                    p->flag36 = status;
                } else {
                    u8 *waiting = ((u8 *)p) + 0xE8;
                    *waiting = 1;
                    (*((u32 *)(((u8 *)p) + 0xE0)))++;
                }
            }
            rfu_REQ_recvData();
        } while (0);
    }
}
