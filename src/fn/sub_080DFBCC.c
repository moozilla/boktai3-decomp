#include "global.h"

struct P {
    u8 f[6];
    u16 flags;
};
void SceneActorReaction_HandleColliderContact(struct P *, s32, s32);

static inline u32 tst(u16 *a, u32 m)
{
    return *a & m;
}

void Dog_OnBodyCollision(struct P *p, s32 a, s32 b)
{
    if (tst(&p->flags, 0x800) == 0)
        SceneActorReaction_HandleColliderContact(p, a, b);
}
