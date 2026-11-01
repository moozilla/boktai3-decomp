#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_attachment.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
static inline void SaveControl(u8 *packet, void *control, u8 behavior)
{
    *(u32 *)(packet + 12) = 30;
    *(void **)(packet + 16) = control;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = behavior;
}
static inline void ClearContextFlag(u16 *flags, u32 zero)
{
    s32 mask = zero - 2;
    mask &= *flags;
    *flags = mask;
}
static inline u32 SaveStoppedControl(u8 *packet, void *control)
{
    u32 zero;
    *(u32 *)(packet + 12) = 30;
    *(void **)(packet + 16) = control;
    zero = 0;
    packet[8] = zero;
    packet[9] = zero;
    packet[10] = zero;
    return zero;
}
static inline void clearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}
static inline u32 testFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

static inline void ClearSavedFlags(u32 *flags, s32 mask)
{
    *flags &= mask;
}
static inline void *UpdateCallback(struct SceneActorRegistryResetView *actor)
{
    return *(void **)((u8 *)actor + 0x270);
}
static inline u8 Mimic_FinishRoute(struct SceneActorRegistryResetView *actor, s32 result,
                                   void *route, struct ContactPosition *position,
                                   struct ContactPosition *target)
{
    if (!result) {
        Chase_ClearBits(&actor->flags2D4, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->body.motion, 8);
        return 1;
    }
    Chase_ClearBits(&actor->flags2D4, 0x200);
    return 0;
}
static inline void Mimic_SaveRouteControl(struct SceneActorRegistryResetView *actor, u8 *context,
                                          u8 *group, u32 *flags)
{
    u8 outside = SceneActorAttachment_TestPositionWindow(
        (void *)actor, (struct ContactPosition *)(context + 0x650), 32, 32);
    if (!outside) {
        ClearSavedFlags(flags, ~0xc0);
        SaveControl(group + 0x8c, actor->entryTarget228, 2);
    } else {
        u32 zero;
        ClearSavedFlags(flags, ~0xc0);
        zero = SaveStoppedControl(group + 0x8c, actor->savedTarget);
        ClearContextFlag((void *)(context + 0x65c), zero);
    }
    actor->field2C2 = 0x40;
}
// confidence=low: provisional name; recovery after a hit (state 3 -> delayed restore), Mimic-owned.

void Mimic_UpdateHitRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    if (Chase_TakeArrived(actor)) {
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        u16 *flags;
        motion = (void *)&actor->tracking.target;
        motion->fieldDC = 0;
        {
            u32 o = 0x2C2;
            u16 *h;
            u32 v;
            h = (u16 *)((u8 *)actor + o);
            v = 0x40;
            *h = v;
        }
        {
            u32 offset = 0x414;
            u16 *field;
            field = (void *)((u8 *)actor + offset);
            *field = 0;
        }
        {
            u32 o = 0x418;
            u16 *h;
            h = (u16 *)((u8 *)actor + o);
            *h = 0;
        }
        flags = &actor->flags2D4;
        {
            u32 mask = ~0x380;
            mask &= *flags;
            *flags = mask;
        }
        {
            s32 mask = ~0x18;
            mask &= motion->flags;
            motion->flags = mask;
        }
        control = &actor->group.common.control;
        if (testFlags(actor, 0x200000)) {
            clearRouteFlag(&actor->flags2D0);
            {
                u32 mask = ~0x200;
                mask &= *flags;
                *flags = mask;
            }
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        {
            u32 mask = ~0x400;
            mask &= *flags;
            *flags = mask;
        }
        actor->group.turn.turnsRemaining = 0xFF;
    }
    if ((s32)actor->counter2AC > 30) {
        u8 *group = (u8 *)actor + 0x3D4;
        u32 *cc = &actor->flags2CC;
        ClearSavedFlags(cc, ~0x100);
        {
            u8 *current = actor->context;
            u16 mask = 1;
            struct ChaseFlags40 *mode = (void *)(current + 0x65C);
            mode->bits |= mask;
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
        Chase_SetState(actor, 3, 1, 0);
        Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        {
            void *route = *(void **)((u8 *)actor + 0x5D0);
            struct ContactPosition *position = &actor->body.position;
            struct ContactPosition *target = (void *)(context + 0x650);
            s32 result = Nav_IsPositionInMesh(route, position, target);
            u32 *savedFlags = cc;
            if (Mimic_FinishRoute(actor, result, route, position, target))
                Mimic_SaveRouteControl(actor, context, group, savedFlags);
        }
    } else {
        Callback_InvokeActor(actor, UpdateCallback(actor));
        actor->counter2AC++;
    }
}
