// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"

#ifdef NONMATCHING
// fuzzy: 98.1%
// Remaining difference: cmp uses r0 rather than the r7 copy of the counter.
s32 BouncingObject_GetNearbyHeading(const struct ContactPosition *);
void SceneActor_StartAdjacentCellRoute(struct SceneActorRegistryResetView *);

s32 BouncingObject_RefreshNearbyHeading(struct SceneActorRegistryResetView *arg)
{
    register struct SceneActorRegistryResetView *actor asm("r6") = arg;
    struct SceneActorControlState **slot = &((struct SceneActorControlPhaseView *)actor)->control;
    register u16 *counter asm("r5") = (u16 *)((u8 *)*slot + 0x6D4);
    register u16 v asm("r0") = *counter;
    register u16 saved asm("r7") = v;
    asm("" : "+r"(v));
    if (saved) {
        *counter = v - 1;
        return 0;
    }
    if (BouncingObject_GetNearbyHeading(&actor->position) < 0) {
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
#else
INCLUDE_ASM("asm/nonmatching", BouncingObject_RefreshNearbyHeading);
#endif
