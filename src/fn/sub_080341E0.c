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
int WirelessPacketQueue_Pop(void *, void *);
int WirelessController_PollInputExchange(struct Engine *p)
{
    u8 *changed;
    int result;
    void *key = &p->key;
    void *message = p->message;
    if (WirelessPacketQueue_Pop(message, key)) {
        u16 *src;
        u16 *diff;
        u16 *history;
        int i;
        changed = &p->changed;
        src = p->input;
        diff = p->diff;
        history = p->history;
        i = 4;
        do {
            *history = *src;
            *diff ^= *src;
            src++;
            diff++;
            history++;
            i--;
        } while (i >= 0);
        result = 0;
    } else {
        result = -2;
        changed = &p->changed;
    }
    {
        int one = 1;
        *changed = one;
    }
    return result;
}
