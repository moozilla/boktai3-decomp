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
extern u8 gUnk_020000EC;
void sub_08219DD8(void *, int);
#define gRfuLinkStatus   gUnk_03006A80
#define gRfuSlotStatusNI gUnk_03006A70
#include "rfu_compat.h"
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
void sub_08035320(struct Engine *);
void sub_0803548C(struct Engine *);
void sub_08035438(struct Engine *);
void sub_080356AC(struct Engine *, u8);
void sub_080340AC(struct Engine *);
int sub_082238F0(int);
// MATCHING_CONCESSION: register-pinning
void sub_08035864(u8 command)
{
    register int key asm("r3") = command;
    int saved = key;
    struct Engine *p = gUnk_02000484;
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
        goto checkError;
    goto done;
lastMore:
    if (saved == 0xF2)
        goto error;
    if (saved == 0xFF)
        goto error;
    goto done;
receive:
    sub_08035438(p);
    goto done;
added: {
    u8 *slots = &p->selected;
    register int incoming asm("r1") = (u8)gUnk_03005390.code;
    *slots |= incoming;
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
    int zero;
    int incoming = (u8)gUnk_03005390.code;
    int selected = *slots & ~incoming;
    zero = 0;
    *slots = selected;
    sub_082238F0(1);
    setOperation(p, 29, zero);
}
    goto done;
reset:
    sub_080340AC(p);
    goto done;
checkError: {
    struct Event *event;
    struct Event *initial = &gUnk_03005390;
    int code = initial->code;
    event = initial;
    if (code != 0x30)
        goto valueCheck;
    if (event->value != 3)
        goto valueCheck;
    event->value = 0;
valueCheck:;
    if (!event->value)
        goto done;
}
error:
    sub_082238F0(1);
    setOperation(p, 28, 0);
done:;
}
