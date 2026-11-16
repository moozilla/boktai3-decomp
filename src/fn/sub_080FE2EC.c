#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
void Axe_UpdateWaitingAnimation(void);
void Axe_BeginMoveToPosition(u8 *s, s16 *v);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
static inline u16 SetFlag(struct ChaseActor *actor, u32 mask)
{
    u16 *flags = &actor->base.motionFlags;
    return *flags = mask | *flags;
}
void Axe_UpdatePlayerDestinationPhase(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (TakeFlag(&actor->base.field2B1)) {
        void *callback = Axe_UpdateWaitingAnimation;
        u32 kind = 10;
        actor->base.field2B2 = 1;
        actor->base.stepDone = 0;
        actor->base.controlKind = kind;
        actor->base.target = callback;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = 0;
        SetFlag(actor, 0x100);
        group->gate.counter = group->gate.duration;
    }
    if (actor->base.stepDone)
        Axe_BeginMoveToPosition(actor, ((u8 **)gPlayers)[0] + 0x30);
    else
        actor->base.counter2AC++;
}
