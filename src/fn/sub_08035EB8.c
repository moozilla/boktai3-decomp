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
void Memory_Clear(void *, int);
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
void Bg_ClearTilemap(int);
void sub_08224DE8(void *);
void sub_08035D58(u8);
void sub_08035EAC(void);
void sub_08224BB8(void (*)(u8));
void sub_08224BA0(void (*)(void));
void Script_Call(void *, int);
void sub_0822B058(int);
void sub_08035EB8(struct Engine *p)
{
    u8 *scratch = p->scratch;
    int i;
    register int bit asm("r6");
    void *listener;
    Bg_ClearTilemap(0);
    sub_08224DE8(p->message);
    Memory_Clear((u8 *)p + 0xB8, 12);
    Memory_Clear((u8 *)p + 0xC4, 12);
    rfu_clearSlot(8, p->mode);
    sub_08224BB8(sub_08035D58);
    sub_08224BA0(sub_08035EAC);
    i = 0;
    do {
        int mask;
        int active = scratch[2];
        bit = 2;
        mask = bit << i;
        if (active & mask)
            rfu_setRecvBuffer(32, i, (u8 *)p + 0xC4, 12);
    } while (++i <= 3);
    p->phase = bit;
    setOperation(p, 23, 0);
    listener = *(void **)((u8 *)p + 0x4C);
    if (listener)
        Script_Call(listener, 0);
    sub_0822B058(4);
}
