#include "scene_actor_control_phase.h"

#ifdef NONMATCHING
// fuzzy: 98.1%
// foundry draft (worker best 98.1%); what differs is mostly register allocation and instruction order

s32 BouncingObject_GetNearbyHeading(const struct ContactPosition *);
void SceneActor_StartAdjacentCellRoute(struct SceneActorRegistryResetView *);

s32 BouncingObject_RefreshNearbyHeading(struct SceneActorRegistryResetView *actor)
{
    u16 *counter = (u16 *)((u8 *)SceneActorControl_GetState(actor) + 0x6D4);
    u16 saved = *counter;
    if (saved) {
        *counter = saved - 1;
        return 0;
    }
    if (BouncingObject_GetNearbyHeading(&actor->position) < 0) {
        u16 *flags = (u16 *)((u8 *)SceneActorControl_GetState(actor) + 0x6F2);
        u32 mask = ~0x400;
        *flags &= mask;
        *counter = saved;
        return 0;
    }
    {
        u8 *control = (u8 *)SceneActorControl_GetState(actor);
        u16 mask = 0x400;
        *(u16 *)(control + 0x6F2) |= mask;
    }
    SceneActor_StartAdjacentCellRoute(actor);
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", BouncingObject_RefreshNearbyHeading);
#endif
