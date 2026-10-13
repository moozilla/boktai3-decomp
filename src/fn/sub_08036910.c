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
void sub_0822C1C8(void);
void *sub_08219C40(int);
void sub_08219DD8(void *, int);
int sub_08228DA0(void);
int sub_080368AC(struct Engine *);
void sub_0821A04C(void *, int, int);
void sub_08219F74(void *);
void sub_08219D38(void *);
extern u32 gUnk_03005308, gUnk_0300546C;
extern const u16 gUnk_0203B400[];
extern u8 gUnk_03003A50;
struct Engine *sub_08036910(void)
{
    struct Engine **singleton = &gUnk_02000484;
    struct Engine *existing = *singleton;
    struct Engine *p;
    int size;
    int random;
    if (existing)
        return existing;
    sub_0822C1C8();
    size = 0x38C;
    p = sub_08219C40(size);
    if (!p)
        goto failed;
    asm("" : "+r"(p));
    sub_08219DD8(p, size);
    random = sub_08228DA0();
    {
        u32 *position = &gUnk_03005308;
        u32 index = *position + 1;
        u32 mask = 1023;
        const u16 *table;
        int term;
        const u8 *saltAddress;
        int byteMask;
        int salt;
        u16 *destination;
        index &= mask;
        *position = index;
        index <<= 1;
        table = gUnk_0203B400;
        term = *(const u16 *)(index + (u32)table);
        saltAddress = &gUnk_03003A50;
        byteMask = 255;
        salt = *saltAddress;
        random += term;
        random &= byteMask;
        random <<= 8;
        salt |= random;
        destination = (u16 *)((u8 *)p + 0x6C);
        *destination = salt;
    }
    if (sub_080368AC(p) >= 0) {
        int one;
        sub_0821A04C(p, 0, 0);
        one = 1;
        ((u8 *)p)[0x14] = one;
        ((u8 *)p)[0x16] = one;
        *(u16 *)((u8 *)p + 0x10) = (u32)existing;
        sub_08219F74(p);
        gUnk_0300546C = one;
        *singleton = p;
        return p;
    }
    sub_08219D38(p);
failed:
    return 0;
}
