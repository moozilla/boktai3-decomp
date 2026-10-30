// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
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
static inline void ClearFlags(u16 *field, u32 mask)
{
    *field = *field & ~mask;
}
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, u8 *group)
{
    void *route = *(void **)(group + 0x1FC);
    struct ContactPosition *position = &actor->body.position;
    register u32 mask asm("r8") = 0x200;
    struct ContactPosition *target;
    s32 result;
    target = *(struct ContactPosition **)(group + mask);
    result = Nav_IsPositionInMesh(route, position, target);
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
struct RecoveryCounter {
    s16 value : 16;
};
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
// confidence=low: restarts the saved route after a timed animation and waits before restoring control.
void Mummy_ResumeSavedRouteAfterAnimation(struct SceneActorRegistryResetView *actor, s32 frame)
{
    void *body;
    u8 *group = (u8 *)&actor->group.common;
    struct RecoverySequence *sequence;
    body = (u8 *)actor + 0x50;
    sequence = (void *)((u8 *)actor->drawable + 0x48);
    if (SceneActorReaction_TakePrepared(actor)) {
        register u8 *reaction asm("r2") = group + 0x4C;
        {
            register u32 *flags asm("r3") = &actor->flags2CC;
            u32 mask = ~0x141C0;
            asm("" : "+r"(mask) : "r"(flags), "r"(reaction));
            *flags &= mask;
        }
        *(u32 *)(reaction + 0x18) = 0;
        *(u16 *)(reaction + 0x14) = 0;
        *(u16 *)(reaction + 8) = 0;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 7);
        actor->field2B3 = 0;
    }
    if (frame <= 0x3B) {
        sequence->frame = 0;
        sequence->timer = 0;
    }
    if (actor->field2B3) {
        ((struct RecoveryCounter *)((u8 *)actor + 0xAA))->value &= 0;
        actor->field2B0 = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
        Chase_SetState(actor, 3, 1, 0);
        {
            u8 *route = group + 0x74;
            SceneBody_StartRoute(body, route, group[0x84], group[0x85], group[0x86]);
            Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), route);
        }
        if (!TryRoute(actor, group)) {
            Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 0);
            Chase_SetState(actor, 3, 0, 0);
        } else {
            (*(struct ContactPosition **)(group + 0x200))->pad = 1;
            {
                u32 mask = 0x10;
                actor->flags2CC |= mask;
            }
            SaveDelay(group + 0x8C, actor->entryTarget228, 7, 0x3C);
        }
    }
}
