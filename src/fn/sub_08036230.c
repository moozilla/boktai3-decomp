// MATCHING_CONCESSION: register-pinning
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
void sub_082156B8(int);
void sub_08224DE8(void *);
void sub_0803614C(u8);
void sub_08036218(void);
void sub_08224BB8(void (*)(u8));
void sub_08224BA0(void (*)(void));
void sub_0821AD08(void *, int);
void sub_08036230(struct Engine *p)
{
    int index;
    u16 *ids;
    struct RfuLinkStatus **links;
    u8 *buffer;
    void *listener;
    int zero;
    sub_082156B8(0);
    index = 0;
    {
        register int offset asm("r0") = 0x16A;
        u16 *first;
        struct RfuLinkStatus **initial;
        register u16 *mine asm("r0");
        first = (u16 *)((u8 *)p + offset);
        initial = &gUnk_03006A80;
        mine = &(*initial)->my.id;
        if (*first != *mine) {
            struct RfuLinkStatus **cursor = initial;
            register int offset asm("r1") = 0x16A;
            u16 *next;
            next = (u16 *)((u8 *)p + offset);
            do {
                next++;
                index++;
                if (index > 3)
                    break;
            } while (*next != (*(struct RfuLinkStatus *volatile *)cursor)->my.id);
        }
    }
    zero = 0;
    p->flag1f = index;
    sub_08224DE8(p->message);
    sub_08219DD8((u8 *)p + 0xB8, 12);
    buffer = (u8 *)p + 0xC4;
    sub_08219DD8(buffer, 12);
    sub_08224BB8(sub_0803614C);
    sub_08224BA0(sub_08036218);
    rfu_setRecvBuffer(32, p->index, buffer, 12);
    p->phase = 2;
    setOperation(p, 24, zero);
    listener = *(void **)((u8 *)p + 0x4C);
    if (listener)
        sub_0821AD08(listener, 0);
}
