#define RouteActor_SetDestination Lun_BeginMoveToPosition
#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
void Lun_UpdateWaitingAnimation(void);
void Lun_BeginMoveToPosition(void *, void *);
struct RouteActorDestinationState {
    u8 unknown0[0x442];
    u16 delayDuration, delay;
};
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u16 SetFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 *flags = &actor->base.flags2D4;
    return *flags = mask | *flags;
}
void RouteActor_UpdatePlayerDestinationPhase(struct SceneActorRegistryResetView *actor)
{
    if (TakeFlag(&actor->base.field2B1)) {
        struct RouteActorDestinationState *state = actor->base.context;
        void *callback = Lun_UpdateWaitingAnimation;
        u32 kind = 10;
        actor->base.field2B2 = 1;
        actor->base.field2B0 = 0;
        actor->base.field2B5 = kind;
        actor->base.target = callback;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.word104 = 0;
        SetFlag(actor, 0x100);
        state->delay = state->delayDuration;
    }
    if (actor->base.field2B0)
        Lun_BeginMoveToPosition(actor, ((u8 **)gPlayers)[0] + 0x30);
    else
        actor->base.counter2AC++;
}
