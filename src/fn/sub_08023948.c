#include "global.h"
void Trigger_ConsumePending(void *, u32);
struct S {
    u8 f0[0xaa];
    u8 aa;
    u8 f1[4];
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 padC8[0x2cc];
    u8 particle;
};

static inline u8 chk(struct S *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void sub_08023948(struct S *arg)
{
    struct S *p = arg;
    struct S *base = p;
    if (chk(p)) {
        u32 k = 6;
        u8 *state = &p->aa;
        u32 zero;
        zero = 0;
        *state = k;
        base->particle = zero;
    }
    if (p->af)
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    else
        p->c4++;
}
