#include "global.h"
#include "scene_actor_control_phase.h"
u8 sub_0812F3CC(struct ChaseActor *);
// confidence=low: selects a route return phase from motion and route flags.
static inline void BeginControl(struct ChaseActor *actor, void *control, u32 kind, u32 zero)
{
    u32 offset;
    {
        u8 *field = &actor->base.field2B2;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->base.controlKind = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}
static inline void BeginControlZero(struct ChaseActor *actor, void *control, u32 kind)
{
    u32 offset;
    {
        u8 *field = &actor->base.field2B2;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = 0;
    actor->base.controlKind = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}
static inline void State(struct ChaseActor *a, u32 behavior)
{
    u8 *field = &a->base.state2A8;
    u32 value = 0;
    *field = value;
    a->base.phase2A9 = value;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = value;
    a->base.field2B1 = 1;
}
#include "scene_actor_chase.h"
#include "scene_actor_attachment.h"
static inline u32 FlagsSave(struct ChaseActor *a, u32 mask, u16 **out)
{
    u16 *field = &a->base.motionFlags;
    u32 value = *field & mask;
    *out = field;
    return value;
}
static inline u16 TestMotion(u16 *field, u32 mask)
{
    u32 value = mask;
    value &= *field;
    return value;
}
static inline u8 TryRoute(struct ChaseActor *actor, void *route, struct ContactPosition *position,
                          struct ContactPosition *target, u16 *flags, u32 mask)
{
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        *flags &= ~mask;
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->base.body.motion, 8);
        return 1;
    }
    *flags &= ~mask;
    return 0;
}
static inline void SubtractContactNode(struct ContactPosition *out, struct ChaseTarget **node,
                                       struct ContactPosition *from)
{
    struct ChaseTarget *target = *node;
    SceneActorTerrain_Subtract(out, &target->position, from);
}
// scope shape 5 (differences once low registers are set aside)
void SceneActorRoute_UpdateContactRecovery(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u8 result;
    u16 *motionFlags;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        BeginControlZero(actor, actor->base.onStanding, 0);
        {
            u32 *flags = &actor->base.behaviorFlags;
            u32 mask = ~0x80;
            *flags &= mask;
        }
        {
            u32 *flags = &actor->base.statusFlags;
            u32 mask = ~0x1000000;
            *flags &= mask;
        }
        {
            u16 *flags = &actor->base.motionFlags;
            u32 mask = ~0x200;
            *flags &= mask;
        }
        actor->base.field2C2 = 64;
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if (FlagsSave(actor, 0x1000, &motionFlags)) {
        struct ContactPosition delta;
        struct ChaseTarget **node = (struct ChaseTarget **)group->contactNode;
        SubtractContactNode(&delta, node, &actor->base.body.position);
        if (((u32)((delta.x * delta.x) + (delta.z * delta.z))) <= 0xFFFE) {
            BeginControl(actor, group->contactArrivalControl, 0x19, result);
            {
                u32 state = 3;
                u8 *field = &actor->base.state2A8;
                *field = state;
            }
            {
                typeof(actor->base.phase2A9) *q1 = &actor->base.phase2A9;
                u8 z1 = 0;
                *q1 = 1;
                actor->base.behavior2AA = z1;
                actor->base.counter2AC = result;
                {
                    typeof(actor->base.field2B1) *q2 = &actor->base.field2B1;
                    *q2 = 1;
                }
                return;
            }
        }
    }
    if (actor->base.counter2AC == 30) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
        {
            u32 mask = 0x1000;
            if (TestMotion(motionFlags, mask)) {
                void (*callback)(struct ChaseActor *) = group->interactionCallback;
                callback(actor);
                return;
            }
        }
    }
    if (((s32)actor->base.counter2AC) > 60) {
        u32 destinationOffset;
        struct ContactPosition **destination;
        State(actor, 7);
        BeginControlZero(actor, actor->base.onAlternateEntry, 1);
        destinationOffset = 0x200;
        destination = (void *)((u8 *)group + destinationOffset);
        (*destination)->pad = 1;
        if (!TryRoute(actor, group->route, &actor->base.body.position, *destination, motionFlags,
                      destinationOffset)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
        {
            struct SceneActorRegistryControl *control = &group->common.control;
            u32 mask = 0x200000;
            if (actor->base.behaviorFlags & mask) {
                {
                    u32 *flags = &actor->base.statusFlags;
                    u32 mask = ~0x1000000;
                    *flags &= mask;
                }
                {
                    u32 mask = ~0x200;
                    struct ChaseFlags40 *flags = (void *)motionFlags;
                    flags->bits = mask & flags->bits;
                }
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
        }
    } else
        actor->base.counter2AC++;
}
