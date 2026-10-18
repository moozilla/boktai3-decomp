#include "global.h"

struct S {
    u8 f[0xae];
    u8 a;
};
extern const u32 gUnk_08605014[];
void sub_0824923C(struct S *, u32);
void GridAttack_DispatchNormalAction(struct S *p)
{
    sub_0824923C(p, gUnk_08605014[p->a]);
}
