#include "global.h"

struct G { u8 f[0x85e]; u16 v; };
struct S { u8 f[0x30]; u16 a; u16 b; u16 c; };
extern struct G *gUnk_02000710;

void sub_080508F0(struct S *p)
{
    p->a = gUnk_02000710->v;
    p->b = 300;
    p->c = 0;
}
