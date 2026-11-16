#include "global.h"
#include "scene_actor_reaction.h"

// confidence=low: stagger reaction; once it finishes, hands control to
// SceneActor_EndReactionAnimation (kind 0x23) and restarts behaviour 3.

void SceneActor_EndReactionAnimation(void);

static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *field = &actor->base.flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.word104 = 0;
}

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void Octopus_UpdateStagger(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->base.drawable + 0x48);
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 3);
        actor->base.field2B3 = 0;
        {
            u16 *d = &actor->base.colliderA.flags;
            u32 flags = *d;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *d = mask;
        }
        {
            u32 bit = 0x800;
            actor->base.flags2C0 = bit | actor->base.flags2C0;
        }
        {
            u32 bit = 0x20;
            actor->base.flags2D4 = *(&bit) | actor->base.flags2D4;
        }
    }
    {
        u32 bit = 0x10;
        actor->base.flags2D6 = *(&bit) | actor->base.flags2D6;
    }
    if (frame <= 17 && sequence->index == 0) {
        u16 elapsed = sequence->elapsed;
        u8 duration = sequence->duration;
        if (elapsed == (duration != 0) && duration != 1)
            sequence->elapsed = 0;
    }
    if (actor->base.field2B3) {
        actor->base.field2B0 = 0;
        {
            u16 bit = 0x20;
            actor->base.flags2D4 &= ~bit;
        }
        {
            u32 *field = &actor->base.flags2CC;
            u32 mask = ~0x80;
            *field &= mask;
        }
        SetCtl(actor, SceneActor_EndReactionAnimation, 0x23);
        SetState(actor, 1, 0, 3);
        actor->base.body.direction = actor->base.tracking.heading;
    }
}
