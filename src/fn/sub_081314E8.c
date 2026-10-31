#include "global.h"
#include "scene_actor_control_phase.h"
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
// confidence=low: selects a route return phase from motion and route flags.
static inline void BeginControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind,
                                u32 zero)
{
    u32 offset;
    {
        u8 *field = &actor->field2B2;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void BeginControlZero(struct SceneActorRegistryResetView *actor, void *control,
                                    u32 kind)
{
    u32 offset;
    u32 zero;
    {
        u8 *field = &actor->field2B2;
        zero = 0;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 behavior)
{
    u8 *field = &a->state2A8;
    u32 value = 0;
    *field = value;
    a->phase2A9 = value;
    a->behavior2AA = behavior;
    a->counter2AC = value;
    a->field2B1 = 1;
}
#include "scene_actor_chase.h"
#include "scene_actor_attachment.h"
static inline u32 FlagsSave(struct SceneActorRegistryResetView *a, u32 mask, u16 **out)
{
    u16 *field = &a->flags2D4;
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
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                          struct ContactPosition *position, struct ContactPosition *target,
                          u16 *flags, u32 mask)
{
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        *flags &= ~mask;
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->body.motion, 8);
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
#ifdef NONMATCHING
// fuzzy: 99.1%
// scope shape 5 (differences once low registers are set aside)
void SceneActorRoute_UpdateContactRecovery(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u8 result;
    u16 *motionFlags;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        BeginControlZero(actor, actor->savedTarget, 0);
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = ~0x80;
            *flags &= mask;
        }
        {
            u32 *flags = &actor->flags2D0;
            u32 mask = ~0x1000000;
            *flags &= mask;
        }
        {
            u16 *flags = &actor->flags2D4;
            u32 mask = ~0x200;
            *flags &= mask;
        }
        actor->field2C2 = 64;
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if (FlagsSave(actor, 0x1000, &motionFlags)) {
        struct ContactPosition delta;
        struct ChaseTarget **node = (struct ChaseTarget **)group->contactNode;
        SubtractContactNode(&delta, node, &actor->body.position);
        if (((u32)((delta.x * delta.x) + (delta.z * delta.z))) <= 0xFFFE) {
            BeginControl(actor, group->field60C, 0x19, result);
            {
                u32 state = 3;
                u8 *field = &actor->state2A8;
                *field = state;
            }
            {
                typeof(actor->phase2A9) *q1 = &actor->phase2A9;
                u8 z1 = 0;
                *q1 = 1;
                actor->behavior2AA = z1;
                actor->counter2AC = result;
                {
                    typeof(actor->field2B1) *q2 = &actor->field2B1;
                    *q2 = 1;
                }
                return;
            }
        }
    }
    if (actor->counter2AC == 30) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
        {
            u32 mask = 0x1000;
            if (TestMotion(motionFlags, mask)) {
                Callback_InvokeActor(actor, group->interactionCallback);
                return;
            }
        }
    }
    if (((s32)actor->counter2AC) > 60) {
        u32 destinationOffset;
        struct ContactPosition **destination;
        State(actor, 7);
        BeginControlZero(actor, actor->entryTarget228, 1);
        destinationOffset = 0x200;
        destination = (void *)((u8 *)group + destinationOffset);
        (*destination)->pad = 1;
        if (!TryRoute(actor, group->route, &actor->body.position, *destination, motionFlags,
                      destinationOffset)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
        {
            struct SceneActorRegistryControl *control = &group->common.control;
            u32 mask = 0x200000;
            if (actor->flags2CC & mask) {
                {
                    u32 *flags = &actor->flags2D0;
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
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081314E8);
#endif
