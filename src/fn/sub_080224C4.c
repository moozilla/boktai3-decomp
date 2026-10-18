#include "global.h"

struct S {
    u8 f[0xaa];
    u8 a;
    u8 g[0x298];
    u8 b;
};
void SceneActorState_SetValueClampedWhenActive(struct S *p, u32 v)
{
    if (p->b == 1)
        v = 3;
    p->a = v;
}
