#include "global.h"

struct Actor080A5FC0 {
    u8 pad0[0x2b0];
    u8 done;
    u8 pad2b1;
    u8 pending;
    u8 ready;
    u8 pad2b4[0x2d4 - 0x2b4];
    u16 flags;
    u16 flags2;
};
void sub_08076034(void *, u32);

static inline u8 Consume(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Clr(u16 *f, s32 m)
{
    *f &= m;
}

static inline void Or(u16 m, u16 *f)
{
    *f |= m;
}

void sub_080A5FC0(struct Actor080A5FC0 *p, s32 time)
{
    if (Consume(&p->pending)) {
        sub_08076034(p, 7);
        Clr(&p->flags, ~1);
    }
    if (time == 40)
        Or(16, &p->flags2);
    if (p->ready && time > 120)
        p->done = 1;
}
