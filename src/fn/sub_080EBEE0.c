#include "global.h"
#include "scene_actor_reaction.h"

void Octopus_ReturnToRoutePoint(void);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
void SceneActorTerrain_ResumeDrawable(struct ChaseActor *);

static inline void SetState(struct ChaseActor *actor, u32 s)
{
    u8 *state = &actor->base.state2A8;
    u32 zero = 0;
    *state = s;
    actor->base.phase2A9 = zero;
    actor->base.behavior2AA = zero;
    actor->base.counter2AC = zero;
    actor->base.field2B1 = 1;
}

static inline void SetCtl(struct ChaseActor *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *field = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
}

struct Delay {
    u8 pad[8];
    u8 a, b, c, d;
    u32 ticks;
    void *cb;
};

/* Octopus: settle into state 3, return to the route point after 60 ticks,
   restore the alternate pose after 14, and raise the drawable flag. */
void sub_080EBEE0(struct ChaseActor *actor)
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
