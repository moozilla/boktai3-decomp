#include "scene_actor_chase.h"

// confidence=low: restores a saved patrol destination and delayed command.
// confidence=low: tentative Vorn behavior role recovered from the surrounding callbacks.
void Vorn_SendMessage(void *);
struct VornPatrolRoute {
    u8 unknown0[8];
    const u16 *points;
};
static inline u32 Vorn_TestRestoreFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2CC & mask;
}
static inline u8 Vorn_GetRestorableMotion(struct SceneActorReactionMotion *m, u32 mask,
                                          struct SceneActorReactionMotion **saved)
{
    u32 bits = m->flags & mask;
    *saved = m;
    if (bits)
        return 1;
    return 0;
}
static inline void Vorn_SetRestoreFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    a->flags2CC |= mask;
}

s32 Vorn_TryRestorePatrolRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group);
    u8 *context = actor->context;
    struct SceneActorReactionMotion *savedMotion;
    struct ContactPosition *destination;
    if (((!Vorn_TestRestoreFlags(actor, 0x400000)) || (!Vorn_TestRestoreFlags(actor, 0x200000))) ||
        (!Vorn_GetRestorableMotion((void *)(&actor->tracking.target), 1, &savedMotion)))
        return 0;
    Vorn_SetRestoreFlags(actor, 0x100);
    actor->field2C2 = 64;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    Chase_SetState(actor, 3, 1, 0);
    Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    Vorn_SetRestoreFlags(actor, 16);
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
    destination = *((struct ContactPosition **)(group + 0x200));
    {
        struct VornPatrolRoute *route = (void *)(group + 0x74);
        destination->x = ((u16 *)route->points)[0];
        destination->z = ((u16 *)route->points)[1];
    }
    destination->y = Chase_Height(destination);
    (*((struct ContactPosition **)(group + 0x200)))->pad = 1;
    actor->field2C2 = 64;
    actor->group.flags414 = 0;
    actor->group.field418 = 0;
    Chase_ClearBits(&actor->flags2D4, 0x380);
    {
        struct SceneActorReactionMotion *m = savedMotion;
        s32 mask = ~0x18;
        m->flags &= mask;
    }
    {
        struct SceneActorRegistryControl *control = &actor->group.control;
        u32 restoreBits = Vorn_TestRestoreFlags(actor, 0x200000);
        u32 *flags = &actor->flags2D0;
        if (restoreBits) {
            {
                u32 k2 = 0x1000000;
                *flags &= ~k2;
            }
            Chase_ClearBits(&actor->flags2D4, 0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        Chase_ClearBits(&actor->flags2D4, 0x400);
        actor->field5DA = 0xFF;
        Chase_ClearBits(&actor->flags2D4, 0x1000);
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
