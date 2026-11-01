// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_attachment.h"
#include "debug.h"

// confidence=low: preserves the existing tentative reaction-state interpretation.

static inline void SetState(struct SceneActorRegistryResetView *actor, u8 s, u8 p, u8 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = cb;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
// confidence=low: timed movement recovery restores a route and saves a delayed control.
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
static inline void SaveControl(u8 *packet, void *control, u8 behavior)
{
    *(u32 *)(packet + 12) = 30;
    *(void **)(packet + 16) = control;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = behavior;
}
void SceneActorReaction_UpdateRouteRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    u8 result;
    u32 *savedFlags;
    if (Chase_TakeArrived(actor)) {
        DEBUG_PRINT();
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if ((s32)actor->counter2AC > 60) {
        struct ContactPosition target;
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = ~0x100;
            *flags &= mask;
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
        SetState(actor, 3, 1, 0);
        SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = 0x10;
            *flags |= mask;
        }
        {
            struct ContactPosition *p = &target;
            u16 *route = *(u16 **)(group + 0x7c);
            p->x = route[0];
            p->z = route[1];
            {
                s32 x = p->x >> 8;
                s32 z = p->z >> 8;
                savedFlags = &actor->flags2CC;
                p->y = Chase_HeightAt(Chase_CellIndex(x, z), p);
            }
        }
        {
            register u32 height asm("r1") = (u16)actor->body.position.y;
            target.y = height;
        }
        Chase_TryRoute(actor, *(void **)(group + 0x1fc), &actor->body.position, &target);
        (*(struct ContactPosition **)(group + 0x200))->pad = 1;
        if (!(u8)SceneActorAttachment_TestPositionWindow(actor, &target, 32, 32)) {
            {
                u32 *flags = savedFlags;
                u32 mask = ~0xc0;
                *flags &= mask;
            }
            SaveControl(group + 0x8c, actor->entryTarget228, 7);
        } else {
            {
                u32 *flags = savedFlags;
                u32 mask = ~0xd0;
                *flags &= mask;
            }
            SaveControl(group + 0x8c, actor->savedTarget, 0);
        }
        actor->field2C2 = 64;
    }
    actor->counter2AC++;
}