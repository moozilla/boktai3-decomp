#include "global.h"

struct P { u8 f00[0xb8]; s32 st; u8 sub[0x14C - 0xBC]; u32 w14c; };
struct G { u8 f00[0x77C]; s32 w; };
extern struct G *gUnk_02000710;

void sub_08215284(void *, s32);
void sub_0822B2F8(s32);

s32 sub_08162D58(struct P *p)
{
    struct P *q = p;
    if (gUnk_02000710->w <= 0)
        return 0;
    if (p->st != 3) {
        p->w14c = 0;
        p->st = 3;
    }
    sub_08215284(q->sub, 0x65);
    sub_0822B2F8(0x198);
    return 1;
}
