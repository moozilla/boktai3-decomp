#include "scene_actor_reset.h"

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = k;
    actor->target = cb;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
s32 SceneActorPlacement_BeginDefaultResponse(struct SceneActorRegistryResetView *p)
{
    if (p->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x112);
    if (p->field2DC & 0x40000) {
        u32 mask = 32;
        u16 *flags = &p->flags2D4;
        mask |= *flags;
        *flags = mask;
    }
    SetControl(p, p->reactionTarget230, 18);
    SetState(p, 3, 0, 0);
    {
        u32 *flags = &p->flags2D0;
        u32 mask = ~0x1000000;
        *flags &= mask;
    }
    {
        u16 *flags = &p->flags2D6;
        s32 mask = (s32)0xFFFFF636;
        mask &= *flags;
        *flags = mask;
    }
    return 1;
}
