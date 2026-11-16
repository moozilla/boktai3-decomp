#include "scene_actor_reset.h"

static inline void SetControl(struct ChaseActor *actor, void *cb, u32 k)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = k;
    actor->base.target = cb;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void SetState(struct ChaseActor *actor, u32 s, u32 p, u32 b)
{
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
s32 SceneActorPlacement_BeginDefaultResponse(struct ChaseActor *p)
{
    if (p->base.reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x112);
    if (p->base.field2DC & 0x40000) {
        u32 mask = 32;
        u16 *flags = &p->base.flags2D4;
        mask |= *flags;
        *flags = mask;
    }
    SetControl(p, p->base.reactionTarget230, 18);
    SetState(p, 3, 0, 0);
    {
        u32 *flags = &p->base.flags2D0;
        u32 mask = ~0x1000000;
        *flags &= mask;
    }
    {
        u16 *flags = &p->base.flags2D6;
        s32 mask = (s32)0xFFFFF636;
        mask &= *flags;
        *flags = mask;
    }
    return 1;
}
