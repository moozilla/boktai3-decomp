#include "scene_actor_control_phase.h"
// confidence=low: refreshes the nearby route heading after the context cooldown expires.

s32 BouncingObject_GetNearbyHeading(const struct ContactPosition *);
void SceneActor_StartAdjacentCellRoute(struct SceneActorRegistryResetView *);

s32 BouncingObject_RefreshNearbyHeading(struct SceneActorRegistryResetView *arg)
{
    struct SceneActorRegistryResetView *actor = arg;
    struct SceneActorControlState **slot = &((struct SceneActorControlPhaseView *)actor)->control;
    u8 *context = (u8 *)*slot;
    u16 *counter;
    u32 v;
    u32 saved;
    counter = (u16 *)(context + 0x6D4);
    v = *counter;
    saved = (unsigned short)v;
    if (saved) {
        *counter = v - 1;
        return 0;
    }
    if (BouncingObject_GetNearbyHeading(&actor->body.position) < 0) {
        u16 *flags = (u16 *)((u8 *)*slot + 0x6F2);
        u32 mask = ~0x400;
        *flags &= mask;
        *counter = saved;
        return 0;
    }
    {
        u8 *control = (u8 *)*slot;
        u32 mask = 0x400;
        u16 *flags = (u16 *)(control + 0x6F2);
        *flags = mask | *flags;
    }
    SceneActor_StartAdjacentCellRoute(actor);
    return 1;
}
