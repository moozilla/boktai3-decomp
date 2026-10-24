// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#define FIELD(p, n, t) (*(t *)((u8 *)(p) + (n)))
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
// confidence=low: selects a route return phase from motion and route flags.
#define SceneActorRoute_UpdateContactRecovery sub_081314E8
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
/* Flag lifetimes, partial-vector scheduling and repeated control tails still differ. */
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
static inline void State(struct SceneActorRegistryResetView *a, u32 behavior, u32 zero)
{
    u8 *field = &a->state2A8;
    u32 value = 0;
    *field = value;
    a->phase2A9 = value;
    a->behavior2AA = behavior;
    a->counter2AC = value;
    a->field2B1 = 1;
}
static inline u32 Finished(struct SceneActorRegistryResetView *a)
{
    register u32 offset asm("r3") = 0x2B0;
    asm("" : "+r"(offset));
    return FIELD(a, offset, u8);
}
static inline u16 MotionFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2D4 & mask;
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
            Memory_Clear(&actor->motion, 8);
        return 1;
    }
    *flags &= ~mask;
    return 0;
}
void SceneActorRoute_UpdateContactRecovery(struct SceneActorRegistryResetView *actor)
{
    register u8 *group asm("sl") = (u8 *)(&actor->group);
    u8 result;
    u16 *motionFlags;
    asm("" : "+r"(group));
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
    if ((u8)Callback_InvokeActor(actor, actor->update270))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if (FlagsSave(actor, 0x1000, &motionFlags)) {
        struct ContactPosition delta;
        struct ContactPosition *p =
            (void *)((*((u8 **)(*((void **)(((u8 *)group) + 0x88))))) + 0xC);
        SceneActorTerrain_Subtract(&delta, p, &actor->position);
        if (((u32)((delta.x * delta.x) + (delta.z * delta.z))) <= 0xFFFE) {
            BeginControl(actor, *((void **)(((u8 *)group) + 0x238)), 0x19, result);
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
        if ((*motionFlags) & 0x1000) {
            Callback_InvokeActor(actor, *((void **)(((u8 *)group) + 0x244)));
            return;
        }
    }
    if (((s32)actor->counter2AC) > 60) {
        u32 destinationOffset;
        struct ContactPosition **destination;
        State(actor, 7, 0);
        BeginControlZero(actor, actor->savedTarget, 1);
        destinationOffset = 0x200;
        destination = (void *)(group + destinationOffset);
        (*destination)->pad = 1;
        if (!TryRoute(actor, *((void **)(((u8 *)group) + 0x1FC)), &actor->position, *destination,
                      motionFlags, destinationOffset)) {
            Callback_InvokeActor(actor, *((void **)(((u8 *)group) + 0x224)));
            return;
        }
        if (actor->flags2CC & 0x200000) {
            u8 *control = group + 0x4C;
            {
                u32 *flags = &actor->flags2D0;
                u32 mask = ~0x1000000;
                *flags &= mask;
            }
            *motionFlags &= ~0x200;
            control[0xA] = 0;
            *((u32 *)(((u8 *)control) + 0x18)) = 0;
            *((u16 *)(((u8 *)control) + 8)) = 0;
        }
    } else
        actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081314E8);
#endif
