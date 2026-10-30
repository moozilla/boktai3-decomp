#include "global.h"
#include "scene_actor_reaction.h"

// confidence=low: stagger reaction; once it finishes, hands control to
// SceneActor_EndReactionAnimation (kind 0x23) and restarts behaviour 3.

void SceneActor_EndReactionAnimation(void);

static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
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

void Octopus_UpdateStagger(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->drawable + 0x48);
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 3);
        actor->field2B3 = 0;
        {
            u16 *d = &actor->colliderA.flags;
            u32 flags = *d;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *d = mask;
        }
        {
            u32 bit = 0x800;
            actor->flags2C0 = bit | actor->flags2C0;
        }
        {
            u32 bit = 0x20;
            actor->flags2D4 = *(&bit) | actor->flags2D4;
        }
    }
    {
        u32 bit = 0x10;
        actor->flags2D6 = *(&bit) | actor->flags2D6;
    }
    if (frame <= 17 && sequence->index == 0) {
        u16 elapsed = sequence->elapsed;
        u8 duration = sequence->duration;
        if (elapsed == (duration != 0) && duration != 1)
            sequence->elapsed = 0;
    }
    if (actor->field2B3) {
        actor->field2B0 = 0;
        {
            u16 bit = 0x20;
            actor->flags2D4 &= ~bit;
        }
        {
            u32 *field = &actor->flags2CC;
            u32 mask = ~0x80;
            *field &= mask;
        }
        SetCtl(actor, SceneActor_EndReactionAnimation, 0x23);
        SetState(actor, 1, 0, 3);
        actor->body.direction = actor->tracking.heading;
    }
}
