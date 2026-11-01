/* Tentative: passes the call through unless the actor is unavailable or flagged. */
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_script.h"
void SceneActorReaction_HandleColliderContact(void *, void *);
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
void SceneActor_TryForward(void *a, void *b, struct SceneActorRegistryResetView *actor)
{
    struct SceneActorScriptPropertiesView *state = actor->context;
    if (IsUnavailable(actor))
        return;
    if (Test(StateFlag(state, 0x10), ~0)) {
        actor->context++;
        actor->context--;
        return;
    }
    if (*((u8 *)state + 0x6D0))
        return;
    SceneActorReaction_HandleColliderContact(a, b);
}
