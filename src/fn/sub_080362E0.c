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
void Wireless_SetStatus(int);
void Callback_InvokeActor(void *, void *);
void sub_08249238(void *);
int Wireless_ExchangeChildNiPacket(struct Engine *argument)
{
    struct Engine *p = argument;
    unsigned long long flags = p->mask;
    struct RfuLinkStatus **links;
    int one;
    u8 *slot;
    int index;
    {
        int test = 2 & flags;
        if (test) {
            goto failed;
        }
    }
    if (gUnk_03006A80->linkLossSlotFlag) {
        goto failed;
    }
    {
        struct RfuSlotStatusNI **slots = gUnk_03006A70;
        u8 *initial;
        int state;
        int index;
        initial = &p->index;
        index = *initial;
        state = slots[index]->recv.state;
        slot = initial;
        if (state == 0x46) {
            void *callback = p->tail;
            if (callback)
                Callback_InvokeActor((u8 *)p + 0xC4, callback);
            rfu_clearSlot(8, *slot);
        } else if ((u16)(state - 0x47) <= 1) {
            void *callback = *(void **)((u8 *)p + 0x384);
            if (callback) {
                sub_08249238(callback);
            }
        }
    }
    flags = p->mask;
    one = 1;
    {
        int check = one;
        check &= flags;
        if (!check) {
            goto ready;
        }
    }
    do {
        rfu_clearSlot(4, *slot);
    } while (0);
    if (rfu_NI_setSendData((u8)(one << (*slot)), 14, ((u8 *)p) + 0xB8, 12)) {
        goto sendError;
    }
    index = 0;
    links = &gUnk_03006A80;
    if (!((*links)->connSlotFlag & one)) {
        struct RfuLinkStatus **cursor = links;
        int bit = 1;
        do {
            index++;
            if (index > 3) {
                break;
            }
        } while (!(((*(struct RfuLinkStatus *volatile *)cursor)->connSlotFlag >> index) & bit));
    }
    *slot = index;
    p->mask = (p->mask & 0xFFFE) | 2;
    goto ready;
sendError:
    Wireless_SetStatus(2);
failed:
    return 0;
ready:
    return 1;
}
