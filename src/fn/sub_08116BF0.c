#include "global.h"
struct Q { u8 f[0x5c]; u8 a[4]; u8 b[0x14]; s32 v; };
struct G { u8 f[0x758]; s16 h; };
extern struct Q *gUnk_020004F0;
extern struct G *gUnk_02000710;
void sub_0811C41C(void *, void *);
void sub_08116BF0(void)
{
    struct Q *q = gUnk_020004F0;
    if (q != 0) {
        q->v = gUnk_02000710->h;
        sub_0811C41C(q->a, q->b);
    }
}
