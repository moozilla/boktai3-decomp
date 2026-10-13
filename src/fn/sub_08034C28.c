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
#define gRfuLinkStatus     gUnk_03006A80
#define gRfuSlotStatusNI   gUnk_03006A70
#define rfu_NI_setSendData sub_08246CC8
#define rfu_clearSlot      sub_08246AAC
#include "rfu_compat.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
u16 sub_08246C68(u8, u8, void *, u32);
int sub_082236C8(int, int, int, const u8 *);
extern const u8 gUnk_0824DE60[];
int sub_08034C28(struct Engine *p)
{
    int raw;
    int status;
    int result;
    if (p->pending) {
        int i;
        void *buffer;
        p->pending = 0;
        i = 0;
        buffer = p->work;
        do {
            sub_08246C68(32, (u8)i, buffer, 4);
            i++;
        } while (i <= 3);
    }
    raw = sub_082236C8(1, 0, 600, gUnk_0824DE60);
    {
        register int narrowed asm("r1") = raw;
        asm("" : "+r"(narrowed));
        status = (u8)narrowed;
    }
    if (!status)
        goto success;
    setOperation(p, 27, 0);
    result = -1;
    goto done;
success: {
    register int operation asm("r1") = 7;
    register int one asm("r1");
    p->flag = status;
    p->operation = operation;
    p->ticks = status;
    {
        register u8 *pending asm("r2") = &p->pending;
        one = 1;
        *pending = one;
    }
}
    result = raw;
done:
    return result;
}
