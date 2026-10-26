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
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
struct Event {
    u8 pad[0x14];
    u16 code, value;
};
extern struct Event gUnk_03005390;
void Wireless_UpdateUniAcknowledgedSlots(struct Engine *);
void Wireless_ReceiveParentUniPackets(struct Engine *);
void Wireless_ProcessChildUniExchange(struct Engine *);
void Wireless_RemoveConnectedSlots(struct Engine *, u8);
void WirelessController_ResetSession(struct Engine *);
int sub_082238F0(int);
void Wireless_OnParentUniSessionEvent(u8 command)
{
    int key = command;
    int saved = key;
    struct Engine *p = gUnk_02000484;
    if (!p) {
        goto done;
    }
    if (key == 0x41) {
        goto done;
    }
    if (key > 0x41) {
        goto high;
    }
    if (key == 0x30) {
        goto disconnected;
    }
    if (key > 0x30) {
        goto lowMore;
    }
    goto done;
lowMore:
    if (key == 0x32) {
        goto removed;
    }

    switch (key) {
        case 0x32 ... 0x7FFFFFFF:
            break;
        default:
            goto added;
    }

    if (key == 0x33) {
        goto disconnected;
    }
    if (key == 0x40) {
        goto lost;
    }
    goto done;
high:
    if (key == 0x50) {
        goto receive;
    }

    if (key > 0x50) {
        goto last;
    }
    if (key == 0x43) {
        goto done;
    }
    switch (key) {
        case 0x43 ... 0x7FFFFFFF:
            break;
        default:
            goto reset;
    }

    if (key == 0x44) {
        goto stop;
    }
    goto done;
last:
    if (key == 0xF1) {
        goto error;
    }

    if (key > 0xF1) {
        goto lastMore;
    }
    if (command == 0xF0) {
        goto error;
    }
    goto done;
lastMore:
    if (saved == 0xF2) {
        goto error;
    }

    if (saved == 0xFF) {
        goto error;
    }
    goto done;
receive:
    Wireless_UpdateUniAcknowledgedSlots(p);
    Wireless_ReceiveParentUniPackets(p);
    gWirelessSessionState = 0;
    goto done;
added: {
    u8 *slots = &p->selected;
    int incoming = (u8)gUnk_03005390.code;
    *slots |= (u8)gUnk_03005390.code;
}
    goto done;
removed: {
    u8 *slots = &p->selected;
    int incoming = (u8)gUnk_03005390.code;
    *slots &= ~incoming;
}
    goto done;
disconnected: {
    u8 *slots = &p->selected;
    struct Event *event = &gUnk_03005390;
    int zero;
    int remaining;
    int incoming = (u8)event->code;
    int selected = *slots & ~incoming;
    zero = 0;
    *slots = selected;
    ((u8 *)p)[0xE9] = 1;
    Wireless_RemoveConnectedSlots(p, (u8)event->code);
    remaining = p->scratch[3];
    if (remaining != 1) {
        goto done;
    }
    sub_082238F0(1);
    {
        int op = 29;
        p->flag = zero;
        p->operation = op;
        p->ticks = zero;
    }
    p->pending = remaining;
}
    goto done;
lost: {
    int remaining;
    Wireless_RemoveConnectedSlots(p, (u8)gUnk_03005390.code);
    remaining = p->scratch[3];
    if (remaining != 1) {
        goto done;
    }
    sub_082238F0(1);
    setOperation(p, 29, 0);
    p->pending = remaining;
}
    goto done;
reset:
    WirelessController_ResetSession(p);
    goto done;
stop:
    setOperation(p, 30, 0);
    goto done;
error:
    sub_082238F0(1);
    setOperation(p, 28, 0);
    gWirelessSessionState = 0;
done:;
}
