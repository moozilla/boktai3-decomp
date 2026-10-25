#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fuzzy: 91.8%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: restores a saved patrol destination and delayed command.
// confidence=low: tentative Vorn behavior role recovered from the surrounding callbacks.
// Remaining differences: terrain destination spill, flag-address reuse, and reset scheduling; savedMotion need not be volatile.
void Vorn_SendMessage(void *);
struct VornPatrolRoute {
    u8 unknown0[8];
    const u16 *points;
};
static inline u32 Bits(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2CC & mask;
}
static inline u8 CanRestore(struct SceneActorReactionMotion *m, u32 mask)
{
    if (m->flags & mask)
        return 1;
    return 0;
}
static inline void Mark(struct SceneActorRegistryResetView *a, u32 mask)
{
    a->flags2CC |= mask;
}
static inline void ClearMotion(struct SceneActorRegistryResetView *a, s32 mask)
{
    a->flags2D4 &= mask;
}
inline static u32 Bits_1(struct SceneActorRegistryResetView *a)
{
    return a->flags2CC & 0x200000;
}

s32 Vorn_TryRestorePatrolRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group);
    u8 *context = actor->context;
    struct SceneActorReactionMotion *motion;
    struct SceneActorReactionMotion *savedMotion;
    struct ContactPosition *destination;
    s16 k1;
    if (((!Bits(actor, 0x400000)) || (!Bits(actor, 0x200000))) ||
        (!CanRestore(motion = (void *)(&actor->fieldDC), 1)))
        return 0;
    savedMotion = motion;
    Mark(actor, 0x100);
    actor->field2C2 = 64;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    Chase_SetState(actor, 3, 1, 0);
    Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    Mark(actor, 16);
    {
        struct SceneActorReactionCommand *command = (void *)(group + 0x8C);
        void *payload = actor->savedTarget;
        u32 kind = 9;
        command->duration = 30;
        command->payload = payload;
        command->first = 0;
        command->second = 0;
        command->kind = kind;
    }
    destination = *((struct ContactPosition **)(group + (k1 = 0x200)));
    {
        struct VornPatrolRoute *route = (void *)(group + 0x74);
        destination->x = ((u16 *)route->points)[0];
        destination->z = ((u16 *)route->points)[1];
    }
    destination->y = Chase_Height(destination);
    (*((struct ContactPosition **)(group + k1)))->pad = 1;
    actor->field2C2 = 64;
    *((u16 *)(((u8 *)actor) + 0x414)) = 0;
    *((u16 *)(((u8 *)actor) + 0x418)) = 0;
    ClearMotion(actor, ~0x380);
    {
        struct SceneActorReactionMotion *m = savedMotion;
        s32 mask = ~0x18;
        m->flags &= mask;
    }
    {
        struct SceneActorRegistryControl *control = &actor->group.control;
        u32 *flags = &actor->flags2D0;
        if (Bits_1(actor)) {
            {
                u32 k2 = 0x1000000;
                *flags &= ~k2;
            }
            ClearMotion(actor, ~k1);
            *(((u8 *)control) + 10) = 0;
            control->counter = 0;
            control->timer = 0;
        }
        ClearMotion(actor, ~0x400);
        *(((u8 *)actor) + 0x5DA) = 0xFF;
        ClearMotion(actor, ~0x1000);
        *((u16 *)(context + 0x6D4)) = 0;
        *((u16 *)(context + 0x69E)) = 0;
        *((u16 *)(context + 0x6AC)) = 0;
        {
            s32 mask = ~4;
            *flags &= mask;
        }
    }
    Vorn_SendMessage(actor);
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", Vorn_TryRestorePatrolRoute);
#endif
