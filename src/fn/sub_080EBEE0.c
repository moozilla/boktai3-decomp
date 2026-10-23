#include "global.h"
#include "scene_actor_reaction.h"

void Octopus_ReturnToRoutePoint(void);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
void SceneActorTerrain_ResumeDrawable(struct SceneActorRegistryResetView *);

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s)
{
    u8 *state = &actor->state2A8;
    u32 zero = 0;
    *state = s;
    actor->phase2A9 = zero;
    actor->behavior2AA = zero;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}

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

struct Delay {
    u8 pad[8];
    u8 a, b, c, d;
    u32 ticks;
    void *cb;
};

/* Octopus: settle into state 3, return to the route point after 60 ticks,
   restore the alternate pose after 14, and raise the drawable flag. */
void sub_080EBEE0(struct SceneActorRegistryResetView *actor)
{
    SetState(actor, 3);
    {
        struct Delay *delay = (void *)((u8 *)actor + 0x460);
        void *cb = Octopus_ReturnToRoutePoint;
        delay->ticks = 60;
        delay->cb = cb;
        delay->a = 3;
        delay->b = 0;
        delay->c = 0;
    }
    SetCtl(actor, SceneActorReaction_RestoreAlternateAfterDelay, 14);
    SceneActorTerrain_ResumeDrawable(actor);
    {
        u8 *a = (u8 *)actor;
        if (a[8] == 0)
            **(u32 **)(a + 4) |= 1;
        else {
            u32 *d = *(u32 **)(a + 4) + 8;
            d[2] |= 1;
        }
    }
}
