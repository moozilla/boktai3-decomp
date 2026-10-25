// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 86.2%
// fix.py best (sweep after the inlined-insn fix)
// Route-slot and flag-pointer lifetimes still differ; candidate is four bytes short.
#include "scene_actor_chase.h"
// confidence=low: context flag 0x628 requests a delayed restart from the route origin.
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
void SceneActor_SetPositionXZ(void *, s32, s32);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (actor->flags2D0 & mask)
            return 1;
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline void SaveDelay(u8 *packet, void *control, u32 behavior, u32 delay)
{
    *(u32 *)(packet + 12) = delay;
    *(void **)(packet + 16) = control;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = behavior;
}
static inline u8 TestPending(u8 *context, u32 mask, u16 **pending)
{
    *pending = (void *)(context + 0x628);
    if (**pending & mask)
        return 1;
    return 0;
}
inline static u8 TestPending_1(u8 *context, u32 mask, u16 *pending_)
{
    u16 **pending = &pending_;
    *pending = (void *)(context + 0x628);
    if ((*(*pending)) & mask)
        return 1;
    return 0;
}

s32 Ghoul_ResumePendingRoute(struct SceneActorRegistryResetView *actor)
{
    register u8 *context asm("sl") = actor->context;
    u8 *group = (u8 *)(&actor->group);
    u16 *flags;
    u16 *pending;
    u16 *timer;
    void *route;
    if (Unavailable(actor))
        return 1;
    if (!TestPending_1(context, 1, pending))
        return 1;
    {
        u32 mask = 0xA107;
        flags = &actor->flags2D4;
        if (mask & (*flags))
            return 1;
    }
    timer = (void *)(context + 0x648);
    if (*timer)
        (*timer)--;
    else {
        {
            u32 *p = &actor->flags2D0;
            u32 mask = ~4;
            *p = mask & (*p);
        }
        route = *((void **)(group + 0x1FC));
        SceneActor_SetPositionXZ(&actor->id, *((s16 *)(((u8 *)route) + 0x1C)),
                                 *((s16 *)(((u8 *)route) + 0x20)));
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SaveDelay(group + 0x8C, *((void **)(group + 0x48)), 12, 30);
        Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        Chase_SetState(actor, 3, 1, 0);
        SceneBody_StartRoute(&actor->id, group + 0x74, group[0x84], group[0x85], group[0x86]);
        {
            void *route = *((void **)(group + 0x1FC));
            struct ContactPosition *position = &actor->position;
            struct ContactPosition *target = (void *)(context + 0x62C);
            s32 result = Nav_IsPositionInMesh(route, position, target);
            if (!result) {
                u32 mask = ~0x200;
                *flags = mask & (*flags);
            } else {
                result = Terrain_ApplyBlocked(route, position, target);
                if (result) {
                    if (result != 2)
                        Memory_Clear(&actor->motion, 8);
                } else {
                    u32 mask = ~0x200;
                    *flags = mask & (*flags);
                }
            }
        }
        {
            u32 mask = ~1;
            *pending = mask & (*pending);
        }
    }
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", Ghoul_ResumePendingRoute);
#endif
