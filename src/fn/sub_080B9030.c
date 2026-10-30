/* Tentative reaction name, based on the observed fields and calls. */
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_script.h"
void SceneActorControl_BeginTimedBehavior56(struct SceneActorRegistryResetView *,
                                            struct SceneActorScriptPropertiesView *);
u8 SceneActorControl_CanTurnTowardTarget(struct SceneActorRegistryResetView *);
static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}
static inline u8 IsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 StateFlag(struct SceneActorScriptPropertiesView *p, u32 mask)
{
    return p->flags6DC & mask;
}
static inline u32 HalfFlag(u16 *p, u32 mask)
{
    return *p & mask;
}
static inline u32 WordFlag(u32 *p, u32 mask)
{
    return *p & mask;
}
void SceneActorReaction_TryStartContact(u8 *source, u16 *contact,
                                        struct SceneActorRegistryResetView *actor)
{
    struct SceneActorScriptPropertiesView *state = actor->context;
    if (IsUnavailable(actor))
        return;
    if (HalfFlag(&contact[3], 4))
        return;
    if (((struct SceneActorRegistryActor *)actor)->field178)
        return;
    if (Test(StateFlag(state, 0x10), ~0))
        return;
    if (ActorFlag(actor, 0xA13F, 0x2D4))
        return;
    if (WordFlag((u32 *)(source + 0x34), 0x10))
        return;
    if (!SceneActorControl_CanTurnTowardTarget(actor))
        return;
    SceneActorControl_BeginTimedBehavior56(actor, state);
    *((u8 *)state + 0x6D0) = 20;
    actor->field2B6 = actor->tracking.heading + 0x80;
}
