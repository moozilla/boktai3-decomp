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
#include "rfu_compat.h"
#include "ram.h"
#include "rfu_manager.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    {
        u8 *pending = &p->pending;
        *pending = 1;
    }
}
void Wireless_UpdateUniAcknowledgedSlots(struct Engine *);
void Wireless_ReceiveParentUniPackets(struct Engine *);
void Wireless_ProcessChildUniExchange(struct Engine *);
void RfuManager_SendData(int);
void Wireless_RemoveConnectedSlots(struct Engine *, u8);
void WirelessController_ResetSession(struct Engine *);
int sub_082238F0(int);
void Wireless_OnChildTransferEvent(u8 command)
{
    int key = command;
    struct Engine *p = gWirelessController;
    if (!p)
        goto done;
    if (key > 0x41)
        goto high;
    /* Range switch retains the original strict comparison
                       boundary. */
    switch (key) {
        case 0x40 ... 0x7FFFFFFF:
            goto done;
        default:
            break;
    }
    if (key == 0x30)
        goto disconnected;
    if (key <= 0x30)
        goto done;
    if (key == 0x32)
        goto removed;
    switch (key) {
        case 0x32 ... 0x7FFFFFFF:
            break;
        default:
            goto added;
    }
    if (key == 0x33)
        goto disconnected;
    goto done;
high:
    if (key == 0x50)
        goto receive;
    if (key > 0x50)
        goto last;
    if (key == 0x42)
        goto reset;
    goto done;
last:
    if (key == 0xF1)
        goto error;
    if (key > 0xF1)
        goto lastMore;
    if (key == 0xF0)
        goto error;
    goto done;
lastMore:
    if (command == 0xF2)
        goto error;
    if (command == 0xFF)
        goto error;
    goto done;
receive:
    RfuManager_SendData(1);
    goto done;
added: {
    u8 *slots = &p->selected;
    *slots |= (u8)gRfuLinkManager.event.words.code;
}
    goto done;
removed: {
    u8 *slots = &p->selected;
    int incoming = (u8)gRfuLinkManager.event.words.code;
    *slots &= ~incoming;
}
    goto done;
disconnected: {
    u8 *slots = &p->selected;
    int zero;
    int incoming = (u8)gRfuLinkManager.event.words.code;
    int selected = *slots & ~incoming;
    zero = 0;
    *slots = selected;
    setOperation(p, 29, zero);
}
    goto done;
reset:
    WirelessController_ResetSession(p);
    goto done;
error:
    if (p->operation != 28)
        setOperation(p, 28, 0);
done:;
}
