#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 90.1%
// fix.py best version (reprinted C)
// Control-pointer allocation, spill ordering, and initial flag operations still differ.
#include "scene_actor_chase.h"
// confidence=low: recovery animation restores retreat, chase, or the saved route.
struct RecoverySequence {
    u8 pad[7];
    u8 mode;
    u16 frame;
    u8 padA[4];
    u16 timer;
};
static inline void SaveDelay(u8 *saved, void *target, u32 behavior, u32 delay)
{
    *(u32 *)(saved + 0xC) = delay;
    *(void **)(saved + 0x10) = target;
    saved[8] = 0;
    saved[9] = 0;
    saved[10] = behavior;
}
static inline void SetFlags(u16 *field, u32 mask)
{
    *field = mask | *field;
}
static inline void ClearFlags(u16 *field, u32 mask)
{
    *field = *field & ~mask;
}
static inline u16 MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 value = actor->flags2D4;
    value &= mask;
    return value;
}
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, u8 *group)
{
    void *route = *(void **)(group + 0x1FC);
    struct ContactPosition *position = &actor->body.position;
    u32 mask = 0x200;
    struct ContactPosition *target = *(struct ContactPosition **)(group + mask);
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        ClearFlags(&actor->flags2D4, mask);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->body.motion, 8);
        return 1;
    }
    ClearFlags(&actor->flags2D4, mask);
    return 0;
}
void Mummy_ResumeRouteAfterRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct RecoverySequence *sequence = (void *)(((u8 *)actor->drawable) + 0x48);
    u16 *reactionFlags;
    if (actor->field2B2)
        actor->field2B2 = 0;
    SetFlags(&actor->flags2D6, 0x10);
    if ((((frame <= 0x11) && (!sequence->frame)) && (sequence->timer == (sequence->mode != 0))) &&
        (sequence->mode != 1))
        sequence->timer = 0;
    reactionFlags = &actor->flags2D6;
    if (actor->field2B3) {
        u8 *group = (u8 *)(&actor->group.common);
        actor->field2B0 = 1;
        if (MotionFlags(actor, 2)) {
            Chase_SetState(actor, 3, 0, 0);
            Chase_SetCtl(actor, *((void **)(((u8 *)actor) + 0x5DC)), 7);
            ClearFlags(reactionFlags, 0x983);
            return;
        }
        if (MotionFlags(actor, 0x80)) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            Chase_SetBehavior(actor, 14);
            Chase_SetCtl(actor, *((void **)(group + 0x48)), 0x25);
            return;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Chase_SetState(actor, 3, 1, 0);
        Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
        SaveDelay(group + 0x8C, actor->entryTarget228, 7, 0x28);
        {
            u32 mask = 0x10;
            actor->flags2CC |= mask;
        }
        Chase_SnapToRoute(*((struct ContactPosition **)(((u8 *)actor) + 0x5D4)),
                          ((u8 *)actor) + 0x448);
        if (!TryRoute(actor, group)) {
            Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 0);
            Chase_SetState(actor, 3, 0, 0);
        } else
            (*((struct ContactPosition **)(group + 0x200)))->pad = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Mummy_ResumeRouteAfterRecovery);
#endif
