#include "global.h"

struct S {
    u8 f[0x384];
    u8 a;
};
struct S *SceneActors_GetBySlot(u32);
void Vampire_DecrementBarrierCount(void)
{
    struct S *p = SceneActors_GetBySlot(0);
    if (p && p->a != 0)
        p->a--;
}
