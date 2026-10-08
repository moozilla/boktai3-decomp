#include "global.h"

struct G { u8 f[0x12]; s16 a; };
extern struct G *gUnk_02000710;
struct S { u8 f[8]; u8 a; u8 b; };
void sub_08032C6C(struct S *p)
{
    s32 v;
    p->a = 0;
    v = gUnk_02000710->a;
    p->b = v;
}
