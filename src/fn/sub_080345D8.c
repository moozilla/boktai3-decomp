// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
#include "rfu_manager.h"
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
    u8 pad4c[0xA0];
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
struct ModeState {
    u8 pad0;
    u8 mode;
};
extern struct ModeState *gRfuLinkStatus;
static inline void reset(struct Engine *p, int operation, int zero)
{
    p->flag = zero;
    p->operation = operation;
    p->ticks = zero;
    p->pending = 1;
}
void WirelessController_OnLinkEvent(u8 command)
{
    int key = command;
    struct Engine *p = gWirelessController;
    if (!p)
        goto done;
    if (key == 0x33)
        goto release;
    if (key > 0x33)
        goto high;
    if (key == 0x13)
        goto reset5;
    if (key > 0x13)
        goto middle;
    if (key == 0x10)
        goto done;
    if (key > 0x10)
        goto lowerOne;
    if (key == 0)
        goto begin;
    goto done;
lowerOne:
    if (key == 0x11)
        goto addBits;
    goto done;
middle:
    if (key == 0x30)
        goto release;
    if (key > 0x30)
        goto flags;
    if (key == 0x14)
        goto mode;
    goto done;
flags:
    if (key == 0x31)
        goto selectedAdd;
    if (key == 0x32)
        goto selectedRemove;
    goto done;
high:
    if (key == 0x44)
        goto reset30;
    if (key > 0x44)
        goto endCommands;
    if (key == 0x41)
        goto done;
    if (key > 0x41)
        goto highPair;
    if (key == 0x40)
        goto releaseOther;
    goto done;
highPair:
    if (key == 0x42)
        goto reset32;
    if (key == 0x43)
        goto reset5Other;
    goto done;
endCommands:
    if (key == 0xF1)
        goto checkStop;
    if (key > 0xF1)
        goto lastCommands;
    if (key == 0x50)
        goto inactive;
    if (key == 0xF0)
        goto checkStop;
    goto done;
lastCommands:
    if (key == 0xF2)
        goto checkStop;
    if (key == 0xFF)
        goto stopIfRunning;
    goto done;

begin:
    reset(p, 5, key);
    goto done;
addBits: {
    register int incoming asm("r1") = gRfuLinkManager.event.bytes.bits;
    p->bits = p->bits | incoming;
}
    goto done;
reset5:
    reset(p, 5, 0);
    goto done;
mode: {
    int mode = gRfuLinkStatus->mode;
    if (!mode)
        reset(p, 5, mode);
    else
        reset(p, 9, 0);
}
    goto done;
selectedAdd: {
    u8 *slot = &p->selected;
    register int incoming asm("r1");
    incoming = gRfuLinkManager.event.bytes.bits;
    *slot = *slot | incoming;
}
    goto done;
selectedRemove: {
    u8 *slot = &p->selected;
    int incoming;
    incoming = gRfuLinkManager.event.bytes.bits;
    *slot = *slot & ~incoming;
}
    goto done;
release: {
    register struct RfuLinkManager *message asm("r2") = &gRfuLinkManager;
    u8 *selected;
    int incoming;
    incoming = message->event.bytes.bits;
    p->bits &= ~incoming;
    selected = &p->selected;
    incoming = message->event.bytes.bits;
    *selected &= ~incoming;
    {
        int zero = p->bits;
        if (!zero) {
            int op = 11;
            p->flag = zero;
            p->operation = op;
            p->ticks = zero;
            p->pending = 1;
        }
    }
}
    goto done;
releaseOther: {
    register struct RfuLinkManager *message asm("r2") = &gRfuLinkManager;
    u8 *selected;
    int incoming;
    incoming = message->event.bytes.bits;
    p->bits &= ~incoming;
    selected = &p->selected;
    incoming = message->event.bytes.bits;
    *selected &= ~incoming;
    {
        int zero = p->bits;
        if (!zero) {
            int op = 11;
            p->flag = zero;
            p->operation = op;
            p->ticks = zero;
            p->pending = 1;
        }
    }
}
    goto done;
reset32:
    reset(p, 32, 0);
    goto done;
reset5Other:
    reset(p, 5, 0);
    goto done;
reset30:
    reset(p, 30, 0);
    goto done;
stopIfRunning:
    if (p->operation == 28 || p->operation <= 1)
        goto inactive;
    goto stop;
checkStop:
    if (p->operation == 28)
        goto inactive;
stop:
    reset(p, 28, 0);
inactive:
    gWirelessSessionState = 0;
    goto done;
done:;
}
