#include "global.h"

struct S {
    u8 f[0xae];
    u8 a;
};
extern const u32 gUnk_08605014[];
void Callback_InvokeActor(struct S *, u32);
void GridAttack_DispatchNormalAction(struct S *p)
{
    Callback_InvokeActor(p, gUnk_08605014[p->a]);
}
