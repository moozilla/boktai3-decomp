#include "global.h"

struct P {
    u8 f[0x2dc];
    u32 flags;
};
void SceneActorReaction_HandleColliderContact(s32, s32);

void Vorn_OnCollisionResponse(s32 a, s32 b, struct P *p)
{
    u32 f;
    SceneActorReaction_HandleColliderContact(a, b);
    f = p->flags;
    if (f & 0x20)
        p->flags = f | 0x80000;
}
